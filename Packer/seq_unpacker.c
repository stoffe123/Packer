#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "common_tools.h"
#include "packer_commons.h"
#include "seq_packer_commons.h"
#include "memfile.h"

#define VERBOSE false

// Unpacker stuff below !!

//Global vars used in unpacker
__declspec(thread) static size_t  seqlens_pos, offsets_pos, distances_pos, packed_file_end;

__declspec(thread) static uint8_t* buf;
__declspec(thread)                  static memfile* offsets, * seqlens, * distances;
__declspec(thread) static uint64_t buf_size = BLOCK_SIZE * 4, buf_pos, size_wraparound;

__declspec(thread) static  bool separate_files = true;


uint8_t read_byte_from_file() {
	packed_file_end--;
	return buf[packed_file_end];
}

void put_buf(uint8_t c) {
	buf[buf_pos--] = c;
	assert(buf_pos > packed_file_end, "buf_pos > packed_file_end in sequnpacker");
}

uint8_t readMeta(meta_kind_t kind) {
	if (separate_files) {
		switch (kind) {
		case SEQLEN: return getCCAtPos(seqlens, --seqlens_pos);
		case OFFSET: return getCCAtPos(offsets, --offsets_pos);
		case DISTANCE: return getCCAtPos(distances, --distances_pos);
		default: printf("\n wrong kind %d in seq_unpacker.readMeta", kind); exit(1);
		}
	}
	else {
		uint8_t c = read_byte_from_file();
		//debug("\n read_offset:%d", c);
		return c;
	}
}


uint64_t getMeta(pageCoding_t pageCoding, meta_kind_t kind) {

	uint64_t pages = pageCoding.pages;
	uint64_t useLongRange = pageCoding.useLongRange;

	uint64_t pageMax = calcPageMax(pageCoding);
	uint64_t lastByte = getLastByte(useLongRange);
	uint64_t lowestSpecial = getLowestSpecial(pageCoding);

	uint64_t offset = readMeta(kind);

	if (useLongRange && offset == 255) {

		offset = readMeta(kind) + pageMax + 1;
		if (useLongRange >= 2) {
			offset += readMeta(kind) * (uint64_t)256;
		}
		if (useLongRange >= 3) {
			offset += readMeta(kind) * (uint64_t)65536;
		}
	}
	else {
		if (offset >= lowestSpecial && offset <= lastByte) {
			uint64_t page = lastByte - offset;
			offset = lowestSpecial + (page * 256) + readMeta(kind);
		}
	}
	return offset;
}


uint64_t get_offset(pageCoding_t p) {	
	return getMeta(p, OFFSET);
}

uint64_t get_distance(pageCoding_t p) {
	return getMeta(p, DISTANCE);
}

uint64_t get_seqlen(pageCoding_t p) {
	return getMeta(p, SEQLEN);
}


uint64_t copyWrapAround(uint8_t seqlen_pages, uint8_t offset_pages, uint64_t useLongRange) {

	uint64_t lastByteOffset = getLastByte(useLongRange);
	uint64_t lowestSpecialOffset = getLastByte(offset_pages, useLongRange);

	uint8_t* temp_ar = malloc(packed_file_end);
	long long i = packed_file_end; //index to read from packed
	long long j = packed_file_end;  //index to write to temp array
	while (i > 0) {
		uint8_t ch = buf[--i];
		if (separate_files) {
			temp_ar[--j] = ch;
			continue;
		} 
		else {

			// ch was code
			uint64_t seqlen = buf[--i];

			//skip seqlen zero or one byte
			uint64_t last_byte = 255;
			uint64_t lowest_special = last_byte + 1 - seqlen_pages;
			if (seqlen_pages > 0 && seqlen >= lowest_special && seqlen <= last_byte) {
				i--;
			}

			//skip offset one, two or three bytes	
			uint64_t offset = buf[--i];
			if (useLongRange && offset == 255) {
				i -= 2;
			}
			else {
				if (offset >= lowestSpecialOffset && offset <= lastByteOffset) {
					i--;
				}
			}
		}
	}
	size_wraparound = packed_file_end - j;
	uint64_t k = buf_size - size_wraparound;
	for (uint64_t i = j; i < packed_file_end; i++) {
		buf[k++] = temp_ar[i];
	}
	assertEqual(k, buf_size, "k=buf_size in seq_unpacker");
	free(temp_ar);
	return buf_size - size_wraparound - 1;
}

uint64_t getRange(uint64_t packType, uint64_t startBit) {
	uint64_t range = isKthBitSet(packType, startBit + 1) ? 1 : 0;
	range += isKthBitSet(packType, startBit) ? 2 : 0;
	return range;
}

//------------------------------------------------------------------------------

