#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "common_tools.h"
#include "packer_commons.h"
#include "seq_packer_commons.h"

#define VERBOSE false

// Unpacker stuff below !!

//Global vars used in unpacker
static size_t  seqlens_pos, offsets_pos, distances_pos, packed_file_end;
static uint8_t buf[BLOCK_SIZE * 4], offsets[BLOCK_SIZE], seqlens[BLOCK_SIZE], distances[BLOCK_SIZE];
static uint64_t buf_size = BLOCK_SIZE * 4, buf_pos, size_wraparound;

static bool separate_files = true;


uint8_t read_byte_from_file() {
	packed_file_end--;
	return buf[packed_file_end];
}

void put_buf(uint8_t c) {
	buf[buf_pos--] = c;
	assert(buf_pos > packed_file_end, "buf_pos > packed_file_end in sequnpacker");
}

uint8_t read_seqlen() {
	if (separate_files) {
		return seqlens[--seqlens_pos];
	}
	else {
		uint8_t c = read_byte_from_file();
		//debug("\nread seqlen:%d", c);
		return c;
	}
}

uint8_t read_offset() {
	if (separate_files) {
		return offsets[--offsets_pos];
	}
	else {
		uint8_t c = read_byte_from_file();
		//debug("\n read_offset:%d", c);
		return c;
	}
}

uint8_t read_distance() {
	if (separate_files) {
		return distances[--distances_pos];
	}
	else {
		uint8_t c = read_byte_from_file();
		//debug("\n distance:%d", c);
		return c;
	}
}


uint64_t transform_seqlen(uint64_t seqlen, bool code_occurred, uint8_t pages) {

	uint64_t last_byte = (code_occurred ? 254 : 255);
	uint64_t lowest_special = last_byte + 1 - pages;

	if (pages > 0 && seqlen >= lowest_special && seqlen <= last_byte) {
		uint64_t page = last_byte - seqlen;
		seqlen = lowest_special + (page * 256) + read_seqlen();
	}
	return seqlen;
}

uint64_t get_distance(uint8_t pages, bool useLongRange, uint64_t distancePagesMax, uint64_t lastByteDistance,
	uint64_t lowestSpecialDistance) {

	uint64_t distance = read_distance();

	if (useLongRange && distance == 255) {
		distance = read_distance() + read_distance() * (uint64_t)256 + read_distance() * (uint64_t)65536 + distancePagesMax;
	}
	else {
		if (distance >= lowestSpecialDistance && distance <= lastByteDistance) {
			uint64_t page = lastByteDistance - distance;
			distance = lowestSpecialDistance + (page * 256) + read_distance();
		}
	}
	return distance;
}

uint64_t get_offset(uint8_t pages, bool useLongRange, uint64_t offsetPagesMax, uint64_t lastByteOffset,
	uint64_t lowestSpecialOffset) {

	uint64_t offset = read_offset();

	if (useLongRange && offset == 255) {
		offset = read_offset() + read_offset() * (uint64_t)256 + offsetPagesMax;
	}
	else {
		if (offset >= lowestSpecialOffset && offset <= lastByteOffset) {
			uint64_t page = lastByteOffset - offset;
			offset = lowestSpecialOffset + (page * 256) + read_offset();
		}
	}
	return offset;
}

