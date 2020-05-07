#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_unpacker.h"
#include "common_tools.h"
#include "seq_packer_commons.h"

// Unpacker stuff below !!

//Global vars used in unpacker
static unsigned char code;
static FILE* infil, * utfil, * seqlens_file, * offsets_file;
static long long read_packedfile_pos, seqlens_pos, offsets_pos;
static  unsigned char buf[BLOCK_SIZE * 2];

static unsigned char offsets[BLOCK_SIZE];
static unsigned char seqlens[BLOCK_SIZE];
static unsigned long buf_pos = 0;
static  unsigned long long buf_size = BLOCK_SIZE*2;

static bool separate_files = false;


unsigned char read_byte_from_file() {	
	return buf[--read_packedfile_pos];
}

void put_buf(unsigned char c) {
	buf[buf_pos--] = c;
}

unsigned char read_seqlen() {
	if (separate_files) {		
		return seqlens[--seqlens_pos];
	}
	else {
		return read_byte_from_file();
	}
}

unsigned char read_offset() {
	if (separate_files) {
		return offsets[--offsets_pos];
	}
	else {
		return read_byte_from_file();
	}
}

unsigned long transform_seqlen(unsigned long seqlen, bool code_occurred, unsigned char pages) {

	unsigned long last_byte = (code_occurred ? 254 : 255);
	unsigned long lowest_special = last_byte + 1 - pages;

	if (pages > 0 && seqlen >= lowest_special && seqlen <= last_byte) {
		unsigned long long page = last_byte - seqlen;
		seqlen = lowest_special + (page * 256) + read_seqlen();
	}
	return seqlen + SEQ_LEN_MIN;
}

unsigned long get_offset(unsigned char pages) {

	unsigned long last_byte = 255;
	unsigned long lowest_special = last_byte + 1 - pages;

	unsigned long offset = read_offset();

	if (offset >= lowest_special && offset <= last_byte) {
		unsigned long long page = last_byte - offset;
		offset = lowest_special + (page * 256) + read_offset();
	}
	return offset;
}

//------------------------------------------------------------------------------

void seq_unpack_internal(const char* source_filename, const char* dest_filename, const char* base_dir)
{
	unsigned char offset_pages, seqlen_pages, 
		code_occurred = 1;

	unsigned long offsets_max, seqlens_max;
	if (separate_files) {
		seqlens_file = fopen(concat(base_dir, "seqlens"), "rb");
		seqlens_pos = fread(&seqlens, 1, BLOCK_SIZE, seqlens_file);
		fclose(seqlens_file);
		offsets_file = fopen(concat(base_dir, "offsets"), "rb");
		offsets_pos = fread(&offsets, 1, BLOCK_SIZE, offsets_file);
		fclose(offsets_file);
	}

	//printf("\n\n Unpacking %s", source_filename);
	unsigned long cc;

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
	read_packedfile_pos = fread(&buf, 1, BLOCK_SIZE * 2, infil);
	fclose(infil);
	buf_pos = buf_size - 1;
	code = read_byte_from_file();
	offset_pages = read_byte_from_file();
	seqlen_pages = read_byte_from_file();
	code_occurred = read_byte_from_file();


	while (read_packedfile_pos > 0) {
		cc = read_byte_from_file();
		if (cc == code) {
			unsigned long long offset,
				seqlen = read_seqlen();
			if (code_occurred && seqlen == SEQ_LEN_FOR_CODE) {
				//occurrence of code in original
				put_buf(code);
			}
			else {
				seqlen = transform_seqlen(seqlen, code_occurred, seqlen_pages);
				offset = get_offset(offset_pages);
				//printf("\nunp: seqlen %d  offst %d  read_packedfile_pos %d ", seqlen, offset, read_packedfile_pos);
				unsigned long long match_index = buf_pos + offset + seqlen;
				assert(match_index < buf_size, "match_index < buf_size in seq_unpacker.unpack");
				//write the sequence at the right place!
				//if (seq_len > offset) {
				for (unsigned long i = 0; i < seqlen; i++) {
					put_buf(buf[match_index - i]);
				}
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
	fwrite(&buf[buf_pos + 1], (buf_size - ((uint64_t)buf_pos + 1)), 1, utfil);
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