#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"  
#include "file_tools.h"
#include "seq_packer.h"  
#include "multi_packer.h"
#include "block_packer.h"
#include <Windows.h>
#include "packer_commons.h"
#include "Two_byte_packer.h"
#include "RLE_packer_advanced.h"
#include "archive_packer.h"
#include "halfbyte_rle_packer.h"
#include "profileFactory.h"

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
	if (rand() % 4 == 0) {
		r = best;
	}
	if (r < 0) {
		r = min;
	}
	if (r < min) {
		r = min;
	}
	if (r > max) {
		r = max;
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

	profile->seqlenMinLimit3 = doFuzz(profile->seqlenMinLimit3, best.seqlenMinLimit3, 0, 255);
	profile->blockSizeMinus = doFuzz(profile->blockSizeMinus, best.blockSizeMinus, 0, 1000);
	profile->winsize = doFuzz(profile->winsize, best.winsize, 5000, 9000000);

	profile->sizeMaxForCanonicalHeaderPack = doFuzz(profile->sizeMaxForCanonicalHeaderPack, best.sizeMaxForCanonicalHeaderPack, 80, 1200);
	profile->sizeMinForCanonical = doFuzz(profile->sizeMinForCanonical, best.sizeMinForCanonical, 10, 700);
	profile->sizeMinForSeqPack = doFuzz(profile->sizeMinForSeqPack, best.sizeMinForSeqPack, 10, 93000);
	profile->sizeMaxForSuperslim = doFuzz(profile->sizeMaxForSuperslim, best.sizeMaxForSuperslim, 10, 100000);

	profile->archiveType = doFuzz(profile->archiveType, best.archiveType, 0, 2);

	profile->metaCompressionFactor = doFuzz(profile->metaCompressionFactor, best.metaCompressionFactor, 1, 200);

	profile->offsetLimit1 = doFuzz(profile->offsetLimit1, best.offsetLimit1, 90, 256);
	profile->offsetLimit2 = doFuzz(profile->offsetLimit2, best.offsetLimit2, 700, 2000);
	profile->offsetLimit3 = doFuzz(profile->offsetLimit3, best.offsetLimit3, 60000, 80000);

}


void fuzzCompleteProfile(completePackProfile* prof, completePackProfile best) {
	fuzzProfile(&(prof->main), best.main);
	fuzzProfile(&(prof->offset), best.offset);
	fuzzProfile(&(prof->seqlen), best.seqlen);
	fuzzProfile(&(prof->distance), best.distance);
}


