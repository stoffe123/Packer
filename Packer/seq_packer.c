#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#include <inttypes.h>
#include "packer_commons.h"
#include "seq_packer_commons.h"
#include "common_tools.h"
#include "seq_packer.h"

#define VERBOSE false

/* Sequence packer */

/* Global variables used in compressor */
static  FILE* infil, * utfil, * seq_lens_file, * offsets_file, * distances_file;

static buffer_size = BLOCK_SIZE * 3;
static  unsigned char buffer[BLOCK_SIZE * 3];

size_t absolute_end, buffer_endpos;
static bool useLongRange = false;
static bool useDistanceLongRange = true;
unsigned char lastByte, longRangeCode = 255;
static const wchar_t* base_dir = L"c:/test/";
static bool separate_files = false;

static uint32_t nextChar[BLOCK_SIZE * 2],
                lastChar[256];

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
	if (buffer_pos % 12000 == 0 && pass == 2 && !VERBOSE) {
		printf("*");
	}
}

void write_seqlen(uint64_t c) {
	//debug("\nwrite_seqlen:%d", c);
	fwrite(&c, 1, 1, (separate_files ? seq_lens_file : utfil));
}

void write_offset(uint64_t c) {
	//debug("\nwrite_offset:%d", c);
	fwrite(&c, 1, 1, (separate_files ? offsets_file : utfil));
}

void write_distance(uint64_t c) {
	//debug("\nwrite_offset:%d", c);
	fwrite(&c, 1, 1, (separate_files ? distances_file : utfil));
}


void out_seqlen(unsigned long seqlen, unsigned char pages, unsigned char seqlen_min) {
	unsigned int last_byte = 255;
	unsigned int lowest_special = last_byte + 1 - pages;
	assert(seqlen >= seqlen_min, "seqlen >= seqlen_min in seq_packer.out_seqlen");
	seqlen -= seqlen_min;
	//debug("\nseqlen after subtract %d = %d", seqlen_min, seqlen);
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
		assert(page < pages, "seq_packer.out_seqlen: no seqlen coding found");
	}
}

void out_distance(uint64_t distance, unsigned char pages, uint64_t distancePageMax) {
	if (distance < distancePageMax) {
		uint64_t last_byte = useDistanceLongRange ? 254 : 255;
		uint64_t lowest_special = last_byte + 1 - pages;
		if (distance < lowest_special) {
			write_distance(distance);
		}
		else {
			uint64_t page = 0;
			for (; page < pages; page++) {

				if (distance < (lowest_special + ((uint64_t)256 * (page + 1)))) {
					write_distance(distance - (lowest_special + ((uint64_t)256 * page)));
					write_distance(last_byte - page);//special code
					break;
				}
			}
			const char* msg = "seq_packer.out_distance: no coding found for distance:";
			assert(page < pages, msg);
		}
	}
	else {  // long range
		distance -= distancePageMax;
		//assert(distance < 65536, " distance too large in seqpacker");
		write_distance(distance / 65536);
		write_distance(distance / 256);
		write_distance(distance % 256);
		write_distance(longRangeCode);
	}
}

void out_offset(unsigned long offset, unsigned char pages, uint64_t offsetPageMax) {
	if (offset < offsetPageMax) {
		unsigned int last_byte = useLongRange ? 254 : 255;
		unsigned int lowest_special = last_byte + 1 - pages;
		if (offset < lowest_special) {
			write_offset(offset);
		}
		else {
			unsigned long page = 0;
			for (; page < pages; page++) {

				if (offset < (lowest_special + (256 * (page + 1)))) {
					write_offset(offset - (lowest_special + (256 * page)));
					write_offset(last_byte - page);//special code
					break;
				}
			}
			const char* msg = "seq_packer.out_offset: no offset coding found for offset:";
			assert(page < pages, msg);
		}
	}
	else {  // long range
		offset -= offsetPageMax;
		assert(offset < 65536, " offset < 65536 in seqpacker");
		write_offset(offset / 256);
		write_offset(offset % 256);
		write_offset(longRangeCode);
	}
}
//--------------------------------------------------------------------------------------------------------

