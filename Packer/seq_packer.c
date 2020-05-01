#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "seq_packer.h"
#include "seq_packer_commons.h"
#define VERBOSE false
#include "common_tools.h"

/* sequence packer */

//global variables used in compressor
static FILE* infil, * utfil, * seq_lens_file, * offsets_file;
static unsigned char code;
static unsigned long long buffer_endpos, buffer_startpos, buffer_min, buffer_size = 65536;
static unsigned char* buffer;
static bool code_occurred = false;
static const char* base_dir = "c:/test/";
static bool separate_files = false;

static void move_buffer(unsigned int steps) {
	buffer_startpos += steps;
	if (buffer_endpos == buffer_size) {
		unsigned long long buffer_left = buffer_size - buffer_startpos;
		if (buffer_left < buffer_min) {
			//load new buffer!!			
			debug("Load new buffer of: %d", buffer_size);

			unsigned char* new_buf = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
			for (unsigned long i = 0; i < (buffer_size - buffer_startpos); i++) {
				new_buf[i] = buffer[i + buffer_startpos];
			}
			unsigned long long res = fread(&new_buf[buffer_size - buffer_startpos], 1, buffer_startpos, infil);
			buffer_endpos = res + (buffer_size - buffer_startpos);
			free(buffer);
			buffer = new_buf;
			buffer_startpos = 0;
		}
	}
}

unsigned char find_best_code(long* char_freq) {
	unsigned char best;
	unsigned long value = ULONG_MAX;
	for (unsigned int i = 0; i < 256; i++) {
		if (char_freq[i] < value) {
			value = char_freq[i];
			best = i;
		}
	}//end for

	printf("\n Found code: %d that occured: %d times.", best, value);

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

void out_seqlen(unsigned long best_seq_len) {
	assert(best_seq_len >= SEQ_LEN_MIN, "best_seq_len >= SEQ_LEN_MIN in seq_packer.out_seqlen");
	unsigned long write_len = best_seq_len - SEQ_LEN_MIN;

	//len = 255 is used for code occurence ... len =254 for next block
	if (write_len <= (code_occurred ? 253 : 254)) {
		write_seqlen(write_len);  /* seqlen */
	}
	else {
		write_seqlen(write_len - ((unsigned long long)(code_occurred ? 254 : 255)));
		write_seqlen((code_occurred ? 254 : 255));
	}
}

void out_offset(unsigned long long best_offset, unsigned long long lowest_special, unsigned char offset_pages) {
	if (best_offset < lowest_special) {
		write_offset(best_offset);
	}
	else {
		int i = 0;
		for (; i < offset_pages; i++) {

			if (best_offset < (lowest_special + (256 * (i + 1)))) {
				write_offset(best_offset - (lowest_special + (256 * i)));
				write_offset(255 - i);
				break;
			}
		}
		if (i == offset_pages) {
			printf("\n\n >>>>>>>> ERROR no offset coding found for offset = %d", best_offset);
			exit(0);
		}
	}
}
//--------------------------------------------------------------------------------------------------------

void pack_internal(const char* src, const char* dest_filename, unsigned char pass, unsigned char offset_pages)
{

	unsigned long long offset, max_seq_len = (code_occurred ? 512 : 513), seq_len,
		winsize = (offset_pages + 1) * (unsigned long long)256 + max_seq_len * 2 + 25,
		best_offset = 0,
		min_seq_len = 3, offsets[1024] = { 0 }, seq_lens[258] = { 0 },
		lowest_special = 256 - offset_pages,
		longest_offset = lowest_special + ((unsigned long long)256 * offset_pages) - 1;


	unsigned long char_freq[256] = { 0 }, best_seq_len;
	buffer_startpos = 0;
	buffer_min = winsize + max_seq_len * 2 + 1024;

	debug("window_pages=%d", offset_pages);


	infil = fopen(src, "rb");
	if (!infil) {
		printf("\nSeq_packer: Hittade inte infil: %s", src);
		getchar();
		exit(1);
	}
	unsigned long total_size = get_file_size(infil);

	if (pass == 2) {
		printf("\n Winsize = %d", winsize);

		if (separate_files) {
			seq_lens_file = fopen(concat(base_dir, "seqlens"), "wb");
			offsets_file = fopen(concat(base_dir, "offsets"), "wb");
		}
		fopen_s(&utfil, dest_filename, "wb");
		if (!utfil) {
			printf("\nHittade inte utfil: %s", dest_filename); getchar(); exit(1);
		}

	}

	/* start compression */

	buffer_endpos = fread(buffer, 1, buffer_size, infil);

	unsigned long long offset_max;

	while (buffer_startpos < buffer_endpos) {

		best_seq_len = 2;

		if (pass == 2) {
			if ((buffer_endpos - buffer_startpos) >= min_seq_len * 2) {
				if (buffer_endpos - (buffer_startpos + winsize) >= min_seq_len * 2) {
					offset_max = buffer_endpos - buffer_startpos - min_seq_len * 2;
				}
				else {
					offset_max = winsize;
				}

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

					while (buffer[buffer_startpos + seq_len] == buffer[buffer_startpos + offset + seq_len] &&
						buffer_startpos + offset + seq_len < buffer_endpos &&
						seq_len < max_seq_len && seq_len < offset)
					{
						seq_len++;
					}
					//check if better than the best!

					if (seq_len > best_seq_len && offset - seq_len <= longest_offset) {
						best_seq_len = seq_len;
						best_offset = offset - seq_len;
						if (best_seq_len == max_seq_len) {
							break;
						}
					}
				}
			}
		}
		/* now we found the longest sequence in the window! */

		if (best_seq_len <= 2 || (best_offset >= lowest_special && best_seq_len <= 3))
		{       /* no sequence found, move window 1 byte forward and read one more byte */
			if (pass == 1) {
				if (char_freq[buffer[buffer_startpos]] < ULONG_MAX) {
					char_freq[buffer[buffer_startpos]]++;
				}
			}
			else {
				// occurence of code in original is handled by {code, 0} pair
				if (buffer[buffer_startpos] == code) {

					//debug("Found code at buffer_startpos=%d", buffer_startpos);

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

			//debug("Found sequence seq_len=%d, offset=%d, at bufferstartpos=%d", best_seq_len, best_offset, buffer_startpos);

			if (pass == 1) {
				//offsets[best_seq_offset]++;// += (best_seq_len - 2);
				seq_lens[best_seq_len]++;
			}
			if (pass == 2) {
				// write offset i.e. distance from end of match

				out_offset(best_offset, lowest_special, offset_pages);

				out_seqlen(best_seq_len);
				WRITE(utfil, code);  /* note file is read backwards during unpack! */
			}
			move_buffer(best_seq_len);
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



void seq_pack_internal(const char* source_filename, const char* dest_filename, unsigned char pages, bool sep) {
	buffer = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
	separate_files = sep;
	pack_internal(source_filename, dest_filename, 1, pages);
	pack_internal(source_filename, dest_filename, 2, pages);
}

void seq_pack(const char* source_filename, const char* dest_filename, unsigned char pages)
{
	seq_pack_internal(source_filename, dest_filename, pages, false);
}

void seq_pack_separate(const char* source_filename, unsigned char pages, const char* dir) {
	base_dir = dir;
	const char* dest_filename = concat(base_dir, "main");	
	seq_pack_internal(source_filename, dest_filename, pages, true);
}