unsigned long long presentResult(bool earlyBreak, uint64_t total_time, unsigned long long acc_size, unsigned long long acc_size_org,
	uint64_t best_size, packProfile profile, packProfile* best) {
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

bool isEarlyBreak(uint64_t best_size, uint64_t acc_size_packed, uint64_t totalTime, uint64_t seconds) {
	if ((best_size > 0 && acc_size_packed > best_size)) {
		printf("\n early break because of too big size.. size had become: %d", acc_size_packed);
		return true;
	}
	if (totalTime > (seconds * 1000)) {
		printf("\n early break because of too big time, time was %d", totalTime);
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
			long long size_org = getFileSizeByName(src);
			printf("\n Packing %s      length:%d", src, size_org);
			int cl = clock();
			//multi_pack(src, packed_name, seqlenProfile, seqlenProfile, offsetProfile, distanceProfile);

			long long size_packed = getFileSizeByName(packed_name);
			acc_size_packed += size_packed;
			if (isEarlyBreak(best_size, acc_size_packed, totalTime, timeLimit)) {
				earlyBreak = true;
				break;
			}
			acc_size_org += size_org;
			if (unpack) {
				//multi_unpack(packed_name, unpackedFilename);
				printf("\n\n Comparing files!");
				if (filesEqual(src, unpackedFilename)) {
					printf(" ****** SUCCESS ****** (equal)\n");
				}
				else {
					exit(1);
				}
			}


			concat(src, metaDir, "offsets");
			concat_int(src, src, kk + 101);
			size_org = getFileSizeByName(src);
			printf("\n Packing... %s with length:%d", src, size_org);

			//multi_pack(src, packed_name, offsetProfile, seqlenProfile, offsetProfile, distanceProfile);
			int pack_time = (clock() - cl);

			size_packed = getFileSizeByName(packed_name);
			acc_size_packed += size_packed;
			if (isEarlyBreak(best_size, acc_size_packed, before_suite, timeLimit)) {
				earlyBreak = true;
				break;
			}
			acc_size_org += size_org;
			if (unpack) {
				//multi_unpack(packed_name, unpackedFilename);
				printf("\n\n Comparing files!");
				if (filesEqual(src, unpackedFilename)) {
					printf(" ****** SUCCESS ****** (equal)\n");
				}
				else {
					exit(1);
				}
			}

			concat(src, metaDir, "distances");
			concat_int(src, src, kk + 101);
			size_org = getFileSizeByName(src);
			printf("\n Packing... %s with length:%d", src, size_org);

			//multi_pack(src, packed_name, distanceProfile, seqlenProfile, offsetProfile, distanceProfile);
			pack_time = (clock() - cl);

			size_packed = getFileSizeByName(packed_name);
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
				if (filesEqual(src, unpackedFilename)) {
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


void printResultToFile(uint64_t size, completePackProfile profile, wchar_t* ext) {
	FILE* utfil = openWrite(L"c:/test/best.txt");
	fprintf(utfil, "else if (equalsIgnoreCase(ext, L\"%ls\")) {", ext);
	fprintf(utfil, "\n\n//Size: %.3f", (float)(size)/1000.0);
	fprintf(utfil, "\n\npackProfile mainProfile = {");
	fprintProfile(utfil, &profile.main);
	fprintf(utfil, "},");
	fprintf(utfil, "\n seqlenProfile = {");
	fprintProfile(utfil, &profile.seqlen);
	fprintf(utfil, "},");
	fprintf(utfil, "\n offsetProfile = {");
	fprintProfile(utfil, &profile.offset);
	fprintf(utfil, "},");
	fprintf(utfil, "\n distanceProfile = {");
	fprintProfile(utfil, &profile.distance);
	fprintf(utfil, "};");
	fprintf(utfil, "\nreturn getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);");
	fprintf(utfil, "\n}");
	fclose(utfil);
}


//-------------------------------------------------------------------------------------------

void blockpack_onefile() {

	const wchar_t* ext = L"pdf";
	const wchar_t* dir = L"c:/test/blobs/";
	const wchar_t src[4096];
	concatw(src, dir, ext);

	int before_suite = clock();
	const wchar_t* unpacked = L"C:/test/unp";

	const wchar_t* packed = L"c:/test/packed.bin";

	uint64_t size_org = getFileSizeByName(src);

	printf("\n Packing... %ls with length:%llu", src, size_org);

	completePackProfile profile = getProfileForExtension(ext);
	completePackProfile bestProfile = cloneCompleteProfile(profile);

	uint64_t bestSize = UINT64_MAX;

	while (true) {

		int cl = clock();
		
		blockPackFull(src, packed, profile);
		int pack_time = (clock() - cl);


		uint64_t size_packed = getFileSizeByName(packed);


		printf("\n size_packed %llu", size_packed);


		cl = clock();
	/*
		block_unpack(packed, unpacked);
		int unpack_time = (clock() - cl);
		//printf("\n Unpacking finished time it took: %d", unpack_time);
		printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);

		wprintf(L"\n\n   --   RATIO OF PACKED   '%s'   %.2f%%   --\n\n", src, ((double)size_packed / (double)size_org) * 100.0);

		printf("\n\n Pack time %d", pack_time);
		printf("\n\n Unpack time %d", unpack_time);


		printf("\n\n Comparing files!");

		if (filesEqual(src, unpacked)) {
			printf("\n ****** SUCCESS ****** (equal)\n");
		}
		else {
			printf("\n Files are not equal!!");
			exit(1);
		}
		*/
		
		if (size_packed < bestSize) {
			bestSize = size_packed;
			bestProfile = cloneCompleteProfile(profile);			
			printf("\a");
			printResultToFile(size_packed, profile, ext);
		}
		fuzzCompleteProfile(&profile, bestProfile);		
	}//end while true

}//onefile

void onefile() {

	int before_suite = clock();

	const wchar_t* src =
		//L"D:/Dropbox/Misc/Blandat/Blandat misc/Documentary/Music/En händig man på turne.mpg";
		L"c:/test/pazera.exe";

	const wchar_t* unpackedFilename = L"C:/test/unp";

	const wchar_t* packed_name = L"c:/test/packed.bin";

	uint64_t size_org = getFileSizeByName(src);

	printf(L"\n Packing... %ls with length:%llu", src, size_org);

	int cl = clock();

	packProfile profile = {
			.rle_ratio = 98,
			.twobyte_ratio = 78,
			.recursive_limit = 239,
			.twobyte_threshold_max = 6465,
			.twobyte_threshold_divide = 3538,
			.twobyte_threshold_min = 974,
			.seqlenMinLimit3 = 196,
			.blockSizeMinus = 31,
			.winsize = 160000,
			.sizeMaxForCanonicalHeaderPack = 278,
			.sizeMinForSeqPack = 1890,
			.sizeMinForCanonical = 562,
			.sizeMaxForSuperslim = 47393,
			.archiveType = 1 // 0 solid, 1 semiseparate 2 separate
	};

	packProfile seqlenProfile = {
			.rle_ratio = 98,
			.twobyte_ratio = 78,
			.recursive_limit = 239,
			.twobyte_threshold_max = 6465,
			.twobyte_threshold_divide = 3538,
			.twobyte_threshold_min = 974,
			.seqlenMinLimit3 = 196,
			.blockSizeMinus = 31,
			.winsize = 160000,
			.sizeMaxForCanonicalHeaderPack = 278,
			.sizeMinForSeqPack = 1890,
			.sizeMinForCanonical = 562,
			.sizeMaxForSuperslim = 47393,
			.archiveType = 1 // 0 solid, 1 semiseparate 2 separate
	},

		offsetProfile = {
			.rle_ratio = 98,
				.twobyte_ratio = 78,
				.recursive_limit = 239,
				.twobyte_threshold_max = 6465,
				.twobyte_threshold_divide = 3538,
				.twobyte_threshold_min = 974,
				.seqlenMinLimit3 = 196,
				.blockSizeMinus = 31,
				.winsize = 160000,
				.sizeMaxForCanonicalHeaderPack = 278,
				.sizeMinForSeqPack = 1890,
				.sizeMinForCanonical = 562,
				.sizeMaxForSuperslim = 47393,
				.archiveType = 1 // 0 solid, 1 semiseparate 2 separate
	},

		distanceProfile = {
	.rle_ratio = 98,
			.twobyte_ratio = 78,
			.recursive_limit = 239,
			.twobyte_threshold_max = 6465,
			.twobyte_threshold_divide = 3538,
			.twobyte_threshold_min = 974,
			.seqlenMinLimit3 = 196,
			.blockSizeMinus = 31,
			.winsize = 160000,
			.sizeMaxForCanonicalHeaderPack = 278,
			.sizeMinForSeqPack = 1890,
			.sizeMinForCanonical = 562,
			.sizeMaxForSuperslim = 47393,
			.archiveType = 1 // 0 solid, 1 semiseparate 2 separate'
	};

	uint64_t size_packed;
	memfile* srcm = getMemfileFromFile(src);

	seqPackBundle packed = seqPackSep(srcm, profile);
	size_packed = getBundleSize(packed);
	
	//memfile* packed = multiPack2(srcm, profile, seqlenProfile, offsetProfile, distanceProfile);
	//memfile* packed = halfbyteRlePack(srcm, 0);
	//memfile* packed = canonicalEncode(srcm);
	//memfile* packed = twoBytePack(srcm, profile);
	//size_packed = getMemSize(packed);

	printf("\n size_packed %llu", size_packed);
	//memfileToFile(packed.main, L"c:/test/packed.bin");
	int pack_time = (clock() - cl);

	cl = clock();

	//memfile* unpacked = twoByteUnpack(packed);
	//memfile* unpacked = canonicalDecode(packed);
	//memfile* unpacked = multiUnpack(packed);
	memfile* unpacked = seqUnpack(packed);
	//memfile* unpacked = halfbyteRleUnpack(packed, 0);


	int unpack_time = (clock() - cl);
	//printf("\n Unpacking finished time it took: %d", unpack_time);
	printf("\nTimes %d/%d/%d", pack_time, unpack_time, pack_time + unpack_time);

	wprintf(L"\n\n   --   RATIO OF PACKED   '%s'   %.2f%%   --\n\n", src, ((double)size_packed / (double)size_org) * 100.0);

	printf("\n\n Pack time %d", pack_time);
	printf("\n\n Unpack time %d", unpack_time);

	memfileToFile(unpacked, unpackedFilename);
	printf("\n\n Comparing files!");

	if (filesEqual(src, unpackedFilename)) {
		printf("\n ****** SUCCESS ****** (equal)\n");
	}
	else {
		printf("\n Files are not equal!!");
		exit(1);
	}
}//onefile



void testarchive() {

	completePackProfile profile = getProfileForExtension(L"gjkrigjrigj");
	completePackProfile bestProfile = cloneCompleteProfile(profile);
	uint64_t time_limit = 1000;
	

	wchar_t* destDir = L"c:\\test\\archiveunp\\";

	wchar_t* source_dir =
		 L"D:/Dropbox/Personal/Programmering/Compression/test/ws_todo";
		//L"D:/Dropbox/Misc/Download";
		//L"c:/test/testallequal";
		//L"c:/test/all";
	// L"c:/test/test13";
	//L"c:/test/47";

	uint64_t bestSize = UINT64_MAX;
	const wchar_t* packed_name = L"c:/test/packed.bin";
	while (true) {

		deleteAllFilesInDir(destDir);
		uint64_t acc_size_org = 0;

		uint64_t before_suite = clock();

		int cl = clock();
		archive_pack(source_dir, packed_name, profile);
		int pack_time = (clock() - cl);
		uint64_t acc_size_packed = getFileSizeByName(packed_name);

		printf("\n Accumulated size %lu kb", (int)(acc_size_packed / 1024));

		cl = clock();

		archive_unpack(packed_name, destDir);

		uint64_t totalTime = clock() - before_suite;
		if (totalTime < (time_limit * 1000)) {
			printf("\n Comparing dirs!");
			if (dirsEqual(source_dir, destDir)) {
				printf("\n ****** SUCCESS ****** (equal)\n");
			}
			else {
				printf("\n => dirs not equal, exiting!");
				exit(1);
			}
			if (acc_size_packed < bestSize) {
				bestSize = acc_size_packed;
				bestProfile = cloneCompleteProfile(profile);
				printf("\a");
				printResultToFile(acc_size_packed, profile, L"_default_");
			}
		}
		else {
			printf("\n Too long time %llu", totalTime);
			printf("\n size was %llu", acc_size_packed);
		}
		fuzzCompleteProfile(&profile, bestProfile);
		printf("\n\n Now doing a new try with this new profile: ");
		printProfile(&profile);
		printf("\n\n");
	}//end while true
}

//-------------------------------------------------------------------------------

int main()
{
	deleteAllFilesInDir(TEMP_DIRW);
	time_t t;
	srand((unsigned)time(&t));
	//testmeta();
	//test16();
	//onefile();
    testarchive();
    blockpack_onefile();
}