void pack_internal(const wchar_t* src, const wchar_t* dest_filename, unsigned char pass,
	unsigned char offset_pages, unsigned char seqlen_pages, unsigned char distance_pages)
{
	useDistanceLongRange = true;
	unsigned int max_seqlen = 258 - seqlen_pages + seqlen_pages * 256; //  -seqlen_pages är nog fel!!
	uint64_t offsetPagesMax = offset_pages * (uint64_t)256 + (useLongRange ? 255 : 256),
		distancePagesMax = distance_pages * (uint64_t)256 + (useDistanceLongRange ? 255 : 256),
		offset,
		winsize = offsetPagesMax + (useLongRange ? 65536 : 0) + max_seqlen + max_seqlen - 3,  // -3 safety!
		best_offset = 0, lowest_special_offset = ((useLongRange ? (uint64_t)255 : (uint64_t)256) - offset_pages),
		min_seq_len = 3, offsets[1024] = { 0 }, seq_lens[258] = { 0 },
		longest_offset = lowest_special_offset + ((uint64_t)256 * offset_pages) - 1;
	long long  best_seqlen, seq_len;
	uint64_t distance = 1; // point one byte past the beginning!
	uint32_t buffer_pos = 0;
	debug("\n Seq_packer pass: %d", pass);

	infil = openRead(src);
	
	if (pass == 2) {
		
		
		debug("\nWinsize: %d", winsize);
		debug("\npages: (%d, %d, %d)", offset_pages, seqlen_pages, distance_pages);
		debug("\nmax_seqlen: %d", max_seqlen);
		
		
	

		if (separate_files) {
			const wchar_t seqlens_name[100] = { 0 };
			const wchar_t offsets_name[100] = { 0 };
			const wchar_t distances_name[100] = { 0 };
			concatw(seqlens_name, base_dir, L"seqlens");
			concatw(offsets_name, base_dir, L"offsets");
			concatw(distances_name, base_dir, L"distances");
			seq_lens_file = openWrite(seqlens_name);
			offsets_file = openWrite(offsets_name);
			distances_file = openWrite(distances_name);
		}
		utfil = openWrite(dest_filename);
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
				int32_t nextChar_pos = buffer_pos, nextCh;
				long diff;
				while (offset < offset_max)
				{
					// find matching sequence		
					if (((nextCh = nextChar[nextChar_pos]) == 0) ||
						((offset += nextCh) >= offset_max)) {
						break;
					}
					nextChar_pos += nextCh;
					if ((offset < 3) || 
						(buffer[buffer_pos + 1] != buffer[buffer_pos + offset + 1]) ||
						(buffer[buffer_pos + 2] != buffer[buffer_pos + offset + 2])) {
						continue;
					}
					seq_len = 3;

					while (buffer[buffer_pos + seq_len] == buffer[buffer_pos + offset + seq_len] && 
						seq_len < offset &&
						buffer_pos + offset + seq_len < absolute_end - 1 &&
						seq_len < max_seqlen)
					{
						seq_len++;
					}
					diff = (buffer_pos + offset + seq_len) - absolute_end;
					if ((diff > 0) || ((diff = (buffer_pos + seq_len) - buffer_endpos) > 0)) {
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
						if (offset >= offsetPagesMax && seq_len < 6) {
							continue;
						}
						best_seqlen = seq_len;
						best_offset = offset - seq_len;
						if (best_seqlen >= max_seqlen) {
							best_seqlen = max_seqlen;
							break;
						}						
					}
				}
			}
		}
		/* now we found the longest sequence in the window! */
		unsigned char seqlen_min = getSeqlenMin(best_offset, lowest_special_offset, offsetPagesMax);
			
		if (best_seqlen < seqlen_min)
		{       /* no sequence found, move window 1 byte forward and read one more byte */
			if (pass == 1) {
				updateNextCharTable(ch, buffer_pos);
			}
			else {
				WRITE(utfil, ch);
				assert(absolute_end < buffer_size, "absolute_end < buffer_size in seqpacker");
				updateNextCharTable(ch, absolute_end);
				buffer[absolute_end++] = ch; // write start to end to wrap-around find sequences					
				distance++;
			}
			buffer_pos++;
			display_progress(buffer_pos, pass);
		}
		else { // insert code triple instead of the matching sequence!
			//debug("\n  seqlenmin:%d  offset:%d", seqlen_min, best_offset);
			assertSmallerOrEqual(buffer_pos + best_offset + best_seqlen + best_seqlen, absolute_end,
				"buffer_startpos + best_offset + best_seqlen*2 <= absolute_end in seq_packer.pack_internal");


			//debug("Found sequence seq_len=%d, offset=%d, at bufferstartpos=%d", best_seq_len, best_offset, buffer_startpos);

			if (pass == 1) {
				//offsets[best_seq_offset]++;// += (best_seq_len - 2);
				seq_lens[best_seqlen]++;
			}
			else if (pass == 2) {  // Write triplet!

				out_offset(best_offset, offset_pages, offsetPagesMax);

				out_seqlen(best_seqlen, seqlen_pages, seqlen_min);
				//if (best_seqlen > 230) {
				if (VERBOSE) {
					printf("\n(%d, %d, %d)  buffer_startpos %d   buffer_endpos %d  seq \"", best_seqlen, best_offset, distance, buffer_pos, buffer_endpos);
					for (int i = 0; i < best_seqlen; i++) {
						printf("%d ", buffer[buffer_pos + i]);
					}
					printf("\"");
				}

				//}
			    /* note file is read backwards during unpack! */
				out_distance(distance, distance_pages, distancePagesMax);
				distance = 0;
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
	
		out_distance(distance, distance_pages, distancePagesMax);
		debug("\n distance to first code: %d", distance);
		bool slimCase = (seqlen_pages + offset_pages + distance_pages == 0);
		if  (!slimCase) {
			WRITE(utfil, seqlen_pages);
			WRITE(utfil, offset_pages);
			WRITE(utfil, distance_pages);
		}
		unsigned char packType = 0;
		if (useLongRange) {
			packType = setKthBit(packType, 1);
		}
		if (slimCase) {
			packType = setKthBit(packType, 2);
		}
		WRITE(utfil, packType);	
		fclose(utfil);
		if (separate_files) {
			fclose(seq_lens_file);
			fclose(offsets_file);
			fclose(distances_file);
		}
	}
	fclose(infil);
}

