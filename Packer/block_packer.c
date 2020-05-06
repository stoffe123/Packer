#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "canonical.h"

// 4 x 65536
#define BLOCK_SIZE 65536



// tar contents of src => utfil
append_to_tar(FILE* utfil,char* src) {
	//write size
	uint32_t size = get_file_size_from_name(src);
	fwrite(&size, 1, 4, utfil);

	//write contents
	append_to_file(utfil, src);
}


//----------------------------------------------------------------------------------------

	void block_pack(const char* src, const char* dst, unsigned char offset_pages,
		unsigned char seqlen_pages) {

		unsigned long long  src_size = get_file_size_from_name(src);

		FILE* utfil, *infil;
		utfil = fopen(dst, "wb");
		infil = fopen(src, "rb");

		uint32_t tmp_size;
		do {
			char* base_dir = get_clock_dir();
			char* tmp = get_temp_file(base_dir);
			char* tmp2 = get_temp_file(base_dir);
			copy_chunk(infil, tmp, BLOCK_SIZE);
			tmp_size = get_file_size_from_name(tmp);
			multi_pack(tmp, tmp2, offset_pages, seqlen_pages);
			append_to_tar(utfil, tmp2);
			remove(tmp);
			remove(tmp2);
		} while (tmp_size == BLOCK_SIZE);

		fclose(infil);
		fclose(utfil);
	}


// ----------------------------------------------------------------

	void block_unpack(const char* src, const char* dst) {
		FILE* utfil, * infil;
		utfil = fopen(dst, "wb");
		infil = fopen(src, "rb");

		uint32_t size;
		while (fread(&size, 1, 4, infil) == 4) {

	
			char* tmp = get_temp_file();
			char* tmp2 = get_temp_file();
			copy_chunk(infil, tmp, size);

			multi_unpack(tmp, tmp2);

			append_to_file(utfil, tmp2);
			remove(tmp);
			remove(tmp2);
		}
		fclose(infil);
		fclose(utfil);
	}




