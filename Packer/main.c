#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"  
#include "seq_packer.h"  
#include "multi_packer.h"
#include "block_packer.h"
#include <Windows.h>
#include "packer_commons.h"
#include "Two_byte_packer.h"
#include "RLE_packer_advanced.h"
#include "archive_packer.h"
#include "halfbyte_rle_packer.h"

//#include "huffman2.h"
//#include "canonical.h"

#define CMP_N 128

void print_string_rep(unsigned char* tt) {
	printf("\n");
	for (int i = 0; i < CMP_N; i++) {
		printf(", ", tt[i]);
	}
}

int fuzz(int i) {
	do {
		if (rand() % 2 == 0) {
			i += (rand() % 4);
		}
		else {
			i -= (rand() % 4);
		}
	} while (!(i >= 0 && i <= 248));
	return i;
}

int fuzzVal(int r, int add) {
	if (rand() % 2 == 0) {
		r += (rand() % add + 1);
	}
	else {
		r -= (rand() % add + 1);
	}
	return r;
}

int doFuzz(int r, int best, int min, int max) {
	int add = (max - min) / 10;
	if (add < 2) {
		add = 2;
	}
	r = fuzzVal(r, add);
	if (r < 0) {
		r = min;
	}
	if (r < min) {
		r = min;
	}
	if (r > max) {
		r = max;
	}
	if (rand() % 4 == 0) {
		r = best;
	}
	return r;
}

void fuzzProfile(packProfile* profile, packProfile best) {

	if (rand() % 3 != 1) {
		copyProfile(&best, profile);
    }

	profile->rle_ratio = doFuzz(profile->rle_ratio, best.rle_ratio, 10, 100);
	profile->twobyte_ratio = doFuzz(profile->twobyte_ratio, best.twobyte_ratio, 10, 100);
	profile->recursive_limit = doFuzz(profile->recursive_limit, best.recursive_limit, 10, 700);

	profile->twobyte_threshold_max = doFuzz(profile->twobyte_threshold_max, best.twobyte_threshold_max, 3, 13000);
	profile->twobyte_threshold_divide = doFuzz(profile->twobyte_threshold_divide, best.twobyte_threshold_divide, 20, 4000);
	profile->twobyte_threshold_min = doFuzz(profile->twobyte_threshold_min, best.twobyte_threshold_min, 3, 1000);

	profile->seqlenMinLimit3 = doFuzz(profile->seqlenMinLimit3, best.seqlenMinLimit3, 0, 512);
	profile->seqlenMinLimit4 = doFuzz(profile->seqlenMinLimit4, best.seqlenMinLimit4, 20000, 90000);
	profile->blockSizeMinus = doFuzz(profile->blockSizeMinus, best.blockSizeMinus, 0, 255);
	profile->winsize = doFuzz(profile->winsize, best.winsize, 5000, 160000);

	profile->sizeMaxForCanonicalHeaderPack = doFuzz(profile->sizeMaxForCanonicalHeaderPack, best.sizeMaxForCanonicalHeaderPack, 80, 1200);
	profile->sizeMinForCanonical = doFuzz(profile->sizeMinForCanonical, best.sizeMinForCanonical, 10, 700);
	profile->sizeMinForSeqPack = doFuzz(profile->sizeMinForSeqPack, best.sizeMinForSeqPack, 10, 93000);
	profile->sizeMaxForSuperslim = doFuzz(profile->sizeMaxForSuperslim, best.sizeMaxForSuperslim, 10, 100000);
}


unsigned long long presentResult(bool earlyBreak, uint64_t total_time, unsigned long long acc_size, unsigned long long acc_size_org,
	unsigned long long best_size, packProfile profile, packProfile* best) {
	if (!earlyBreak) {
		double size_kb = (double)acc_size / (double)1024;
		printf("\n\n **** ALL SUCCEEDED ****\n %.0f kb   (%llu)", size_kb, acc_size);
		double time_sec = (double)total_time / 1000.0;
		printf("\n\Time %.0fs  (%d)", time_sec, total_time);
		if (acc_size_org > 0) {
			double pack_ratio = (double)acc_size / (double)acc_size_org;
			printf("\nPack Ratio %.2f%%", pack_ratio * (double)100);
		}

		double eff = size_kb / time_sec;
		printf("\nEfficiency %.2f kb/s\n\n", eff);

		if (best_size == 0 || (acc_size < best_size && acc_size != 0)) {
			copyProfile(&profile, best);
			best_size = acc_size;
			printf("\n\n\a ************ BEST FOUND *************");
		}
		else {
			printf("\n Not enough this time...\n");
		}
	}
	printf("\n  STATUS:\n%.0f kb   (%llu)", round((double)best_size / 1024.0), best_size);
	printProfile(best);
	printf("\n");
	return best_size;
}

