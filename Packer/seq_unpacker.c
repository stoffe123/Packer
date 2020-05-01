#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "seq_unpacker.h"
#include "common_tools.h"
#include "seq_packer_commons.h"

// Unpacker stuff below !!

//Global vars used in unpacker
static unsigned char code;
static FILE *infil, *utfil, *seq_lens_file, *offsets_file;
static long long read_packedfile_pos,
	seqlens_file_pos,
	offsets_file_pos;
static unsigned char* buf;
static unsigned long buf_pos = 0;
static unsigned buf_size = 40000000;
static const char* base_dir = "c:/test/";
static bool separate_files = false;


static unsigned char read_byte_from_file() {
	read_packedfile_pos++;
	fseek(infil, -read_packedfile_pos, SEEK_END);
	return fgetc(infil);
}

static void write_byte_to_file(unsigned char cc) {
	putc(cc, utfil);
}

static void put_buf(unsigned char c) {
	buf[buf_pos] = c;
	buf_pos--;
}


static unsigned char read_seqlen() {
	if (separate_files) {
		seqlens_file_pos++;
		fseek(seq_lens_file, -seqlens_file_pos, SEEK_END);
		return fgetc(seq_lens_file);
	}
	else {
		return read_byte_from_file();
	}
}

static unsigned char read_offset() {
	if (separate_files) {
		offsets_file_pos++;
		fseek(offsets_file, -offsets_file_pos, SEEK_END);
		return fgetc(offsets_file);
	}
	else {
		return read_byte_from_file();
	}
}

static unsigned long get_seqlen(bool code_occurred, unsigned char seqlen_pages) {
	unsigned long seqlen = read_seqlen();
	if (code_occurred && seqlen == SEQ_LEN_FOR_CODE) {
		return seqlen;
	}
	/**
	if (seqlen == (code_occurred ? 254 : 255)) {
		return (unsigned long)(code_occurred ? 254 : 255) + read_seqlen() + SEQ_LEN_MIN;
	}
	else {
		return seqlen + SEQ_LEN_MIN;
	}
	**/
	unsigned long long lowest_special = (unsigned long long)(code_occurred ? 255 : 256) - seqlen_pages;

	if (seqlen >= lowest_special && seqlen <= (code_occurred ? 254 : 255)) {
		unsigned long long page = (unsigned long long)(code_occurred ? 254 : 255) - seqlen;
		seqlen = (unsigned long long)lowest_special + (page * 256) + (unsigned long long)read_seqlen();
	}
	return seqlen + SEQ_LEN_MIN;
}

unsigned long get_offset(unsigned char window_pages) {
	unsigned long offset = read_offset();
	unsigned long long lowest_special = 256 - window_pages;

	if (offset >= lowest_special && offset <= 255) {
		unsigned long long page = 255 - offset;
		offset = (unsigned long long)lowest_special + (page * 256) + (unsigned long long)read_offset();
	}
	return offset;
}

//------------------------------------------------------------------------------

void seq_unpack_internal(const char* source_filename, const char* dest_filename)
{
	unsigned char offset_pages, seqlen_pages,
	              code_occurred = 1;
	if (separate_files) {
		seq_lens_file = fopen(concat(base_dir, "seqlens"), "rb");
		offsets_file = fopen(concat(base_dir, "offsets"), "rb");
	}

	//printf("\n\n Unpacking %s", source_filename);
	unsigned long i, cc;

	read_packedfile_pos = 0;
	seqlens_file_pos = 0;
	offsets_file_pos = 0;


	fopen_s(&infil, source_filename, "rb");
	if (!infil) {
		printf("\nHittade inte utfil: %s", source_filename);
		getchar();
		exit(1);
	}
	fopen_s(&utfil, dest_filename, "wb");
	if (!utfil) {
		puts("Hittade inte utfil!");
		getchar();
		exit(1);
	}
	long long total_size = get_file_size(infil);
	buf = (unsigned char*)malloc((buf_size + 1024) * sizeof(unsigned char));
	fseek(infil, 0, SEEK_END);

	buf_pos = buf_size - 1;
	code = read_byte_from_file();
	offset_pages = read_byte_from_file();
	seqlen_pages = read_byte_from_file();
	code_occurred = read_byte_from_file();


	while (total_size > read_packedfile_pos) {
		cc = read_byte_from_file();
		if (cc == code) {
			unsigned long long offset, 
				               seq_len = get_seqlen(code_occurred, seqlen_pages);

			if (cc == code && seq_len == 255 && code_occurred) {
				//occurrence of code in original
				put_buf(code);
			}
			else {
				
				offset = get_offset(offset_pages);		

				unsigned long match_index = buf_pos + offset + seq_len;
				assert(match_index < buf_size, "match_index < buf_size in seq_unpacker.unpack");
				//write the sequence at the right place!
				//if (seq_len > offset) {
					for (i = 0; i < seq_len; i++) {
						put_buf(buf[match_index - i]); ;
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
	fwrite(&buf[buf_pos + 1], (buf_size - (buf_pos + 1)) * sizeof(unsigned char), 1, utfil);

	fclose(infil);
	fclose(utfil);
	if (separate_files) {
		fclose(seq_lens_file);
		fclose(offsets_file);
	}
}

void seq_unpack(const char* source_filename, const char* dest_filename) {
	separate_files = false;
	seq_unpack_internal(source_filename, dest_filename);
}

void seq_unpack_separate(const char* source_filename, const char* dest_filename, const char* dir)
{
	base_dir = dir;
	source_filename = concat(base_dir, source_filename);
	separate_files = true;
	seq_unpack_internal(source_filename, dest_filename);
}