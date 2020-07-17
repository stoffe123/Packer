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
#include "memfile.h"

#define VERBOSE false

/* Sequence packer */

/* Global variables used in compressor */

__declspec(thread) static const wchar_t* src_name;

__declspec(thread) static size_t absolute_end, buffer_endpos;

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

void writeMeta(memfile* file, uint64_t c) {
	if (c > 255) {
		printf("\n write_distance called with value > 255 was %d", c);
		exit(1);
	}
	//debug("\nwrite_offset:%d", c);
	if (separate_files) {
		fputccLight(c, file);
	}
	else {
		//TODO  fix nonseparate
	}
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

void convertMeta(memfile* file, unsigned long distance, pageCoding_t pageCoding) {

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
			assert(page < pages, "seq_packer.out_distance: no coding found for distance");
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

pageCoding_t createMetaFile(const wchar_t* metaname, memfile* file) {

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
	bestPageCoding.pages = 0;
	bestPageCoding.useLongRange = false;
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
	for (int i = 0; i < metaValuesCount; i++) {		
		convertMeta(file, values[i], bestPageCoding);		
	}
	syncMemSize(file);
	uint64_t filesize = getMemSize(file);
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

seqPackBundle pack_internal(memfile* infil, unsigned char pass, packProfile profile)
{
	rewindMem(infil);
	
	unsigned int max_seqlen = 65791;
	bool superslim = false;
	uint64_t size_org = getMemSize(infil);
	assert(size_org > 0, "size_org negative in seqpacker");
	assert(size_org < BLOCK_SIZE, "size_org larger than BLOCK_SIZE in seqpacker");
	reallocMem(infil, size_org * 3);
	if (size_org < profile.sizeMaxForSuperslim) {
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

	
	seqPackBundle utfil;
	utfil.main = NULL;
	if (pass == 2) {

		debug("\nWinsize: %d", winsize);
		debug("\nmax_seqlen: %d", max_seqlen);

		utfil.main = getMemfile(size_org * 3 + 1000, L"seqpacker_utfilmain");		
		
	}

	printf("\n");
	/* start compression */
	
	buffer_endpos = size_org;

	absolute_end = buffer_endpos;

	uint64_t offset_max;

	while (buffer_pos < buffer_endpos) {

		best_seqlen = 0;
		uint8_t* buffer = infil->block;
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
				fputccLight(ch, utfil.main);
				//assert(absolute_end < BLOCK_SIZE, "absolute_end < BLOCK_SIZE in seqpacker");
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
			//printf("%c", buffer[i]);
		}
		printf("\n\n");
	}
	setSize(infil, size_org);
	if (pass == 2) {

		out_distance(distance);
		debug("\n distance to first code: %d", distance);

		utfil.seqlens = getMemfile(size_org / 2, L"seqpacker_utfilseqlens");
		utfil.offsets = getMemfile(size_org / 2, L"seqpacker_utfiloffsets");
		utfil.distances = getMemfile(size_org / 2, L"seqpacker_utfildistances");

		pageCoding_t pageCoding = createMetaFile(L"offsets", utfil.offsets);
		uint8_t offsetPages = pageCoding.pages;
		uint64_t useOffsetLongRange = pageCoding.useLongRange;

		pageCoding = createMetaFile(L"distances", utfil.distances);
		uint8_t distancePages = pageCoding.pages;
		uint64_t useDistanceLongRange = pageCoding.useLongRange;

		pageCoding = createMetaFile(L"seqlens", utfil.seqlens);
		uint8_t seqlenPages = pageCoding.pages;
		uint64_t useSeqlenLongRange = pageCoding.useLongRange;

		bool slimCase = (seqlenPages + offsetPages + distancePages == 0);
		if (!slimCase) {
			fputccLight(seqlenPages, utfil.main);
			fputccLight(offsetPages, utfil.main);
			fputccLight(distancePages, utfil.main);
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
			fputccLight(profile.seqlenMinLimit3, utfil.main);
		}
		else {
			printf("\n superslim used!");
		}
		fputccLight(packType, utfil.main);		
		syncMemSize(utfil.main);
		return utfil;
	}
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

seqPackBundle seq_pack_internal(memfile* m, packProfile profile, bool sep) {	
	separate_files = sep;
	initGlobalArrays();
	pack_internal(m, 1, profile);
	return pack_internal(m, 2, profile);
}

seqPackBundle seqPackSep(memfile* mem, packProfile profile) {
	return seq_pack_internal(mem, profile, true);
}