bool isEarlyBreak(uint64_t best_size, uint64_t acc_size_packed, uint64_t before_suite, uint64_t seconds) {
	if ((best_size > 0 && acc_size_packed > best_size)) {
		printf("\n early break because of too big size.. size had become: %d", acc_size_packed);
		return true;
	}
	int time = clock() - before_suite;
	if (time > (seconds * 1000)) {
		printf("\n early break because of too big time, time was %d", time);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------

void testmeta() {

	/// <summary>
	/// TODO refactor to wchar!!!
	/// </summary>
	
	packProfile bestProfile, bestOffsetProfile, bestDistanceProfile;

	//meta testsuit 1170047  / 33s
	static packProfile seqlenProfile = {
		.rle_ratio = 31,
		.twobyte_ratio = 97,
		.recursive_limit = 118,
		.twobyte_threshold_max = 5866,
		.twobyte_threshold_divide = 1979,
		.twobyte_threshold_min = 160,
		.seqlenMinLimit3 = 58,
		.seqlenMinLimit4 = 20000,
		.winsize = 77525,
		.sizeMaxForCanonicalHeaderPack = 175,
		.sizeMinForSeqPack = 5690,
		.sizeMinForCanonical = 30,
		.sizeMaxForSuperslim = 16384
	},
    offsetProfile = {
			.rle_ratio = 78,
			.twobyte_ratio = 93,
			.recursive_limit = 58,
			.twobyte_threshold_max = 10970,
			.twobyte_threshold_divide = 2520,
			.twobyte_threshold_min = 411,
			.seqlenMinLimit3 = 82,
			.seqlenMinLimit4 = 20000,
			.winsize = 92198,
			.sizeMaxForCanonicalHeaderPack = 563,
			.sizeMinForSeqPack = 4753,
			.sizeMinForCanonical = 302,
			.sizeMaxForSuperslim = 16384
	},
	distanceProfile = {
			.rle_ratio = 72,
			.twobyte_ratio = 100,
			.recursive_limit = 40,
			.twobyte_threshold_max = 3641,
			.twobyte_threshold_divide = 3609,
			.twobyte_threshold_min = 37,
			.seqlenMinLimit3 = 35,
			.seqlenMinLimit4 = 20000,
			.winsize = 83179,
			.sizeMaxForCanonicalHeaderPack = 214,
			.sizeMinForSeqPack = 10,
			.sizeMinForCanonical = 363,
			.sizeMaxForSuperslim = 16384
	};
	
	copyProfile(&seqlenProfile, &bestProfile);
	copyProfile(&offsetProfile, &bestOffsetProfile);
	copyProfile(&distanceProfile, &bestDistanceProfile);
	bool unpack = true;
	unsigned long long best_size = 0;
	uint64_t timeLimit = 34;
	while (true) {
		/*
		seqlenProfile.offset_pages = 0;
		seqlenProfile.seqlen_pages = 0;
		offsetProfile.offset_pages = 0;
		offsetProfile.seqlen_pages = 0;
		*/
		
		unsigned long long acc_size_packed = 0,
			acc_size_org = 0;

		uint64_t before_suite = clock(), totalTime = 0;
		int kk = 0;
		bool earlyBreak = true;
		printf("\n Now testing with seqlenprofile:");
		printProfile(&seqlenProfile);
		printf("\n\n and offset profile:");
		printProfile(&offsetProfile);
		
		printf("\n-------------------------------");
		for (; kk < 38; kk++)
		{
			const wchar_t src[100] = { 0 };

			const wchar_t* metaDir = L"D:/Dropbox/Personal/Programmering/Compression/test/meta/";
			concat(src, metaDir, "seqlens");
			concat_int(src, src, kk + 101);			
			const wchar_t* unpackedFilename = L"C:/test/unp";
			const wchar_t* packed_name = L"c:/test/packed.bin";
			long long size_org = get_file_size_from_wname(src);
			printf("\n Packing %s      length:%d", src, size_org);
			int cl = clock();
			//multi_pack(src, packed_name, seqlenProfile, seqlenProfile, offsetProfile, distanceProfile);
				
			long long size_packed = get_file_size_from_wname(packed_name);
			acc_size_packed += size_packed;
			if (isEarlyBreak(best_size, acc_size_packed, before_suite, timeLimit)) {
				earlyBreak = true;
				break;
			}
			acc_size_org += size_org;
			if (unpack) {
				//multi_unpack(packed_name, unpackedFilename);
				printf("\n\n Comparing files!");
				if (files_equalw(src, unpackedFilename)) {
					printf(" ****** SUCCESS ****** (equal)\n");
				}
				else {
					exit(1);
				}
			}


			concat(src, metaDir, "offsets");
			concat_int(src, src, kk + 101);
			size_org = get_file_size_from_wname(src);
			printf("\n Packing... %s with length:%d", src, size_org);
			
			//multi_pack(src, packed_name, offsetProfile, seqlenProfile, offsetProfile, distanceProfile);
			int pack_time = (clock() - cl);
		
			size_packed = get_file_size_from_wname(packed_name);
			acc_size_packed += size_packed;
			if (isEarlyBreak(best_size, acc_size_packed, before_suite, timeLimit)) {
				earlyBreak = true;
				break;
			}
			acc_size_org += size_org;
			if (unpack) {
				//multi_unpack(packed_name, unpackedFilename);
				printf("\n\n Comparing files!");
				if (files_equalw(src, unpackedFilename)) {
					printf(" ****** SUCCESS ****** (equal)\n");
				}
				else {
					exit(1);
				}
			}

			concat(src, metaDir, "distances");
			concat_int(src, src, kk + 101);
			size_org = get_file_size_from_wname(src);
			printf("\n Packing... %s with length:%d", src, size_org);

			//multi_pack(src, packed_name, distanceProfile, seqlenProfile, offsetProfile, distanceProfile);
			pack_time = (clock() - cl);

			size_packed = get_file_size_from_wname(packed_name);
			acc_size_packed += size_packed;
			
			if (isEarlyBreak(best_size, acc_size_packed, before_suite, timeLimit)) {
				earlyBreak = true;
				break;
			}
			acc_size_org += size_org;

			//if (unpack) multi_unpack(packed_name, unpackedFilename);

			printf("\n Accumulated size %d kb", acc_size_packed / 1024);
			cl = clock();

			int unpack_time = (clock() - cl);
			//printf("\n Unpacking finished time it took: %d", unpack_time);
			printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);

			if (unpack) {
				printf("\n\n Comparing files!");
				if (files_equalw(src, unpackedFilename)) {
					printf(" ****** SUCCESS ****** (equal)\n");
				}
				else {
					exit(1);
				}
			}
		
			totalTime = clock() - before_suite;
			earlyBreak = isEarlyBreak(best_size, acc_size_packed, before_suite, timeLimit);
		}//end for
		unsigned long long old_best_size = best_size;
		best_size = presentResult(earlyBreak, totalTime, acc_size_packed, acc_size_org, best_size, seqlenProfile, &bestProfile);
		
		if (old_best_size != best_size) {
			old_best_size = best_size;
			copyProfile(&offsetProfile, &bestOffsetProfile);	
			copyProfile(&distanceProfile, &bestDistanceProfile);
		}
		printf("\n>>Offset Profile");
		printProfile(&bestOffsetProfile);
		printf("\n\n>>Distance Profile");
		printProfile(&bestDistanceProfile);
		printf("\n-------------------------------\n");
		fuzzProfile(&seqlenProfile, bestProfile);
		fuzzProfile(&offsetProfile, bestOffsetProfile);
		fuzzProfile(&distanceProfile, bestDistanceProfile);
	}
}//end test_meta

//-------------------------------------------------------------------------------------------

void onefile() {

	int before_suite = clock();

	const wchar_t* src = L"c:/test/canonical_unp";
	const wchar_t* unpackedFilename = L"C:/test/unp";

	const wchar_t* packed_name = L"c:/test/packed.bin";

	long long size_org = get_file_size_from_wname(src);

	wprintf(L"\n Packing... %s with length:%d", src, size_org);

	int cl = clock();

	packProfile profile = getPackProfile();
	profile.rle_ratio = 85;
	profile.twobyte_ratio = 90;
	profile.recursive_limit = 242;
	profile.twobyte_threshold_max = 200;
	profile.twobyte_threshold_divide = 1111;
	profile.twobyte_threshold_min = 50;

	//meta testsuit 838297
	packProfile seqlenProfile = getPackProfile();
	seqlenProfile.rle_ratio = 64;
	seqlenProfile.twobyte_ratio = 62;
	seqlenProfile.recursive_limit = 10;
	seqlenProfile.twobyte_threshold_max = 8373;
	seqlenProfile.twobyte_threshold_divide = 3598;
	seqlenProfile.twobyte_threshold_min = 24;
	

	packProfile offsetProfile = getPackProfile(72, 56, 10);
	offsetProfile.rle_ratio = 56;
	offsetProfile.twobyte_ratio = 82;
	offsetProfile.recursive_limit = 220;
	offsetProfile.twobyte_threshold_max = 11235;
	offsetProfile.twobyte_threshold_divide = 1906;
	offsetProfile.twobyte_threshold_min = 913;
	offsetProfile.seqlenMinLimit3 = 20;

	packProfile distanceProfile = getPackProfile();
	distanceProfile.rle_ratio = 95;
	distanceProfile.twobyte_ratio = 100;
	distanceProfile.recursive_limit = 90;
	distanceProfile.twobyte_threshold_max = 2365;
	distanceProfile.twobyte_threshold_divide = 2281;
	distanceProfile.twobyte_threshold_min = 64;
	distanceProfile.seqlenMinLimit3 = 20;
	uint64_t size_packed;
	memfile* srcm = getMemfileFromFile(src);
	
	//seqPackBundle packed = seqPackSep(srcm, profile);
	//size_packed = getBundleSize(packed);
	//memfile* packed = multiPack2(srcm, profile, seqlenProfile, offsetProfile, distanceProfile);
	//memfile* packed = halfbyteRlePack(srcm, 0);
	memfile* packed = CanonicalEncodeMem(srcm);
	size_packed = getMemSize(packed);
	printf("\n size_packed %d", size_packed);
	int pack_time = (clock() - cl);
		
	cl = clock();

	memfile* unpacked = CanonicalDecodeMem(packed);
	//memfile* unpacked = multiUnpack2(packed);
	//memfile* unpacked = seqUnpack(packed);
	//memfile* unpacked = halfbyteRleUnpack(packed, 0);
	

	int unpack_time = (clock() - cl);
	//printf("\n Unpacking finished time it took: %d", unpack_time);
	printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);
	
	wprintf(L"\n\n   --   RATIO OF PACKED   '%s'   %.2f%%   --\n\n", src, ((double)size_packed / (double)size_org) * 100.0);

	memfileToFile(unpacked, unpackedFilename);
	printf("\n\n Comparing files!");

	if (files_equalw(src, unpackedFilename)) {
		printf("\n ****** SUCCESS ****** (equal)\n");
	}
	else {
		exit(1);
	}
}//onefile

