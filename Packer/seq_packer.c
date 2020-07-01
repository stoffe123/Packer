#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#include <inttypes.h>
#include "packer_commons.h"
#include "seq_packer_commons.h"
#include "common_tools.h"
#include "seq_packer.h"

#define VERBOSE false

/* Sequence packer */

/* Global variables used in compressor */
__declspec(thread) static FILE* infil, * utfil;

__declspec(thread) static const wchar_t* src_name;

__declspec(thread) static  buffer_size = BLOCK_SIZE * 3;
__declspec(thread) static  unsigned char buffer[BLOCK_SIZE * 3];

__declspec(thread) static size_t absolute_end, buffer_endpos;

__declspec(thread) static const wchar_t* base_dir = L"c:/test/";
__declspec(thread) static bool separate_files = false;

__declspec(thread) static uint32_t nextChar[BLOCK_SIZE * 2],
lastChar[256],
distances[BLOCK_SIZE],
distanceFreq[BLOCK_SIZE],
distancesPos,

offsets[BLOCK_SIZE],
offsetFreq[BLOCK_SIZE],
offsetsPos,

seqlens[BLOCK_SIZE],
seqlenFreq[BLOCK_SIZE],
seqlensPos;


static void updateNextCharTable(unsigned char ch, uint32_t pos) {
	uint32_t lastPos = lastChar[ch];
	if (lastPos == INT32_MAX) { // first occurence!
		lastChar[ch] = pos;
	}
	else {
		nextChar[lastPos] = pos - lastPos;
		assert(pos - lastPos > 0, "pos - lastPos > 0 seqpacker");
		lastChar[ch] = pos;
	}
}

static void display_progress(uint32_t buffer_pos, uint8_t pass) {
	if (buffer_pos % 48000 == 0 && pass == 2 && !VERBOSE) {
		printf("*");
	}
}

void writeMeta(FILE* file, uint64_t c) {
	if (c > 255) {
		printf("\n write_distance called with value > 255 was %d", c);
		exit(1);
	}
	//debug("\nwrite_offset:%d", c);
	fwrite(&c, 1, 1, (separate_files ? file : utfil));
}

void out_distance(uint64_t distance) {
	distances[distancesPos++] = distance;
	if (distanceFreq[distance] < UINT32_MAX) {
		distanceFreq[distance]++;
	}
}

void out_offset(uint64_t offset) {
	offsets[offsetsPos++] = offset;
	if (offsetFreq[offset] < UINT32_MAX) {
		offsetFreq[offset]++;
	}
}

void out_seqlen(uint64_t seqlen) {
	seqlens[seqlensPos++] = seqlen;
	if (seqlenFreq[seqlen] < UINT32_MAX) {
		seqlenFreq[seqlen]++;
	}
}

void convertMeta(FILE* file, unsigned long distance, pageCoding_t pageCoding) {

	unsigned char pages = pageCoding.pages;
	uint64_t pageMax = calcPageMax(pageCoding);
	uint64_t useLongRange = pageCoding.useLongRange;

	if (distance <= pageMax) {
		uint64_t last_byte = getLastByte(useLongRange);
		uint64_t lowest_special = getLowestSpecial(pageCoding);
		if (distance < lowest_special) {
			writeMeta(file, distance);
		}
		else {
			uint64_t page = 0;
			for (; page < pages; page++) {

				if (distance < (lowest_special + ((uint64_t)256 * (page + 1)))) {
					writeMeta(file, distance - (lowest_special + ((uint64_t)256 * page)));
					writeMeta(file, last_byte - page);//special code
					break;
				}
			}
			const char* msg = "seq_packer.out_distance: no coding found for distance:";
			assert(page < pages, msg);
		}
	}
	else {  // long range
		if (!useLongRange) {
			wprintf(L"\n seqpacker: long range was needed but not set!! distance %d pages %d pagemax %d when packing %s",
				distance, pages, pageMax, src_name);
			exit(1);
		}
		distance -= (pageMax + 1);

		if (useLongRange >= 3) {
			writeMeta(file, distance / 65536);
			distance %= 65536;
		}
		if (useLongRange >= 2) {
			writeMeta(file, distance / 256);
		}
		writeMeta(file, distance % 256);
		writeMeta(file, 255);
	}
}

