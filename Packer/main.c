#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"
#include "seq_unpacker.h"  
#include "seq_packer.h"  
//#include "huffman2.h"
//#include "canonical.h"
#include "multi_packer.h"

#include "RLE_packer_advanced.h"

#define CMP_N 128



void print_string_rep(unsigned char* tt) {
	printf("\n");
	for (int i = 0; i < CMP_N; i++) {
		printf("%d, ", tt[i]);
	}
}

int files_equal(const char* source_filename, const char* dest_filename) {
	FILE* f1, * f2;

	fopen_s(&f1, source_filename, "rb");
	if (!f1) {
		printf("\nHittade inte utfil: %s", source_filename);
		getchar();
		exit(1);
	}

	fopen_s(&f2, dest_filename, "rb");
	if (!f2) {
		puts("Hittade inte utfil!");
		getchar();
		exit(1);
	}
	long f1_size = get_file_size(f1);
	long f2_size = get_file_size(f2);
	int res = 1;
	if (f1_size != f2_size) {
		printf("\n >>>>>>>>>>>>>>> FILES NOT EQUAL!!!! <<<<<<<<<<<<<<<<<<");
		printf("\n Lengths differ   %d  %d", f1_size, f2_size);
		res = 0;
	}
	unsigned char tmp1, tmp2;

	size_t bytes = 0;
	int count = 0;
	while (!feof(f1) && !feof(f2)) {
		fread(&tmp1, 1, 1, f1);
		fread(&tmp2, 1, 1, f2);

		if (tmp1 != tmp2) {

			printf("\n Contents differ at position  %d ", count);
			printf("\n File1:");
			printf("%c", tmp1);
			//print_string_rep(tmp1);
			printf("\n File2:");
			//print_string_rep(tmp2);
			printf("%c", tmp2);
			return 0;
		}
		count++;
	}
	fclose(f1);
	fclose(f2);
	return res;
}

int main()
{
	char filenames[16][100] = { "bad.cdg","repeatchar.txt", "onechar.txt", "empty.txt",  "oneseq.txt",
		 "book_med.txt","book.txt",
			
		  	 "amb.dll",
			 "rel.pdf",
			 "nex.doc",	
		  	"did.csh",
			 "bad.mp3",
			
			 "aft.htm",
		     "pazera.exe",
		     "tob.pdf",
		"voc.wav"


		    
	};
	unsigned long long acc_size = 0;

	int before_suite = clock();
	for (int kk = 0; kk < 16; kk++) 
	{
		const char* src = concat("C:/test/testsuite/", filenames[kk]);
		const char* dst = "C:/test/unp";

		const char* packed_name = "c:/test/packed.bin";

		unsigned long int best_size = ULONG_MAX;
		unsigned char best_page = 0;

		long long size_org = get_file_size_from_name(src);

		printf("\n Packing... %s with length:%d", src, size_org);

		//int i = 0;
		//for (int i = 4; i < 162; i++) 
		{

			int cl = clock();

			//	printf("\n\n  ------- Pages %d --------- ", i);

			two_byte_pack(src, packed_name, 60, 15);


			//RLE_simple_pack(src, packed_name);

			int pack_time = (clock() - cl);
			//printf("\n Packing finished time it took: %d", pack_time);
			long long size_packed = get_file_size_from_name(packed_name);

			printf("\nLength of packed: %d", size_packed);
			printf("  (%f)", (double)size_packed / (double)size_org);

			acc_size += size_packed;

			printf("\n Accumulated size %d kb", acc_size / 1024);

			/*
			if (size_packed < best_size) {
				best_size = size_packed;
				best_page = i;
				printf("\n BEST! ");
			}
			*/
			//printf("\n\n unpacking... packed.bin");
			cl = clock();

			two_byte_unpack(packed_name, dst);


			int unpack_time = (clock() - cl);
			//printf("\n Unpacking finished time it took: %d", unpack_time);
			printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);


			//printf("\n\n Comparing files!");
			if (files_equal(src, dst)) {
				printf("\n ****** SUCCESS ****** (equal)\n");
			}
			else {
				return 1;
			}
		}
		//printf("\n Best page=%d, size=%d", best_page, best_size);
		
	}
	long total_time = clock() - before_suite;
	printf("\n\n **** ALL SUCCEEDED **** Accumulated size %d kb (%d) ************", acc_size / 1024, acc_size);
	printf("\n\n Total time %d seconds     (%d)", total_time / 1000, total_time);
}