//---------------------------------------------------------------------------------------

void test16() {
	
	wchar_t test_filenames[16][100] = {
		
			L"rel.pdf",
		L"book.txt",
		L"repeatchar.txt",
		L"oneseq.txt",
		L"empty.txt",
		L"onechar.txt",
		
		L"book_med.txt",
		
		L"did.csh",
		L"voc.wav",
	
		L"tob.pdf",
		L"pazera.exe",
		L"amb.dll",
        L"bad.cdg",
		L"bad.mp3",
		L"nex.doc",		
		L"aft.htm"
		
			
	};
	
	packProfile bestProfile, 
		profile = {
			.rle_ratio = 76,
			.twobyte_ratio = 82,
			.recursive_limit = 437,
			.twobyte_threshold_max = 11371,
			.twobyte_threshold_divide = 20,
			.twobyte_threshold_min = 718,
			.seqlenMinLimit3 = 111,
			.seqlenMinLimit4 = 72867,
			.blockSizeMinus = 112,
			.winsize = 18233,
			.sizeMaxForCanonicalHeaderPack = 332,
			.sizeMinForSeqPack = 6934,
			.sizeMinForCanonical = 124,
			.sizeMaxForSuperslim = 16921
	};

	copyProfile(&profile, &bestProfile);
	uint64_t timeLimit = 650;
	bool unpack = true;
	unsigned long long best_size = 0; // 44127835; // (43094 kb)
	while (true) 
	{
		
		unsigned long long acc_size_packed = 0,
			acc_size_org = 0;

		uint64_t before_suite = clock(), totalTime = 0;
		int kk = 0;
		bool earlyBreak = true;
		for (; kk < 16; kk++)
		{
			const wchar_t src[200] = { 0 };
			concatw(src, L"C:/test/test16/", test_filenames[kk]);

			const wchar_t* dst = L"C:/test/unp";

			const wchar_t* packed_name = L"c:/test/packed.bin";

			uint64_t size_org = get_file_size_from_wname(src);
			printf("\n------------------------------------------------");
			wprintf(L"\n Packing... %s with length:%d", src, size_org);

			int cl = clock();

			block_pack(src, packed_name, profile);
			//seq_pack_separate(src, "c:/test/", profile);

			int pack_time = (clock() - cl);
			//printf("\n Packing finished time it took: %d", pack_time);
			uint64_t size_packed = get_file_size_from_wname(packed_name);
			printf("\n packed size %d", size_packed);
			wprintf(L"\n\n   --   RATIO OF PACKED   '%s'   %.2f%%   --\n\n", src, ((double)size_packed / (double)size_org) * 100.0);

			acc_size_packed += size_packed;
			if (isEarlyBreak(best_size, acc_size_packed, before_suite, timeLimit)) {
				earlyBreak = true;
				break;
			}
			acc_size_org += size_org;

			printf("\n Accumulated size %lu kb", acc_size_packed / 1024);

			cl = clock();
			if (unpack) {
				block_unpack(packed_name, dst);
				//seq_unpack_separate("main", dst, "c:/test/");

				int unpack_time = (clock() - cl);
				//printf("\n Unpacking finished time it took: %d", unpack_time);
				printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);

				printf("\n\n Comparing files!");
				if (files_equalw(src, dst)) {
					printf("\n ****** SUCCESS ****** (equal)\n");
				}
				else {
					exit(1);
				}
			}
	
			totalTime = clock() - before_suite;
			earlyBreak = isEarlyBreak(best_size, acc_size_packed, before_suite, timeLimit);
		}//end for
		best_size = presentResult(earlyBreak, totalTime, acc_size_packed, acc_size_org, best_size, profile, &bestProfile);
		fuzzProfile(&profile, bestProfile);
	}//end while true
}//end test suit 16

