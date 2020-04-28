#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "seq_packer.h"
#include "common_tools.h"
#define TRUE 1
#define FALSE 0

#define math_max(x,y) ((x) >= (y)) ? (x) : (y)
#define math_min(x,y) ((x) <= (y)) ? (x) : (y)

/* sequence packer */

FILE* infil, * utfil;

unsigned char code;

const bool verbose = false;

void WRITE(unsigned long long c)
{
	fwrite(&c, 1, 1, utfil);
}


unsigned long long READ(unsigned char* cptr)
{
	return (fread(cptr, 1, 1, infil));
}

//global variables used in compressor
unsigned long long buffer_endpos, buffer_startpos, buffer_min, buffer_size = 65536;
unsigned char* buffer;
unsigned char seqlen_add = 2;

void move_buffer(unsigned int steps) {
	buffer_startpos += steps;
	if (buffer_endpos == buffer_size) {
		unsigned long long buffer_left = buffer_size - buffer_startpos;
		if (buffer_left < buffer_min) {
			//load new buffer!!
			if (verbose) {
				printf("\nLoad new buffer of: %d", buffer_size);
			}

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
	//if (verbose) {
		printf("\n Found code: %d that occured: %d times.", best, value);
	//}
	char_freq[best] = ULONG_MAX; // mark it as used!
	if (value == 0) {
		seqlen_add = 3;
	}
	return best;	
}

FILE* seq_lens_file;
FILE* offsets_file;

void write_seqlen(unsigned long long c) {
	fwrite(&c, 1, 1, seq_lens_file);
}

void write_offset(unsigned long long c) {
	fwrite(&c, 1, 1, offsets_file);
}

//--------------------------------------------------------------------------------------------------------

void pack_internal(const char* src, const char* dest_filename, unsigned char pass, unsigned char window_pages)
{
	seq_lens_file = fopen("c:/test/seqlens", "wb");
	offsets_file = fopen("c:/test/offsets", "wb");
	unsigned long long offset, max_seq_len = 510 + seqlen_add, seq_len,
		winsize = (window_pages + 1) * (unsigned long long)256 + max_seq_len * 2 + 25,
		best_offset = 0,
		min_seq_len = 3, offsets[1024] = { 0 }, seq_lens[258] = { 0 },
		lowest_special = 256 - window_pages,
		longest_offset = lowest_special + ((unsigned long long)256 * window_pages) - 1;


	unsigned long char_freq[256] = { 0 }, best_seq_len;
	buffer_startpos = 0;
	buffer_min = winsize + max_seq_len * 2 + 1024;

	//printf("\nwinsize=%d", winsize);
	//printf("\nwindow_pages=%d", window_pages);

	infil = fopen(src, "rb");
	if (!infil) {
		printf("Hittade inte infil: %s", infil);
		getchar();
		exit(1);
	}
	unsigned long total_size = get_file_size(infil);

	if (pass == 2) {
		fopen_s(&utfil, dest_filename, "wb");
		if (!utfil) {
			puts("Hittade inte utfil!"); getchar(); exit(1);
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

					while (buffer[buffer_startpos + seq_len] == buffer[buffer_startpos + offset + seq_len] && buffer_startpos + offset + seq_len < buffer_endpos &&
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
					if (verbose) {
						printf("\nFound code at buffer_startpos=%d", buffer_startpos);
					}
					write_seqlen(0);
					WRITE(code);
				}
				else
				{
					WRITE(buffer[buffer_startpos]);
				}
			}
			move_buffer(1);
		}
		else { // insert code triple instead of the matching sequence!

			if (verbose) {
				printf("\nFound sequence seq_len=%d, offset=%d, at bufferstartpos=%d", best_seq_len, best_offset, buffer_startpos);
			}
			if (pass == 1) {
				//offsets[best_seq_offset]++;// += (best_seq_len - 2);
				seq_lens[best_seq_len]++;
			}
			if (pass == 2) {
				// write offset i.e. distance from end of match

				if (best_offset < lowest_special) {
					write_offset(best_offset);
				}
				else {
					int found = 0;
					for (long long i = 0; i < window_pages; i++) {

						if (best_offset < (lowest_special + (256 * (i + 1)))) {
							write_offset(best_offset - (lowest_special + (256 * i)));
							write_offset(255 - i);
							found = 1;
							break;
						}
					}
					if (!found) {
						printf("\n\n >>>>>>>> ERROR no offset coding found for offset = %d", best_offset);
					}
				}
				//subtract to so smallest will be 3 -2 = 1
				//len = 0 is used for code occurence
				if (best_seq_len < 255 + seqlen_add) {
					write_seqlen(best_seq_len - seqlen_add);  /* seqlen */
				}
				else {
					write_seqlen(best_seq_len - ((unsigned long)255 + seqlen_add));
					write_seqlen(255);
				}
				WRITE(code);  /* note file is read backwards during unpack! */
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
		WRITE(seqlen_add);
		WRITE(window_pages);
		WRITE(code);
		fclose(utfil);
	}
	fclose(infil);
	fclose(seq_lens_file);
	fclose(offsets_file);
}

void seq_pack(const char* source_filename, const char* dest_filename, unsigned char pages)
{
	buffer = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
	pack_internal(source_filename, dest_filename, 1, pages);
	pack_internal(source_filename, dest_filename, 2, pages);
}