uint64_t calcMetaSize(pageCoding_t pageCoding, uint64_t pagesMax, int32_t* freqTable, int32_t pos)
{
	uint64_t lowestSpecial = getLowestSpecial(pageCoding);	
	uint64_t size = 0;

	//1 bytes
	for (int i = 0; i < lowestSpecial; i++) {

		size += freqTable[i];

	}
	uint64_t freqs = size;

	// 2 bytes
	for (int i = lowestSpecial; i <= pagesMax; i++) {

		size += (freqTable[i] * (uint64_t)2);
		freqs += freqTable[i];

	}
	// long ranges
	uint64_t longRangeFreq = pos - freqs;
	if (pageCoding.useLongRange == 0 && longRangeFreq > 0) {
		wprintf(L"\n seqpacker.calcMetaSize: useLongRange == 0 && longRangeFreq > 0 when packing %s", src_name);
		exit(1);
	}
	size += (longRangeFreq * (pageCoding.useLongRange + 1));
	return size;
}


uint64_t calcUseLongRange(uint64_t pageMax, uint64_t highestDistance) {
	if (pageMax >= highestDistance) {
		return 0;
	}
	uint64_t max = highestDistance - (pageMax + 1);
	if (max < 256) {
		return 1;
	}
	if (max < 65536) {
		return 2;
	}
	return 3;
}

pageCoding_t createMetaFile(const wchar_t* metaname) {

	int32_t* freqs, * values, metaValuesCount;
	if (equalsw(metaname, L"offsets")) {
		freqs = offsetFreq;
		values = offsets;
		metaValuesCount = offsetsPos;
	}
	else if (equalsw(metaname, L"distances")) {
		freqs = distanceFreq;
		values = distances;
		metaValuesCount = distancesPos;
	}
	else if (equalsw(metaname, L"seqlens")) {
		freqs = seqlenFreq;
		values = seqlens;
		metaValuesCount = seqlensPos;
	}
	else {
		printf("\n wrong metaname in seq_packer.createMetaFile");
		exit(1);
	}

	//determine highest meta value
	uint64_t highestValue = BLOCK_SIZE - 1;
	while (highestValue > 0 && freqs[highestValue] == 0) {
		highestValue--;
	}
	//wprintf(L"\n Highest %s was %d", metaname, highestValue);
	

	uint32_t bestSize = UINT32_MAX;

	uint64_t bestPage = 0;
	uint64_t highestPageTry = 253;
	pageCoding_t bestPageCoding;
	pageCoding_t pageCoding;
	//test all pages!
	for (pageCoding.pages = 0; pageCoding.pages < highestPageTry; pageCoding.pages++) {

		//calc pageMax without using longRange to see if longRange could be skipped	
		pageCoding.useLongRange = false;
		uint64_t pageMax = calcPageMax(pageCoding);		
		pageCoding.useLongRange = calcUseLongRange(pageMax, highestValue);

		pageMax = calcPageMax(pageCoding);
		//update now that pageMax has changed
		pageCoding.useLongRange = calcUseLongRange(pageMax, highestValue);
		
		uint32_t size = calcMetaSize(pageCoding, pageMax, freqs, metaValuesCount);
		if (size < bestSize) {
			bestSize = size;
			bestPageCoding = pageCoding;			
		}
		//printf("\n pages %d gave size %d useOffsetLongRange %d", pages, size, useLongRange);
		if (highestValue < pageMax) {
			break;
		}
	}
	const wchar_t filename[100] = { 0 };
	concatw(filename, base_dir, metaname);
	FILE* file = openWrite(filename);
	
	for (int i = 0; i < metaValuesCount; i++) {		
		convertMeta(file, values[i], bestPageCoding);		
	}
	fclose(file);
	uint64_t filesize = get_file_size_from_wname(filename);
	if (bestSize != filesize) {
	    wprintf(L"\n   comparing %s meta file sizes ... %d %d when seqpacking file %s", metaname, bestSize, filesize, src_name);
		exit(1);
	}
	//wprintf(L"\n %s pages %d useLongrange %d", metaname, bestPageCoding.pages, bestPageCoding.useLongRange);
	return bestPageCoding;
}

uint64_t storeLongRange(uint64_t packType, uint64_t longRange,  uint64_t startBit) {

	if (longRange / 2 == 1) {
		packType = setKthBit(packType, startBit);
	}
	if (longRange % 2 == 1) {
		packType = setKthBit(packType, startBit + 1);
	}
	return packType;
}

//--------------------------------------------------------------------------------------------------------