uint64_t copyWrapAround(bool code_occurred, uint8_t seqlen_pages, uint8_t offset_pages, bool useLongRange, uint64_t lastByteOffset, uint64_t lowestSpecialOffset) {
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
			uint64_t last_byte = (code_occurred ? 254 : 255);
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

//------------------------------------------------------------------------------

void seq_unpack_internal(const wchar_t* source_filename, const wchar_t* dest_filename,
	const wchar_t* base_dir, bool sep)
{
	separate_files = sep;
	uint8_t offset_pages, seqlen_pages, distance_pages;
	bool useLongRange, useDistanceLongRange;

	static FILE* infil, * utfil, * seqlens_file, * offsets_file, * distances_file;

	if (separate_files) {
		const wchar_t seqlens_name[100] = { 0 };
		const wchar_t offsets_name[100] = { 0 };
		const wchar_t distances_name[100] = { 0 };
		concatw(seqlens_name, base_dir, L"seqlens");
		concatw(offsets_name, base_dir, L"offsets");
		concatw(distances_name, base_dir, L"distances");
		seqlens_file = openRead(seqlens_name);
		seqlens_pos = fread(&seqlens, 1, BLOCK_SIZE, seqlens_file);
		fclose(seqlens_file);
		offsets_file = openRead(offsets_name);
		offsets_pos = fread(&offsets, 1, BLOCK_SIZE, offsets_file);
		fclose(offsets_file);
		distances_file = openRead(distances_name);
		distances_pos = fread(&distances, 1, BLOCK_SIZE, distances_file);
		fclose(distances_file);
	}

	debug("\n\n Seq unpack !!");
    uint8_t cc;

	infil = openRead(source_filename);
	utfil = openWrite(dest_filename);
	packed_file_end = fread(&buf, 1, BLOCK_SIZE * 2, infil);
	debug("\n packed_file_end %d", packed_file_end);
	fclose(infil);

	unsigned char packType = read_byte_from_file();
	useLongRange = isKthBitSet(packType, 0);
	useDistanceLongRange = isKthBitSet(packType, 1);
	if (isKthBitSet(packType, 2)) {
		offset_pages = 0;
		seqlen_pages = 0;
		distance_pages = 0;
	}
	else {
		distance_pages = read_byte_from_file();
		offset_pages = read_byte_from_file();
		seqlen_pages = read_byte_from_file();
		
	}

	uint64_t lastByteOffset = (useLongRange ? 254 : 255);
	uint64_t lastByteDistance = (useDistanceLongRange ? 254 : 255);
	uint64_t lowestSpecialOffset = lastByteOffset + 1 - offset_pages;
	uint64_t lowestSpecialDistance = lastByteDistance + 1 - distance_pages;

	uint64_t offsetPagesMax = offset_pages * (uint64_t)256 + (useLongRange ? 255 : 256);
	uint64_t distancePagesMax = distance_pages * (uint64_t)256 + ((useDistanceLongRange ? (uint64_t)254 : (uint64_t)255)
		                                                            - distance_pages);
	

	uint64_t lastDistance = get_distance(distance_pages, useDistanceLongRange, distancePagesMax, lastByteDistance, lowestSpecialDistance);

	bool code_occurred = false;
	buf_pos = copyWrapAround(code_occurred, seqlen_pages, offset_pages, useLongRange, lastByteOffset, lowestSpecialOffset);
	
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
			lastDistance = get_distance(distance_pages, useDistanceLongRange, distancePagesMax, lastByteDistance, lowestSpecialDistance);
			distance = 0;
			uint64_t offset, seqlen = read_seqlen();
			
				seqlen = transform_seqlen(seqlen, code_occurred, seqlen_pages);
				offset = get_offset(offset_pages, useLongRange, offsetPagesMax, lastByteOffset, lowestSpecialOffset);

				unsigned char seqlen_min = getSeqlenMin(offset, lowestSpecialOffset, offsetPagesMax);

				seqlen += seqlen_min;

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
	fwrite(&buf[buf_pos + 1], size_out, 1, utfil);
	fclose(utfil);
}

void seq_unpack(const char* src, const char* dst) {
	wchar_t srcw[500], dstw[500];
	toUni(srcw, src);
	toUni(dstw, dst);
	seq_unpack_internal(srcw, dstw, L"", false);
}

void seqUnpackSeparate(const wchar_t* src, const wchar_t* dst, const wchar_t* base_dir) {
	seq_unpack_internal(src, dst, base_dir, true);
}

void seq_unpack_separate(const char* src, const char* dst, const char* base_dir)
{
	wchar_t srcw[500], dstw[500], basew[500];
	toUni(srcw, src);
	toUni(dstw, dst);
	toUni(basew, base_dir);
	seq_unpack_internal(srcw, dstw, basew, true);
}

void seqUnpack(const wchar_t* src, const wchar_t* dst) {
	seq_unpack_internal(src, dst, L"", false);
}