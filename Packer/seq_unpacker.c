#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_unpacker.h"
#include "common_tools.h"
#include "packer_commons.h"
#include "seq_packer_commons.h"

#define VERBOSE false

// Unpacker stuff below !!

//Global vars used in unpacker
static size_t  seqlens_pos, offsets_pos, packed_file_end;
static  uint8_t buf[BLOCK_SIZE * 4], offsets[BLOCK_SIZE], seqlens[BLOCK_SIZE];
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

uint64_t transform_seqlen(uint64_t seqlen, bool code_occurred, uint8_t pages) {

	uint64_t last_byte = (code_occurred ? 254 : 255);
	uint64_t lowest_special = last_byte + 1 - pages;

	if (pages > 0 && seqlen >= lowest_special && seqlen <= last_byte) {
		uint64_t page = last_byte - seqlen;
		seqlen = lowest_special + (page * 256) + read_seqlen();
	}
	return seqlen;
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

uint64_t copyWrapAround(bool code_occurred, uint8_t seqlen_pages, uint8_t offset_pages, uint8_t code, bool useLongRange, uint64_t lastByteOffset, uint64_t lowestSpecialOffset) {
	uint8_t* temp_ar = malloc(packed_file_end);
	long long i = packed_file_end; //index to read from packed
	long long j = packed_file_end;  //index to write to temp array
	while (i > 0) {
		uint8_t ch = buf[--i];
		if (separate_files || ch != code) {
			if (ch != code) {
				temp_ar[--j] = ch;
			}
			continue;
		}
		// ch was code
		uint64_t seqlen = buf[--i];
		if (code_occurred && seqlen == SEQ_LEN_FOR_CODE) {
			continue;
		}

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

void seq_unpack_internal(const char* source_filename, const char* dest_filename, const char* base_dir)
{
	uint8_t offset_pages, seqlen_pages;
	bool code_occurred, useLongRange;

	static FILE* infil, * utfil, * seqlens_file, * offsets_file;
	
	if (separate_files) {
		const char seqlens_name[100] = { 0 };
		const char offsets_name[100] = { 0 };
		concat(seqlens_name, base_dir, "seqlens");
		concat(offsets_name, base_dir, "offsets");
		seqlens_file = fopen(seqlens_name, "rb");
		seqlens_pos = fread(&seqlens, 1, BLOCK_SIZE, seqlens_file);
		fclose(seqlens_file);
		offsets_file = fopen(offsets_name, "rb");
		offsets_pos = fread(&offsets, 1, BLOCK_SIZE, offsets_file);
		fclose(offsets_file);
	}

	//printf("\n\n Unpacking %s", source_filename);
	uint8_t cc;

	fopen_s(&infil, source_filename, "rb");
	if (!infil) {
		printf("\nHittade inte utfil %s", source_filename);
		getchar();
		exit(1);
	}
	fopen_s(&utfil, dest_filename, "wb");
	if (!utfil) {
		printf("\nHittade inte utfil %s", dest_filename);
		getchar();
		exit(1);
	}
	packed_file_end = fread(&buf, 1, BLOCK_SIZE * 2, infil);
	debug("\n packed_file_end %d", packed_file_end);
	fclose(infil);

	uint8_t code = read_byte_from_file();
	offset_pages = read_byte_from_file();
	seqlen_pages = read_byte_from_file();
	unsigned char packType = read_byte_from_file();
	code_occurred = isKthBitSet(packType, 0);
	useLongRange = isKthBitSet(packType, 1);

	uint64_t lastByteOffset = (useLongRange ? 254 : 255);
	uint64_t lowestSpecialOffset = lastByteOffset + 1 - offset_pages;

	buf_pos = copyWrapAround(code_occurred, seqlen_pages, offset_pages, code, useLongRange, lastByteOffset, lowestSpecialOffset);
	debug("\n buf_pos after wraparound %d", buf_pos);
	
	uint64_t offsetPagesMax = offset_pages * (uint64_t)256 + (useLongRange ? 255 : 256);
	

	if (VERBOSE) {
		printf("\nwrap around:\n");
		for (uint64_t i = packed_file_end; i < packed_file_end; i++) {
			printf("%c", buf[i]);
		}
		printf("\n\n");
	}

	while (packed_file_end > 0) {
		cc = read_byte_from_file();
		if (cc == code) {
			uint64_t offset, seqlen = read_seqlen();
			if (code_occurred && seqlen == SEQ_LEN_FOR_CODE) {
				//occurrence of code in original
				put_buf(code);
			}
			else {
				seqlen = transform_seqlen(seqlen, code_occurred, seqlen_pages);
				offset = get_offset(offset_pages, useLongRange, offsetPagesMax, lastByteOffset, lowestSpecialOffset);

				unsigned char seqlen_min = getSeqlenMin(offset, lowestSpecialOffset, offsetPagesMax);
				
				seqlen += seqlen_min;

				uint64_t match_index = buf_pos + offset + seqlen;
				debug("unp: (%d, %d)  packed_file_end %d match_index:%d buf_pos:%d buf_size:%d '", seqlen, offset, packed_file_end, match_index, buf_pos, buf_size);
				assert(match_index < buf_size, "match_index < buf_size in seq_unpacker.unpack");
				//write the sequence at the right place!
				//if (seq_len > offset) {
				for (uint64_t i = 0; i < seqlen; i++) {
					put_buf(buf[match_index - i]);
					debug("%c", buf[(match_index - seqlen) + (i + 1)]);
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

void seq_unpack(const char* source_filename, const char* dest_filename) {
	separate_files = false;
	seq_unpack_internal(source_filename, dest_filename, "");
}

void seq_unpack_separate(const char* src, const char* dest_filename, const char* base_dir)
{
	separate_files = true;
	seq_unpack_internal(src, dest_filename, base_dir);
}