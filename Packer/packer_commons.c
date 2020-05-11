#include <stdbool.h>
#include "common_tools.h"


const char** get_test_filenames() {

	//really strange that you have to duplicate this here but can't find a way to receive it
	char test_filenames[16][100] = { "bad.cdg","repeatchar.txt", "onechar.txt", "empty.txt",  "oneseq.txt", "book_med.txt","book.txt",
			 "amb.dll",
			 "rel.pdf",
			 "nex.doc",
			"did.csh",
			 "aft.htm",
			 "tob.pdf",
		 "pazera.exe",
		"voc.wav",
		 "bad.mp3"



	};
	printf("test_filenames 0: %s", test_filenames[0]);
	return test_filenames;
}

void CanonicalDecodeAndReplace(const char* src) {
	printf("\n Canonical unpacking (in place) %s", src);
	const char* tmp = get_temp_file2("multi_canonicaldec");
	CanonicalDecode(src, tmp);
	remove(src);
	rename(tmp, src);
}

bool CanonicalEncodeAndTest(const char* src) {
	char* tmp = get_temp_file2("multi_canonicaled");
	CanonicalEncode(src, tmp);
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name(tmp);
	//printf("\n CanonicalEncode:%s  (%f)", src, (double)size_packed / (double)size_org);
	bool compression_success = (size_packed < size_org);
	if (compression_success) {
		remove(src);
		rename(tmp, src);
	}
	else {
		remove(tmp);
	}
	return compression_success;
}


bool SeqPackAndTest(const char* src, int seqlen_pages, int offset_pages, int ratio_limit) {	
	char* tmp = get_temp_file2("multi_seqpacked");
	seq_pack(src, tmp, seqlen_pages, offset_pages);
	int size_org = get_file_size_from_name(src);
	int size_packed = get_file_size_from_name(tmp);
	double pack_ratio = (double)size_packed / (double)size_org;
	printf("\n SeqPacked:%s  got ratio: %f", src, pack_ratio);
	bool compression_success = (pack_ratio < ((double)ratio_limit / (double)100));
	if (compression_success) {
		remove(src);
		rename(tmp, src);
	}
	else {
		remove(tmp);
	}
	return compression_success;
}