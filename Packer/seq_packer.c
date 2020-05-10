#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "seq_packer.h"
#include <inttypes.h>
#include "seq_packer_commons.h"
#define VERBOSE false
#include "common_tools.h"

/* sequence packer */

//global variables used in compressor
static  FILE* infil, * utfil, * seq_lens_file, * offsets_file;
static unsigned char code;

static buffer_size = BLOCK_SIZE * 3;
static  unsigned char buffer[BLOCK_SIZE * 3];

size_t absolute_end, buffer_endpos;
static bool code_occurred = false;
static const char* base_dir = "c:/test/";
static bool separate_files = false;

static uint32_t nextChar[BLOCK_SIZE * 2];

static uint32_t lastChar[256];

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


void write_seqlen(uint64_t c) {
	fwrite(&c, 1, 1, (separate_files ? seq_lens_file : utfil));
}

void write_offset(uint64_t c) {
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
	uint64_t offset,
		winsize = (offset_pages + (uint64_t)1) * (uint64_t)256 + max_seqlen + max_seqlen,
		best_offset = 0, lowest_special_offset = (256 - offset_pages),
		min_seq_len = 3, offsets[1024] = { 0 }, seq_lens[258] = { 0 },
		longest_offset = lowest_special_offset + ((uint64_t)256 * offset_pages) - 1;
	long long  best_seqlen, seq_len;

	unsigned long char_freq[256] = { 0 };
	uint32_t buffer_pos = 0;
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
	long long size_org = get_file_size(infil);

	
	buffer_endpos = fread(&buffer, 1, buffer_size, infil);	

	assertEqual(buffer_endpos, size_org, "buffer_endpos == size_org in seqpacker");

	assert(buffer_size > size_org * 2, "buffer_size > size_org * 2  in seqpacker");

	absolute_end = buffer_endpos;

	uint64_t offset_max;

	while (buffer_pos < buffer_endpos) {

		best_seqlen = 0;
		unsigned char ch = buffer[buffer_pos];

		if (pass == 2) {
			if ((buffer_endpos - buffer_pos) >= min_seq_len) {
				if (buffer_pos + winsize > absolute_end - min_seq_len) {

					offset_max = (absolute_end - min_seq_len) - buffer_pos;
				}
				else {
					offset_max = winsize;
				}
			
				offset = 0;
				int32_t nextChar_pos = buffer_pos;
				int32_t nextCh;
				while (offset < offset_max) 
				{
					// find matching sequence		
					if ((nextCh = nextChar[nextChar_pos]) == 0) {
						break;
					}					
					if ((offset += nextCh) >= offset_max) {
						break;
					}
					nextChar_pos += nextCh;
					if (offset < 3) {
						continue;
					}
					
					//printf("\n File %s  Buffer pos %d", src, buffer_pos);
					/*
					if (ch != buffer[buffer_pos + offset]) {
						printf("ch = buffer[buffer[buffer_pos + offset]] in seqpacker\n");
					
						exit(0);
					}
					*/
					
					if (buffer[buffer_pos + 1] != buffer[buffer_pos + offset + 1]) {
						continue;
					}
					if (buffer[buffer_pos + 2] != buffer[buffer_pos + offset + 2]) {
						continue;
					}
					seq_len = 3;
				
					while (buffer[buffer_pos + seq_len] == buffer[buffer_pos + offset + seq_len] && seq_len < offset
						&& buffer_pos + offset + seq_len < absolute_end - 1 &&
						seq_len < max_seqlen)
					{
						seq_len++;
					}
					long diff = (buffer_pos + offset + seq_len) - absolute_end;
					if (diff > 0) {
						if (seq_len <= diff) {
							continue;
						}
						else {
							seq_len -= diff;  
							assert(seq_len >= 0, "seq_len >= 0");
						}
					}
					diff = (buffer_pos + seq_len) - buffer_endpos;
					if (diff > 0) {
						if (seq_len <= diff) {
							continue;
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
				if (char_freq[ch] < LONG_MAX) {
					char_freq[ch]++;
				}
				updateNextCharTable(ch, buffer_pos);
			}
			else {
				// occurence of code in original is handled by {code, 255} pair
				if (ch == code) {

					debug("Found code at buffer_startpos=%d", buffer_pos);

					write_seqlen(SEQ_LEN_FOR_CODE);
					WRITE(utfil, code);
				}
				else
				{
					
					WRITE(utfil, ch);
					assert(absolute_end < buffer_size, "absolute_end < buffer_size in seqpacker");
					updateNextCharTable(ch, absolute_end);
					buffer[absolute_end++] = ch; // write start to end to wrap-around find sequences					
				}
			}
			buffer_pos++; 
			display_progress(buffer_pos, pass);
		}
		else { // insert code triple instead of the matching sequence!

			assertSmallerOrEqual(buffer_pos + best_offset + best_seqlen + best_seqlen, absolute_end,
				"buffer_startpos + best_offset + best_seqlen*2 <= absolute_end in seq_packer.pack_internal");


			//debug("Found sequence seq_len=%d, offset=%d, at bufferstartpos=%d", best_seq_len, best_offset, buffer_startpos);

			if (pass == 1) {
				//offsets[best_seq_offset]++;// += (best_seq_len - 2);
				seq_lens[best_seqlen]++;
			}
			else if (pass == 2) {  // Write triplet!

				out_offset(best_offset, offset_pages);
				out_seqlen(best_seqlen, seqlen_pages);
				//if (best_seqlen > 230) {
				if (VERBOSE) {
					printf("\n%d, %d  buffer_startpos %d   buffer_endpos %d  seq \"", best_seqlen, best_offset, buffer_pos, buffer_endpos);
					for (int i = 0; i < best_seqlen; i++) {
						printf("%d ", buffer[buffer_pos + i]);
					}
					printf("\"");					
				}

				//}
				WRITE(utfil, code);  /* note file is read backwards during unpack! */
			}
			buffer_pos += best_seqlen;
			display_progress(buffer_pos, pass);
		}//end if
	}//end while

	if (VERBOSE && false) {
		printf("\nwrap around:\n");
		for (int i = buffer_endpos; i < absolute_end; i++) {
			printf("%c", buffer[i]);
		}
		printf("\n\n");
	}


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
	for (int i = 0; i < 256; i++) {
		lastChar[i] = INT32_MAX;
	}
	for (int i = 0; i < BLOCK_SIZE * 2; i++) {
		nextChar[i] = 0;
	}
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

