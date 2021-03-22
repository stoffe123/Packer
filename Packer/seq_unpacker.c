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
__declspec(thread) static memfile* offsets, * seqlens, * distances;
__declspec(thread) static uint64_t buf_size = BLOCK_SIZE * 4, buf_pos, size_wraparound;

__declspec(thread) static  bool separate_files = true;


uint8_t read_byte_from_file() {	
	return buf[--packed_file_end];
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
	else if (useLongRange >= 3 && offset == 254) {
		offset = readMeta(kind) + pageMax + 1;		
		offset += readMeta(kind) * (uint64_t)256;		
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

memfile* seq_unpack_internal(seqPackBundle mf_arr, bool sep)
{
	wprintf(L"\n starting seq_unpack of %s", getMemName(mf_arr.main));
	separate_files = sep;
	uint8_t offset_pages, seqlen_pages, distance_pages;


	if (separate_files) {

		seqlens = mf_arr.seqlens;
		seqlens_pos = getMemSize(seqlens);

		offsets = mf_arr.offsets;
		offsets_pos = getMemSize(offsets);

		distances = mf_arr.distances;
		distances_pos = getMemSize(distances);
	}

	debug("\n\n ---------- Seq UNPACKER ----------------- ");
	uint8_t cc;
	uint64_t infil_orgsize = getMemSize(mf_arr.main);
	packed_file_end = infil_orgsize;
	assert(buf_size == BLOCK_SIZE * 4, "buf_size was not equal to BLOCK_SIZE*4 in seq_unpacker");
	reallocMem(mf_arr.main, buf_size);
	buf = mf_arr.main->block;
	debug("\n packed_file_end %llu", packed_file_end);

	unsigned char packType = read_byte_from_file();
	bool superslim = isKthBitSet(packType, 7);
	uint64_t seqlenMinLimit3 = SUPERSLIM_SEQLEN_MIN_LIMIT3;
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

	if (VERBOSE) {
		printf("\nwrap around:\n");
		for (uint64_t i = packed_file_end; i < packed_file_end; i++) {
			printf("%c", buf[i]);
		}
		printf("\n\n");
	}

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

			seqlen += getSeqlenMin(offset, seqlenMinLimit3);

			uint64_t match_index = buf_pos + offset + seqlen;
#if VERBOSE
			printf("unp: (%d, %d, %d)  packed_file_end %d match_index:%d buf_pos:%d buf_size:%d '", seqlen, offset, lastDistance, packed_file_end, match_index, buf_pos, buf_size);
			assert(match_index < buf_size, "match_index < buf_size in seq_unpacker.unpack");
#endif
			//write the sequence at the right place!
			//if (seq_len > offset) {
			for (uint64_t i = 0; i < seqlen; i++) {
				put_buf(buf[match_index - i]);
#if VERBOSE
				printf("%d ", buf[(match_index - seqlen) + (i + 1)]);
#endif
			}
#if VERBOSE
			printf("'\n");
#endif
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
	debug("Writing outfile from %llu to %llu", buf_pos + 1, buf_pos + 1 + size_out);
	memfile* utfil = getMemfile(size_out, L"sequnpacker_utfil");
	memWrite(&buf[buf_pos + 1], size_out, utfil);

	setSize(mf_arr.main, infil_orgsize);

	debug("\n seqlenMinLimit3=%llu", seqlenMinLimit3);
	debug("\n superslim=%d", superslim);

	return utfil;
}

memfile* seqUnpack(seqPackBundle m) {
	return seq_unpack_internal(m, true);
}



