#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "seq_packer.h"
#include "seq_packer_commons.h"
#define VERBOSE false
#include "common_tools.h"

/* sequence packer */

//global variables used in compressor
static  FILE* infil, * utfil, * seq_lens_file, * offsets_file;
static unsigned char code;
static unsigned long long buffer_endpos = 0, buffer_startpos = 0,
                           buffer_size = BLOCK_SIZE * 2;

static  unsigned char buffer[BLOCK_SIZE * 2];
static bool code_occurred = false;
static const char* base_dir = "c:/test/";
static bool separate_files = false;

static void move_buffer(unsigned int steps) {
	buffer_startpos += steps;
	if (buffer_startpos % 16384 == 0) {
		printf("*");
	}
	assert(buffer_startpos <= buffer_size, "buffer_startpos <= buffer_size in seq_packer.move_buffer");	
}

static unsigned char find_best_code(long* char_freq) {
	unsigned char best;
	unsigned long value = LONG_MAX;
	for (unsigned int i = 0; i < 256; i++) {
		if (char_freq[i] < value) {
			value = char_freq[i];
			best = i;
		}
	}//end for

	printf("\n Found code %d with freq %d", best, value);

	char_freq[best] = ULONG_MAX; // mark it as used!
	code_occurred = (value > 0);
	return best;
}


void write_seqlen(unsigned long long c) {
	fwrite(&c, 1, 1, (separate_files ? seq_lens_file : utfil));
}

void write_offset(unsigned long long c) {
	fwrite(&c, 1, 1, (separate_files ? offsets_file : utfil));
}

void out_seqlen(unsigned long seqlen, unsigned char pages) {
	unsigned int last_byte = (code_occurred ? 254 : 255);
	unsigned int lowest_special = last_byte + 1 - pages;
	assert(seqlen >= SEQ_LEN_MIN, "best_seq_len >= SEQ_LEN_MIN in seq_packer.out_seqlen");
	seqlen -= SEQ_LEN_MIN;

	//len = 255 is used for code occurence ... len =254 for next block
	if (seqlen < lowest_special || pages == 0) {
		write_seqlen(seqlen);
		return;
	}
	else {
		unsigned long page = 0;
		for (; page < pages; page++) {
			if (seqlen < (lowest_special + (256 * (page + 1)))) {
				write_seqlen(seqlen - (lowest_special + (256 * page)));
				write_seqlen(last_byte - page);
				break;
			}
		}
		assert(page < pages, concat(
			">>>>>>>> ERROR in seq_packer.out_seqlen: no seqlen coding found for seqlen:",
			int_to_string(seqlen)));
	}
}

void out_offset(unsigned long offset, unsigned char pages) {
	unsigned int last_byte = 255;
	unsigned int lowest_special = last_byte + 1 - pages;
	if (offset < lowest_special) {
		write_offset(offset);
	}
	else {
		unsigned long page = 0;
		for (; page < pages; page++) {

			if (offset < (lowest_special + (256 * (page + 1)))) {
				write_offset(offset - (lowest_special + (256 * page)));
				write_offset(last_byte - page);
				break;
			}
		}
		assert(page < pages, concat(
			">>>>>>>> ERROR in seq_packer.out_offset: no offset coding found for offset:",
			int_to_string(offset)));
	}
}
//--------------------------------------------------------------------------------------------------------

