#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "seq_unpacker.h"  
#include "seq_packer.h"  
#include "common_tools.h"
#include "multi_packer.h"
#include "canonical.h"

// tar contents of src => utfil
append_to_tar(FILE* utfil,char* src) {
	//write size
	uint32_t size = get_file_size_from_name(src);
	fwrite(&size, sizeof(size), 1, utfil);

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

		uint64_t tmp_size;
		do {
			char* tmp = get_temp_file2("block_chunck");
			uint64_t read_size = BLOCK_SIZE;
			
			//workaround for a bug that even 2 powers cause bug
			//this bug should be fixed but it is hard
			if (read_size % 256 == 0) {
				read_size--;
			}
			copy_chunk(infil, tmp, read_size);
			tmp_size = get_file_size_from_name(tmp);

			char* tmp2 = get_temp_file2("block_multipacked");
			multi_pack(tmp, tmp2, offset_pages, seqlen_pages);
			remove(tmp);

			append_to_tar(utfil, tmp2);
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
		while (fread(&size, sizeof(size), 1, infil) == 1) {

	
			char* tmp = get_temp_file2("block_tobeunpacked");
			copy_chunk(infil, tmp, size);

			char* tmp2 = get_temp_file2("block_multiunpacked");
			multi_unpack(tmp, tmp2);

			remove(tmp);

			append_to_file(utfil, tmp2);
			
			remove(tmp2);
		}
		fclose(infil);
		fclose(utfil);
	}