void pack_internal(const wchar_t* src, const wchar_t* dest_filename, unsigned char pass, packProfile profile)
{
	src_name = src;
	unsigned int max_seqlen = 65791;
	bool superslim = false;
	if (get_file_size_from_wname(src) < profile.sizeMaxForSuperslim) {
		superslim = true;
		profile.seqlenMinLimit3 = SUPERSLIM_SEQLEN_MIN_LIMIT3;
	}
	
	uint64_t	offset,
		winsize = profile.winsize,
		best_offset = 0,
		min_seq_len = 3;

	int64_t  best_seqlen, seq_len;
	uint64_t distance = 1; // point one byte past the beginning!
	uint32_t buffer_pos = 0;
	debug("\n Seq_packer pass: %d", pass);

	infil = openRead(src);

	if (pass == 2) {

		debug("\nWinsize: %d", winsize);
		debug("\nmax_seqlen: %d", max_seqlen);

		utfil = openWrite(dest_filename);
	}

	printf("\n");
	/* start compression */
	long long size_org = get_file_size(infil);


	buffer_endpos = fread(&buffer, 1, buffer_size, infil);

	assertEqual(buffer_endpos, size_org, "buffer_endpos == size_org in seqpacker");

	assert(buffer_size > size_org * 2, "buffer_size > size_org * 2  in seqpacker");

	absolute_end = buffer_endpos;

	uint64_t offset_max;

	while (buffer_pos < buffer_endpos) {

		best_seqlen = 0;
		unsigned char ch = buffer[buffer_pos];
		uint64_t buffer_pos_plus1 = (uint64_t)1 + buffer_pos;
		uint64_t buffer_pos_plus2 = (uint64_t)2 + buffer_pos;
		unsigned char ch1 = buffer[buffer_pos_plus1];
		unsigned char ch2 = buffer[buffer_pos_plus2];
		

		if (pass == 2) {
			if ((buffer_endpos - buffer_pos) >= min_seq_len) {
				if (buffer_pos + winsize > absolute_end - min_seq_len) {

					offset_max = (absolute_end - min_seq_len) - buffer_pos;
				}
				else {
					offset_max = winsize;
				}

				offset = 0;
				int32_t nextChar_pos = buffer_pos, nextCh;
				long diff;
				while (offset < offset_max)
				{
					// find matching sequence		
					if (((nextCh = nextChar[nextChar_pos]) == 0) ||
						((offset += nextCh) >= offset_max)) {
						break;
					}
					nextChar_pos += nextCh;
					if (
						(ch1 != buffer[buffer_pos_plus1 + offset]) ||
						(ch2 != buffer[buffer_pos_plus2 + offset]) || offset < 3) {
						continue;
					}
					seq_len = 3;

					while (buffer[buffer_pos + seq_len] == buffer[buffer_pos + offset + seq_len] &&
						seq_len < offset &&
						buffer_pos + offset + seq_len < absolute_end - 1)
					{
						seq_len++;
					}
					diff = (buffer_pos + offset + seq_len) - absolute_end;
					if ((diff > 0) || ((diff = (buffer_pos + seq_len) - buffer_endpos) > 0)) {
						if (seq_len <= diff) {
							continue;
						}
						else {
							seq_len -= diff;
							assert(seq_len >= 0, "seq_len >= 0");
						}
					}
					//check if better than the best!

					if (seq_len > best_seqlen) { //&& (offset - seq_len) <= longest_offset) {
						/*
						if (offset >= offsetPagesMax && seq_len < 6) {
							continue;
						}
						*/
						best_seqlen = seq_len;
						best_offset = offset;
						if (best_seqlen >= max_seqlen) {
							best_seqlen = max_seqlen;						
							break;
						}
					}
				}
			}
		}
		/* now we found the longest sequence in the window! */
		best_offset -= best_seqlen;
		unsigned char seqlen_min = getSeqlenMin(best_offset, profile);

		if (best_seqlen < seqlen_min)
		{       /* no sequence found, move window 1 byte forward and read one more byte */
			if (pass == 1) {
				updateNextCharTable(ch, buffer_pos);
			}
			else {
				WRITE(utfil, ch);
				assert(absolute_end < buffer_size, "absolute_end < buffer_size in seqpacker");
				updateNextCharTable(ch, absolute_end);
				buffer[absolute_end++] = ch; // write start to end to wrap-around find sequences					
				distance++;
			}
			buffer_pos++;
			display_progress(buffer_pos, pass);
		}
		else { // insert code triple instead of the matching sequence!
			//debug("\n  seqlenmin:%d  offset:%d", seqlen_min, best_offset);
			assertSmallerOrEqual(buffer_pos + best_offset + best_seqlen + best_seqlen, absolute_end,
				"buffer_startpos + best_offset + best_seqlen*2 <= absolute_end in seq_packer.pack_internal");


			//debug("Found sequence seq_len=%d, offset=%d, at bufferstartpos=%d", best_seq_len, best_offset, buffer_startpos);

			if (pass == 2) {  // Write triplet!

				out_offset(best_offset);

				out_seqlen(best_seqlen - seqlen_min);
				if (VERBOSE) {
					printf("\n(%d, %d, %d)  buffer_startpos %d   buffer_endpos %d  seq \"", best_seqlen, best_offset, distance, buffer_pos, buffer_endpos);
					for (int i = 0; i < best_seqlen; i++) {
						printf("%d ", buffer[buffer_pos + i]);
					}
					printf("\"");
				}

				
				/* note file is read backwards during unpack! */
				out_distance(distance);
				distance = 0;
			}
			buffer_pos += best_seqlen;
			display_progress(buffer_pos, pass);
		}//end if
	}//end while

	if (VERBOSE && false) {
		printf("\nwrap around:\n");
		for (int i = buffer_endpos; i < absolute_end; i++) {
			printf("%c", buffer[i]);
		}
		printf("\n\n");
	}

	if (pass == 2) {

		out_distance(distance);
		debug("\n distance to first code: %d", distance);

		pageCoding_t pageCoding = createMetaFile(L"offsets");
		uint8_t offsetPages = pageCoding.pages;
		uint64_t useOffsetLongRange = pageCoding.useLongRange;

		pageCoding = createMetaFile(L"distances");
		uint8_t distancePages = pageCoding.pages;
		uint64_t useDistanceLongRange = pageCoding.useLongRange;

		pageCoding = createMetaFile(L"seqlens");
		uint8_t seqlenPages = pageCoding.pages;
		uint64_t useSeqlenLongRange = pageCoding.useLongRange;

		bool slimCase = (seqlenPages + offsetPages + distancePages == 0);
		if (!slimCase) {
			WRITE(utfil, seqlenPages);
			WRITE(utfil, offsetPages);
			WRITE(utfil, distancePages);
		}
		unsigned char packType = 0;
		packType = storeLongRange(packType, useOffsetLongRange, 1);
		packType = storeLongRange(packType, useDistanceLongRange, 3);
		packType = storeLongRange(packType, useSeqlenLongRange, 5);
		if (slimCase) {
			packType = setKthBit(packType, 0);
		}
		if (superslim) {
			packType = setKthBit(packType, 7);
		}
		if (!superslim) {
			WRITE(utfil, profile.seqlenMinLimit3);
		}
		else {
			printf("\n superslim used!");
		}
		WRITE(utfil, packType);
		fclose(utfil);
	}
	fclose(infil);
}