memfile* seq_unpack_internal(memfile** mf_arr, bool sep)
{
	separate_files = sep;
	uint8_t offset_pages, seqlen_pages, distance_pages;	
	uint64_t useDistanceLongRange, useOffsetLongRange, useSeqlenLongRange;

	memfile* infil, * utfil;

	if (separate_files) {

		seqlens = mf_arr[1];
		rewindMem(seqlens);
		seqlens_pos = getSize(seqlens);

		offsets = mf_arr[2];
		rewindMem(offsets);
		offsets_pos = getSize(offsets);
		
		distances  = mf_arr[3];
		rewindMem(distances);
		distances_pos = getSize(distances);
	}

	debug("\n\n Seq unpack !!");
    uint8_t cc;
	
	infil = mf_arr[0];
	packed_file_end = getSize(infil);
	utfil = getMemfile(BLOCK_SIZE);
	reallocMem(infil, BLOCK_SIZE * 4);
	buf = infil->block;
	debug("\n packed_file_end %d", packed_file_end);	

	unsigned char packType = read_byte_from_file();
	bool superslim = isKthBitSet(packType, 7);
	uint64_t seqlenMinLimit3 = SUPERSLIM_SEQLEN_MIN_LIMIT3;
	uint64_t seqlenMinLimit4 = SEQLEN_MIN_LIMIT4;
	if (!superslim) {
		seqlenMinLimit3 = read_byte_from_file();
	}

	if (isKthBitSet(packType, 0)) {
		offset_pages = 0;
		seqlen_pages = 0;
		distance_pages = 0;
	}
	else {
		distance_pages = read_byte_from_file();
		offset_pages = read_byte_from_file();
		seqlen_pages = read_byte_from_file();		
	}
	pageCoding_t distancePageCoding;
	distancePageCoding.pages = distance_pages;
	distancePageCoding.useLongRange = getRange(packType, 3);

	pageCoding_t offsetPageCoding;
	offsetPageCoding.pages = offset_pages;
	offsetPageCoding.useLongRange = getRange(packType, 1);

	pageCoding_t seqlenPageCoding;
	seqlenPageCoding.pages = seqlen_pages;
	seqlenPageCoding.useLongRange = getRange(packType, 5);

	uint64_t lastDistance = get_distance(distancePageCoding);

	buf_pos = copyWrapAround(seqlen_pages, offset_pages, offsetPageCoding.useLongRange);
	
	debug(" \n pages=(%d, %d, %d)", offset_pages, seqlen_pages, distance_pages);
	
	debug("\n buf_pos after wraparound %d", buf_pos);

	debug("\n distance to first code: %d", lastDistance);

	debug("\n useOffsetLongrange: %d", useOffsetLongRange);

	debug("\n useDistanceLongrange: %d", useDistanceLongRange);

	debug("\n useSeqlenLongrange: %d", useSeqlenLongRange);

	if (VERBOSE) {
		printf("\nwrap around:\n");
		for (uint64_t i = packed_file_end; i < packed_file_end; i++) {
			printf("%c", buf[i]);
		}
		printf("\n\n");
	}
	packProfile profile = getPackProfile();
	profile.seqlenMinLimit3 = seqlenMinLimit3;


	uint64_t distance = 0;
	while (packed_file_end > 0 || distance == lastDistance) {
		if (distance < lastDistance) {
			cc = read_byte_from_file();
		}
		if (distance++ == lastDistance) {
			lastDistance = get_distance(distancePageCoding);
			distance = 0;
			uint64_t seqlen = get_seqlen(seqlenPageCoding);
			uint64_t offset = get_offset(offsetPageCoding);

			seqlen += getSeqlenMin(offset, profile);

			uint64_t match_index = buf_pos + offset + seqlen;
			debug("unp: (%d, %d, %d)  packed_file_end %d match_index:%d buf_pos:%d buf_size:%d '", seqlen, offset, lastDistance, packed_file_end, match_index, buf_pos, buf_size);
			assert(match_index < buf_size, "match_index < buf_size in seq_unpacker.unpack");
			//write the sequence at the right place!
			//if (seq_len > offset) {
			for (uint64_t i = 0; i < seqlen; i++) {
				put_buf(buf[match_index - i]);
				debug("%d ", buf[(match_index - seqlen) + (i + 1)]);
			}
			debug("'\n");
			/*	}

				else {  //offset < seq_len, repeating
					i = 0;
					long j = 0;
					while (i < seq_len) {
						put_buf(buf[match_index - j]);
						i++;
						j++;
						if (j == offset) {
							j = 0;
						}
					}
				}
				*/

		}
		else {
			put_buf(cc);
		}
	}
	uint64_t size_out = buf_size - ((uint64_t)buf_pos + 1) - size_wraparound;
	debug("Writing outfile from %d to %d", buf_pos + 1, buf_pos + 1 + size_out);
	memWrite(&buf[buf_pos + 1], size_out, utfil);
	return utfil;
}

memfile* seqUnpack(memfile* m) {
	return seq_unpack_internal(m, false);
}

void seq_unpack(const char* src, const char* dst) {
	printf("\n seq_unpack not supported");
	exit(1);
	/*
	wchar_t srcw[500], dstw[500];
	toUni(srcw, src);
	toUni(dstw, dst);
	seq_unpack_internal(srcw, dstw, L"", false);
	*/
}

void seq_unpack_separate(const char* src, const char* dst, const char* base_dir)
{
	const char mainFilename[100] = { 0 };
	concat(mainFilename, base_dir, "main");

	const char seqlensFilename[100] = { 0 };
	concat(seqlensFilename, base_dir, "seqlens");

	const char offsetsFilename[100] = { 0 };
	concat(offsetsFilename, base_dir, "offsets");

	const char distancesFilename[100] = { 0 };
	concat(distancesFilename, base_dir, "distances");

	memfile* mf_arr[4];
	mf_arr[0] = get_memfile_from_file(mainFilename);
	mf_arr[1] = get_memfile_from_file(seqlensFilename);
	mf_arr[2] = get_memfile_from_file(offsetsFilename);
	mf_arr[3] = get_memfile_from_file(distancesFilename);	
	memfile* unp = seq_unpack_internal(mf_arr, true);
	for (int i = 0; i < 4; i++) {
		fre(mf_arr[i]);
	}
	memfile_to_file(unp, dst);
	fre(unp);
}

void seqUnpackFiles(const wchar_t* srcw, const wchar_t* dstw) {

	printf("\n seqUnpackFiles not supported");
	exit(1);

	memfile* s = getMemfileFromFile(srcw);
	memfile* packed = seqUnpack(s);
	fre(s);
	memfileToFile(packed, dstw);
	fre(packed);
}