void pack_internal(const char* src, const char* dest_filename, unsigned char pass,
	unsigned char offset_pages, unsigned char seqlen_pages)
{

	unsigned int max_seqlen = (code_occurred ? 257 : 258) - seqlen_pages + seqlen_pages * 256;
	unsigned long long offset,
		winsize = (offset_pages + (unsigned long long)1) * (unsigned long long)256 + max_seqlen + max_seqlen,
		best_offset = 0, lowest_special_offset = (256 - offset_pages),
		min_seq_len = 3, offsets[1024] = { 0 }, seq_lens[258] = { 0 },
		longest_offset = lowest_special_offset + ((unsigned long long)256 * offset_pages) - 1;
	long long  best_seqlen, seq_len;

	unsigned long char_freq[256] = { 0 };
	buffer_startpos = 0;
	debug("Seq_packer pass: %d", pass);

	infil = fopen(src, "rb");
	if (!infil) {
		printf("\nSeq_packer: Hittade inte infil: %s", src);
		getchar();
		exit(1);
	}

	if (pass == 2) {
		/*
		printf("\nWinsize: %d", winsize);
		printf("\noffset_pages: %d", offset_pages);
		printf("\nseqlen_pages: %d", seqlen_pages);
		printf("\nmax_seqlen: %d", max_seqlen);
		printf("\nbuffer_min: %d", buffer_min);
		printf("\ncode_occurred: %d", code_occurred);
	*/

		if (separate_files) {
			seq_lens_file = fopen(concat(base_dir, "seqlens"), "wb");
			offsets_file = fopen(concat(base_dir, "offsets"), "wb");
		}
		fopen_s(&utfil, dest_filename, "wb");
		if (!utfil) {
			printf("\nHittade inte utfil: %s", dest_filename); getchar(); exit(1);
		}

	}

	printf("\n");
	/* start compression */

	buffer_endpos = fread(&buffer, 1, buffer_size, infil);	

	unsigned long long offset_max;

	while (buffer_startpos < buffer_endpos) {

		best_seqlen = 0;

		if (pass == 2) {
			if ((buffer_endpos - buffer_startpos) >= min_seq_len * 2) {
				if (buffer_endpos - (buffer_startpos + winsize) < min_seq_len * 2) {
					offset_max = (buffer_endpos - buffer_startpos - min_seq_len * 2) + 1;
				}
				else {
				    offset_max = winsize;
				}
				//printf("\noffset max %d\n", offset_max);
				for (offset = 3; offset < offset_max; offset++)
				{
					// find matching sequence				
					//if (buffer_startpos + offset + min_seq_len * 2 > buffer_endpos) {
					//	break;
					//}

					if (buffer[buffer_startpos] != buffer[buffer_startpos + offset]) {
						continue;
					}
					if (buffer[buffer_startpos + 1] != buffer[buffer_startpos + offset + 1]) {
						continue;
					}
					if (buffer[buffer_startpos + 2] != buffer[buffer_startpos + offset + 2]) {
						continue;
					}
					seq_len = 3;

					while (buffer[buffer_startpos + seq_len] == buffer[buffer_startpos + offset + seq_len] && seq_len < offset
						&& buffer_startpos + offset + seq_len < buffer_endpos - 1 &&
						seq_len < max_seqlen)
					{
						seq_len++;
					}
					int diff = (buffer_startpos + offset + seq_len) - buffer_endpos;
					if (diff > 0) {
						if (seq_len <= diff) {
							seq_len = 0;
						}
						else {
							seq_len -= diff;  
							assert(seq_len >= 0, "seq_len >= 0");
						}
					}
					//check if better than the best!

					if (seq_len > best_seqlen && (offset - seq_len) <= longest_offset) {
						best_seqlen = seq_len;
						best_offset = offset - seq_len;
						assert(best_seqlen <= max_seqlen, "best_seq_len <= max_seqlen i seq_packer.pack_internal");
					
						if (best_seqlen == max_seqlen) {
							break;
						}
					}
				}
			}
		}
		/* now we found the longest sequence in the window! */

		if (best_seqlen <= 2 || (best_offset >= lowest_special_offset && best_seqlen <= 3))
		{       /* no sequence found, move window 1 byte forward and read one more byte */
			if (pass == 1) {
				if (char_freq[buffer[buffer_startpos]] < LONG_MAX) {
					char_freq[buffer[buffer_startpos]]++;
				}
			}
			else {
				// occurence of code in original is handled by {code, 255} pair
				if (buffer[buffer_startpos] == code) {

					debug("Found code at buffer_startpos=%d", buffer_startpos);

					write_seqlen(SEQ_LEN_FOR_CODE);
					WRITE(utfil, code);
				}
				else
				{
					WRITE(utfil, buffer[buffer_startpos]);
				}
			}
			move_buffer(1);
		}
		else { // insert code triple instead of the matching sequence!

			assertSmallerOrEqual(buffer_startpos + best_offset + best_seqlen + best_seqlen, buffer_endpos,
				"buffer_startpos + best_offset + best_seqlen*2 <= buffer_endpos in seq_packer.pack_internal");


			//debug("Found sequence seq_len=%d, offset=%d, at bufferstartpos=%d", best_seq_len, best_offset, buffer_startpos);

			if (pass == 1) {
				//offsets[best_seq_offset]++;// += (best_seq_len - 2);
				seq_lens[best_seqlen]++;
			}
			else if (pass == 2) {  // Write triplet!

				out_offset(best_offset, offset_pages);
				out_seqlen(best_seqlen, seqlen_pages);
				//if (best_seqlen > 230) {
					//printf("\nseqlen %d  offst %d  buffer_startpos %d   buffer_endpos %d", best_seqlen, best_offset, buffer_startpos, buffer_endpos);
				//}
				WRITE(utfil, code);  /* note file is read backwards during unpack! */
			}
			move_buffer(best_seqlen);
		}//end if
	}//end while


	if (pass == 1) {

		code = find_best_code(char_freq);
#if false
		printf("\n\n SEQ_LEN frequency:\n");
		for (int i = 0; i < 257; i++) {
			printf("%d,", seq_lens[i]);
		}

		printf("\n\n OFFSET :\n");
		for (int i = 0; i < 1024; i++) {
			printf("%d,", offsets[i]);
		}
#endif

	}
	else {
		WRITE(utfil, code_occurred);
		WRITE(utfil, seqlen_pages);
		WRITE(utfil, offset_pages);
		WRITE(utfil, code);
		fclose(utfil);
		if (separate_files) {
			fclose(seq_lens_file);
			fclose(offsets_file);
		}
	}
	fclose(infil);
}

unsigned char check_pages(pages, size) {
	unsigned char new_pages = 255;
	if ((uint64_t)256 + pages * (uint64_t)256 > size) {
		new_pages = size / 256 + 1;
		if (new_pages > 0) {
			new_pages--;
		}
	}
	if (new_pages < pages) {
		pages = new_pages;
	}
	return pages;
}

void seq_pack_internal(const char* source_filename, const char* dest_filename, unsigned char offset_pages, unsigned char seqlen_pages, bool sep) {
	long long source_size = get_file_size_from_name(source_filename);
	printf("\nPages (%d,%d)", offset_pages, seqlen_pages);
	offset_pages = check_pages(offset_pages, source_size);
	seqlen_pages = check_pages(seqlen_pages, source_size);
	printf("=> (%d,%d)", offset_pages, seqlen_pages);
	separate_files = sep;
	pack_internal(source_filename, dest_filename, 1, offset_pages, seqlen_pages);
	pack_internal(source_filename, dest_filename, 2, offset_pages, seqlen_pages);
}

void seq_pack(const char* source_filename, const char* dest_filename, unsigned char offset_pages, unsigned char seqlen_pages)
{
	seq_pack_internal(source_filename, dest_filename, offset_pages, seqlen_pages, false);
}

void seq_pack_separate(const char* source_filename, const char* dir, unsigned char offset_pages, unsigned char seqlen_pages) {
	base_dir = dir;
	const char* dest_filename = concat(base_dir, "main");
	seq_pack_internal(source_filename, dest_filename, offset_pages, seqlen_pages, true);
}

