#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
//#include "huffman2.h"
//#include "canonical.h"
#include "multi_packer.h"

#define TRUE 1
#define FALSE 0
#define END 999

#define math_max(x,y) ((x) >= (y)) ? (x) : (y)
#define math_min(x,y) ((x) <= (y)) ? (x) : (y)

#define CMP_N 256



void print_string_rep(const char* tt) {
	printf("\n");
	for (int i = 0; i < 16; i++) {
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
	if (f1_size != f2_size) {
		printf("\n File lengths differ! %d, %d", f1_size, f2_size);
		return 0;
	}
	char tmp1[CMP_N], tmp2[CMP_N];

	size_t bytes = 0, readsz = sizeof(tmp1);
	int count = 0;
	while (!feof(f1) && !feof(f2)) {
		fread(tmp1, sizeof * tmp1, readsz, f1);
		fread(tmp2, sizeof * tmp2, readsz, f2);
		count += 16;
		if (memcmp(tmp1, tmp2, readsz)) {
			printf("\n File contents differ at position %d :", count);
			printf("\n File1:");
			print_string_rep(tmp1);
			printf("\n File2:");
			print_string_rep(tmp2);
			return 0;
		}
	}
	fclose(f1);
	fclose(f2);
	return 1;
}

int main()
{	
	const char* src = "C:/test/book.txt"; const char* dst = "C:/test/unp";

	const char* packed_name = "c:/test/packed.bin";
	const char* packed_name2 = "c:/test/packed2.bin";

	int cl = clock();
	//printf("\n packing... ");

	long long size_org = get_file_size_from_name(src);
	printf("\nLength of source : %d", size_org);

	//rle_pack(src, "C:/test/packed.rle");

	//pack(src, "c:/test/packed.bin");
	printf("\n packing... %s", src);
	
	//seq_pack(src, packed_name);
	multi_pack(src, packed_name);	

	int pack_time = (clock() - cl);
	printf("\n Packing finished time it took: %d", pack_time);
	long long size_packed = get_file_size_from_name(packed_name);
	printf("\nLength of packed: %d", size_packed);
	printf("\nCompression ratio: %f", (double)size_packed / (double)size_org);
	printf("\n\n unpacking... packed.bin");
	cl = clock();
	multi_unpack(packed_name, dst);
	//seq_unpack(packed_name, dst);	

	int unpack_time = (clock() - cl);
	printf("\n Unpacking finished time it took: %d", unpack_time);
	printf("\n Total time %d", pack_time + unpack_time);
	//unpack("C:/test/packed.bin", dst);
	//printf("\nRLE unpacking... ");
	//rle_unpack("c:/test/unpackedrle.bin", dst);

	printf("\n\n Comparing files!");
	if (files_equal(src, dst)) {
		printf("\n ***** SUCCESS ***** (files equal)\n");
	}
	else {
		printf("\n >>>>>>>>>>>>>>> FILES NOT EQUAL!!!! <<<<<<<<<<<<<<<<<<");
	}
	return 0;
}