void initGlobalArrays() {
	for (int i = 0; i < 256; i++) {
		lastChar[i] = INT32_MAX;
	}
	for (int i = 0; i < BLOCK_SIZE * 2; i++) {
		nextChar[i] = 0;
	}

	for (int i = 0; i < BLOCK_SIZE; i++) {
		distances[i] = 0;
		distanceFreq[i] = 0;
		offsets[i] = 0;
		offsetFreq[i] = 0;
		seqlens[i] = 0;
		seqlenFreq[i] = 0;		
	}	
	distancesPos = 0;
	offsetsPos = 0;
	seqlensPos = 0;
}

void seq_pack_internal(const wchar_t* source_filename, const wchar_t* dest_filename, packProfile profile, bool sep) {
	long long source_size = get_file_size_from_wname(source_filename);
	separate_files = sep;
	initGlobalArrays();
	pack_internal(source_filename, dest_filename, 1, profile);
	pack_internal(source_filename, dest_filename, 2, profile);
}

void seq_pack(const char* src, const char* dst, packProfile profile)
{
	wchar_t u1[500], u2[500];
	toUni(u1, src);
	toUni(u2, dst);
	seq_pack_internal(u1, u2, profile, false);
}

void seqPack(const wchar_t* src, const wchar_t* dst, packProfile profile) {
	seq_pack_internal(src, dst, profile, false);
}

void seq_pack_separate(const char* src, const char* dir, packProfile profile) {
	wchar_t srcw[500], dirw[500];
	toUni(srcw, src);
	toUni(dirw, dir);
	seqPackSeparate(srcw, dirw, profile);
}

void seqPackSeparate(const wchar_t* src, const wchar_t* dir, packProfile profile) {
	const wchar_t dest_filename[100] = { 0 };
	concatw(dest_filename, dir, L"main");
	base_dir = dir;
	seq_pack_internal(src, dest_filename, profile, true);
}

