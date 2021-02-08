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

#define LIMIT_24_BIT 16777216

/* Sequence packer */

/* Global variables used in compressor */

__declspec(thread) static const wchar_t* src_name;

__declspec(thread) static bool separate_files = false;

__declspec(thread) static uint32_t *nextChar,
lastChar[LIMIT_24_BIT],
*distances,
distanceFreq[BLOCK_SIZE],
distancesPos,

*offsets,
offsetFreq[BLOCK_SIZE],
offsetsPos,

*seqlens,
seqlenFreq[BLOCK_SIZE],
seqlensPos;


uint32_t calc24Bit(uint8_t ch, uint8_t ch1, uint8_t ch2) {
	return ch + 256 * ch1 + 65536 * ch2;
}


void updateNextCharTable(uint8_t ch1, uint8_t ch2, uint8_t ch3, uint32_t pos) {
	uint32_t ch = calc24Bit(ch1, ch2, ch3);
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



void writeMeta(memfile* file, uint64_t c) {
	if (c > 255) {
		printf("\n write_distance called with value > 255 was %llu", c);
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

void convertMeta(memfile* file, uint64_t distance, pageCoding_t pageCoding) {

	uint8_t pages = pageCoding.pages;
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
			printf("\n Seqpacker: long range was needed but not set!! distance %llu pages %d pagemax %llu when packing %ls",
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
		//printf("\n i=%d", i);
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
		printf("\n   comparing %ls meta file sizes ... %u %llu when seqpacking file %ls", metaname, bestSize, filesize, src_name);
		exit(1);
	}
	//wprintf(L"\n %s pages %d useLongrange %d", metaname, bestPageCoding.pages, bestPageCoding.useLongRange);

	return bestPageCoding;
}

uint64_t storeLongRange(uint64_t packType, uint64_t longRange, uint64_t startBit) {

	if (longRange / 2 == 1) {
		packType = setKthBit(packType, startBit);
	}
	if (longRange % 2 == 1) {
		packType = setKthBit(packType, startBit + 1);
	}
	return packType;
}

//--------------------------------------------------------------------------------------------------------

seqPackBundle pack_internal(memfile* infil, uint8_t pass, packProfile profile)
{
	rewindMem(infil);

	unsigned int max_seqlen = 65791;
	bool superslim = false;
	uint64_t size_org = getMemSize(infil);
	assert(size_org > 0, "size_org negative in seqpacker");
	assert(size_org <= BLOCK_SIZE, "size_org larger than BLOCK_SIZE in seqpacker");
	reallocMem(infil, size_org * 3);
	int64_t seqlenMinLimit3 = profile.seqlenMinLimit3;
	if (seqlenMinLimit3 > 255) {
		printf("\n WARNING!!!  seqlenMinLimit3 was over 255. Doing % 256 on it!");
		seqlenMinLimit3 %= 256;
	}
	if (size_org < profile.sizeMaxForSuperslim) {
		superslim = true;
		seqlenMinLimit3 = SUPERSLIM_SEQLEN_MIN_LIMIT3;
	}
	debug("\n seqlenMinLimit3=%llu", seqlenMinLimit3);
	debug("\n superslim=%d", superslim);

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

		debug("\nWinsize: %llu", winsize);
		debug("\nmax_seqlen: %d", max_seqlen);

		utfil.main = getMemfile(size_org * 3 + 1000, L"seqpacker_utfilmain");

	}	
	/* start compression */

	uint64_t buffer_endpos = size_org;

	uint64_t absolute_end = buffer_endpos;
	uint64_t real_absolute_end = buffer_endpos - 2;

	uint64_t offset_max;
	uint8_t* buffer = infil->block;

	uint8_t lastChMinus2 = buffer[buffer_endpos - 2],
	        lastChMinus1 = buffer[buffer_endpos - 1];

	while (buffer_pos < buffer_endpos) {

		best_seqlen = 0;
		
	    uint8_t ch = buffer[buffer_pos];
		uint8_t ch1 = buffer[1 + buffer_pos];
		uint8_t ch2 = buffer[2 + buffer_pos];
		if (pass == 2) {
			if ((buffer_endpos - buffer_pos) >= min_seq_len && (buffer_pos - real_absolute_end > 5)) {
							
			//	if (buffer_pos + winsize > real_absolute_end - min_seq_len) {

					offset_max = (real_absolute_end - min_seq_len) - buffer_pos;
				//}
				//else {
			//		offset_max = winsize;
			//	}

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
					if (offset < 3) {
						continue;
					}
					seq_len = 3;

					while (buffer[buffer_pos + seq_len] == buffer[buffer_pos + offset + seq_len] &&
						seq_len < offset &&
						buffer_pos + offset + seq_len < real_absolute_end - 1)
					{
						seq_len++;
					}
					diff = (buffer_pos + offset + seq_len) - real_absolute_end;
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
		uint8_t seqlen_min = getSeqlenMin(best_offset, seqlenMinLimit3);

		if (best_seqlen < seqlen_min)
		{       /* no sequence found, move window 1 byte forward and read one more byte */
			if (pass == 1) {

				if (buffer_pos <= buffer_endpos - 3) {
					updateNextCharTable(ch, ch1, ch2, buffer_pos);
				}
			}
			else {
				fputccLight(ch, utfil.main);
				//assert(absolute_end < BLOCK_SIZE, "absolute_end < BLOCK_SIZE in seqpacker");

				updateNextCharTable(lastChMinus2, lastChMinus1, ch, real_absolute_end++);
				//printf("\n Writing to end: %lu", rss);
				//printf("\n  %llu, %llu", absolute_end, real_absolute_end);
				lastChMinus2 = lastChMinus1;
				lastChMinus1 = ch;

				buffer[absolute_end++] = ch; // write start to end to wrap-around find sequences					
				distance++;
			}
			buffer_pos++;
		}
		else { // insert code triple instead of the matching sequence!
			//debug("\n  seqlenmin:%d  offset:%d", seqlen_min, best_offset);
			assertSmallerOrEqual(buffer_pos + best_offset + best_seqlen + best_seqlen, real_absolute_end,
				"buffer_startpos + best_offset + best_seqlen*2 <= absolute_end in seq_packer.pack_internal");


			//debug("Found sequence seq_len=%d, offset=%d, at bufferstartpos=%d", best_seq_len, best_offset, buffer_startpos);

			if (pass == 2) {  // Write triplet!

				out_offset(best_offset);

				out_seqlen(best_seqlen - seqlen_min);
#if VERBOSE
				printf("\n(%d, %d, %d)  buffer_startpos %d   buffer_endpos %d  seq \"", best_seqlen, best_offset, distance, buffer_pos, buffer_endpos);
				for (int i = 0; i < best_seqlen; i++) {
					printf("%x ", buffer[buffer_pos + i]);
				}
				printf("\"");
#endif


				/* note file is read backwards during unpack! */
				out_distance(distance);
				distance = 0;
			}
			buffer_pos += best_seqlen;
		}//end if
	}//end while

#if VERBOSE
	printf("\nwraparound:\n");
	for (int i = buffer_endpos; i < real_absolute_end; i++) {
		printf("%x ", buffer[i]);
	}
	printf("\n\n");
#endif
	setSize(infil, size_org);
	if (pass == 2) {

		out_distance(distance);
		debug("\n distance to first code: %llu", distance);

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
		uint8_t packType = 0;
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
			fputccLight(seqlenMinLimit3, utfil.main);
		}
		else {
			printf("\n superslim used!");
		}
		fputccLight(packType, utfil.main);
		syncMemSize(utfil.main);
		debug("utfil.main size=%d", getMemSize(utfil.main));
		return utfil;
	}
}



void initGlobalArrays(uint64_t size) {

	size += 10;
	
	nextChar = calloc(size * 2, sizeof(uint32_t));
	distances = calloc(size, sizeof(uint32_t));
	offsets = calloc(size, sizeof(uint32_t));
	seqlens =  calloc(size, sizeof(uint32_t));
	
	for (int i = 0; i < LIMIT_24_BIT; i++) {
		lastChar[i] = INT32_MAX;
	}		
	for (int i = 0; i < BLOCK_SIZE; i++) {
		seqlenFreq[i] = 0;
		distanceFreq[i] = 0;
		offsetFreq[i] = 0;
	}
	distancesPos = 0;
	offsetsPos = 0;
	seqlensPos = 0;	
}


seqPackBundle seq_pack_internal(memfile* memToPack, packProfile profile, bool sep) {
	separate_files = sep;
	initGlobalArrays(getMemSize(memToPack));
	pack_internal(memToPack, 1, profile);

	seqPackBundle packedBundle = pack_internal(memToPack, 2, profile);

	if (DOUBLE_CHECK_PACK) {
		wchar_t* name = getMemName(memToPack);
		printf("\n Double checking the seqpack of: %ls", name);
		memfile* unpackedMem = seqUnpack(packedBundle);
		bool eq = memsEqual(unpackedMem, memToPack);
		if (!eq) {
			printf("\n\n\n !!!! FAILED to seq pack: %ls", name);
			const wchar_t filename[300] = { 0 };
			if (name[1] != ':') {
				concatw(filename, L"c:/test/temp_files/", name);
			}
			else {
				wcscpy(filename, name);
			}
			memfileToFile(memToPack, filename);
			memfileToFile(unpackedMem, L"c:/test/temp_files/unpackedmem");
			printf("\n\n Profile used:");
			printf("\n ------------------------");
			printProfile(&profile);
			exit(1);
		}
		freeMem(unpackedMem);

		free(nextChar);
		free(distances);

		free(offsets);

		free(seqlens);
	}
	return packedBundle;
}

seqPackBundle seqPackSep(memfile* mem, packProfile profile) {
	return seq_pack_internal(mem, profile, true);
}