unsigned char check_pages(pages, size) {
	int64_t new_pages = 255;
	if ((int64_t)256 + pages * (int64_t)256 >= size) {
		new_pages = size / (int64_t)256;
		uint64_t overflow = size % (int64_t)256;
		if (overflow < 127 && new_pages > 0) {
			new_pages--;
		}
	}
	if (new_pages < pages) {
		pages = new_pages;
	}
	return pages;
}

void seq_pack_internal(const wchar_t* source_filename, const wchar_t* dest_filename, packProfile profile, bool sep) {
	unsigned char offset_pages = profile.offset_pages,
		seqlen_pages = profile.seqlen_pages,
		distance_pages = profile.distance_pages;
	long long source_size = get_file_size_from_wname(source_filename);
	printf("\nPages (%d,%d)", offset_pages, seqlen_pages);
	unsigned char new_offset_pages = check_pages(offset_pages, source_size);
	useLongRange = true;
	if (new_offset_pages < offset_pages) {
		offset_pages = new_offset_pages;
		useLongRange = false;
	}
	seqlen_pages = check_pages(seqlen_pages, source_size);
	printf("=> (%d,%d)", offset_pages, seqlen_pages);
	separate_files = sep;
	for (int i = 0; i < 256; i++) {
		lastChar[i] = INT32_MAX;
	}
	for (int i = 0; i < BLOCK_SIZE * 2; i++) {
		nextChar[i] = 0;
	}
	pack_internal(source_filename, dest_filename, 1, offset_pages, seqlen_pages, distance_pages);
	pack_internal(source_filename, dest_filename, 2, offset_pages, seqlen_pages, distance_pages);
}

void seq_pack(const char* src, const char* dst, packProfile profile)
{
	wchar_t u1[500], u2[500];
	toUni(u1, src);
	toUni(u2, dst);
	seq_pack_internal(u1, u2, profile, false);
}

void seqPack(const wchar_t* src, const wchar_t* dst, packProfile profile) {
	seq_pack_internal(src, dst, profile, false);
}

void seq_pack_separate(const char* src, const char* dir, packProfile profile) {
	wchar_t srcw[500], dirw[500];
	toUni(srcw, src);
	toUni(dirw, dir);
	seqPackSeparate(srcw, dirw, profile);
}

void seqPackSeparate(const wchar_t* src, const wchar_t* dir, packProfile profile) {
	const wchar_t dest_filename[100] = { 0 };
	concatw(dest_filename, dir, L"main");
	base_dir = dir;
	seq_pack_internal(src, dest_filename, profile, true);
}