//-----------------------------------------------------------------------------------------

void testarchive() {
	
	packProfile bestProfile,
		profile = {
			.rle_ratio = 91,
			.twobyte_ratio = 94,
			.recursive_limit = 10,
			.twobyte_threshold_max = 11854,
			.twobyte_threshold_divide = 20,
			.twobyte_threshold_min = 906,
			.seqlenMinLimit3 = 161,
			.seqlenMinLimit4 = 51458,
			.blockSizeMinus = 109,
			.winsize = 18468, //104172
			.sizeMaxForCanonicalHeaderPack = 284,
			.sizeMinForSeqPack = 10877,
			.sizeMinForCanonical = 171,
			.sizeMaxForSuperslim = 10
	};
	copyProfile(&profile, &bestProfile);
	
    wchar_t* destDir = L"c:\\test\\archiveunp\\";
	wchar_t* source_dir = L"D:/Dropbox/Personal/Programmering/Compression/test/ws_todo";
    //wchar_t* source_dir = L"D:/Dropbox/Personal/Programmering/Compression/test/test13";

	unsigned long long best_size = 0;
	const wchar_t* packed_name = L"c:/test/packed.bin";
	while (true) {

		deleteAllFilesInDir(destDir);
		uint64_t acc_size_org = 0;

		uint64_t before_suite = clock();
		

		int cl = clock();
		archive_pack(source_dir, packed_name, profile);
		int pack_time = (clock() - cl);
		uint64_t acc_size_packed = get_file_size_from_wname(packed_name);

		printf("\n Accumulated size %lu kb", acc_size_packed / 1024);

		cl = clock();

		archive_unpack(packed_name, destDir);

		int unpack_time = (clock() - cl);
		uint64_t totalTime = clock() - before_suite;
		if (totalTime < (83 * 1000)) {
			printf("\n Comparing dirs!");
			if (dirs_equalw(source_dir, destDir)) {
				printf("\n ****** SUCCESS ****** (equal)\n");
			}
			else {
				printf("\n => dirs not equal, exiting!");
				exit(1);
			}


			best_size = presentResult(false, totalTime, acc_size_packed, acc_size_org, best_size, profile, &bestProfile);
		}
		else {
			printf("\n Too long time %d", totalTime);
			printf("\n size was %d", acc_size_packed);
			printProfile(&profile);
			printf("\n Current best profile");
			printf("\n --------------------");
			printProfile(&bestProfile);
		}
		fuzzProfile(&profile, bestProfile);
	}//end while true
}

//-------------------------------------------------------------------------------

int main()
{
	deleteAllFilesInDir(TEMP_DIRW);
	time_t t;
	srand((unsigned)time(&t));
	//testmeta();
    test16();
	//testarchive();
    //onefile();
}