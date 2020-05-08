#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_unpacker.h"
#include "common_tools.h"
#include "seq_packer_commons.h"

#define VERBOSE false

// Unpacker stuff below !!

//Global vars used in unpacker
static size_t  seqlens_pos, offsets_pos, packed_file_end;
static  uint8_t buf[BLOCK_SIZE * 3], offsets[BLOCK_SIZE], seqlens[BLOCK_SIZE];
static uint64_t buf_size = BLOCK_SIZE * 3, buf_pos, read_packedfile_pos, size_wraparound;

static bool separate_files = false;


uint8_t read_byte_from_file() {	
	read_packedfile_pos--;
	return buf[read_packedfile_pos];
}

void put_buf(uint8_t c) {
	buf[buf_pos--] = c;
}

uint8_t read_seqlen() {
	if (separate_files) {		
		return seqlens[--seqlens_pos];
	}
	else {
		return read_byte_from_file();
	}
}

uint8_t read_offset() {
	if (separate_files) {
		return offsets[--offsets_pos];
	}
	else {
		return read_byte_from_file();
	}
}

uint64_t transform_seqlen(uint64_t seqlen, bool code_occurred, uint8_t pages) {

	uint64_t last_byte = (code_occurred ? 254 : 255);
	uint64_t lowest_special = last_byte + 1 - pages;

	if (pages > 0 && seqlen >= lowest_special && seqlen <= last_byte) {
		uint64_t page = last_byte - seqlen;
		seqlen = lowest_special + (page * 256) + read_seqlen();
	}
	return seqlen + SEQ_LEN_MIN;
}

uint64_t get_offset(uint8_t pages) {

	uint64_t last_byte = 255;
	uint64_t lowest_special = last_byte + 1 - pages;

	uint64_t offset = read_offset();

	if (offset >= lowest_special && offset <= last_byte) {
		uint64_t page = last_byte - offset;
		offset = lowest_special + (page * 256) + read_offset();
	}
	return offset;
}

size_t copyWrapAround(bool code_occurred, uint8_t seqlen_pages, uint8_t offset_pages, uint8_t code) {
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
		// ch == code
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

		//skip offset one or two bytes
		last_byte = 255;
		lowest_special = last_byte + 1 - offset_pages;
		uint64_t offset = buf[--i];
		if (offset >= lowest_special && offset <= last_byte) {
			i--;
		}
	}
	uint64_t k = packed_file_end;
	size_wraparound = packed_file_end - j;
	for (uint64_t i = j; i < packed_file_end; i++) {
		buf[k++] = temp_ar[i];
	}
	return k;
}

//------------------------------------------------------------------------------

void seq_unpack_internal(const char* source_filename, const char* dest_filename, const char* base_dir)
{
	uint8_t offset_pages, seqlen_pages, 
		code_occurred = 1;

	static FILE* infil, * utfil, * seqlens_file, * offsets_file;

	if (separate_files) {
		seqlens_file = fopen(concat(base_dir, "seqlens"), "rb");
		seqlens_pos = fread(&seqlens, 1, BLOCK_SIZE, seqlens_file);
		fclose(seqlens_file);
		offsets_file = fopen(concat(base_dir, "offsets"), "rb");
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
	fclose(infil);
	
	uint8_t code = buf[--packed_file_end];
	offset_pages = buf[--packed_file_end];
	seqlen_pages = buf[--packed_file_end];
	code_occurred = buf[--packed_file_end];

	read_packedfile_pos = copyWrapAround(code_occurred, seqlen_pages, offset_pages, code);
	if (VERBOSE) {
		printf("\nwrap around:\n");
		for (uint64_t i = packed_file_end; i < read_packedfile_pos; i++) {
			printf("%c", buf[i]);
		}
		printf("\n\n");
	}

	buf_pos = buf_size - 1;

	while (read_packedfile_pos > 0) {
		cc = read_byte_from_file();
		if (cc == code) {
			uint64_t offset, seqlen = read_seqlen();			
			if (code_occurred && seqlen == SEQ_LEN_FOR_CODE) {
				//occurrence of code in original
				put_buf(code);
			}
			else {
				seqlen = transform_seqlen(seqlen, code_occurred, seqlen_pages);
				offset = get_offset(offset_pages);

				uint64_t match_index = buf_pos + offset + seqlen;
				printf("\nunp: %d, %d  read_packedfile_pos %d match_index %d '", seqlen, offset, read_packedfile_pos, match_index);
				assert(match_index < buf_size, "match_index < buf_size in seq_unpacker.unpack");
				//write the sequence at the right place!
				//if (seq_len > offset) {
				for (uint64_t i = 0; i < seqlen; i++) {
					put_buf(buf[match_index - i]);
					printf("%d ", buf[(match_index - seqlen) + (i + 1)]);
				}
				printf("'\n");
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
	printf("Writing outfile from %d to %d", buf_pos + 1, buf_pos + 1 + size_out);
	fwrite(&buf[buf_pos + 1], size_out, 1, utfil);
	fclose(utfil);
}

void seq_unpack(const char* source_filename, const char* dest_filename) {
	separate_files = false;
	seq_unpack_internal(source_filename, dest_filename, "");
}

void seq_unpack_separate(const char* source_filename, const char* dest_filename, const char* base_dir)
{
	source_filename = concat(base_dir, source_filename);
	separate_files = true;
	seq_unpack_internal(source_filename, dest_filename, base_dir);
}