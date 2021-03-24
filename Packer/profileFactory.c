#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <tchar.h> 
#include <strsafe.h>
#include <string.h>
#include <conio.h>
#include <process.h>
#include <inttypes.h>

#include "common_tools.h"
#include "packer_commons.h"
#include "file_tools.h"
#include "profileFactory.h"



__declspec(thread) static char* strtokPointer;


void removeLineFeeds(char* line) {
	if (line[strlen(line) - 1] == '\n') {
		line[strlen(line) - 1] = 0;
	}
	if (line[strlen(line) - 1] == '\r') {
		line[strlen(line) - 1] = 0;
	}
}

int64_t getCommaSeparatedInt(char* line) {
	if (strtokPointer == NULL) {
		strtokPointer = strtok(line, ",");
	}
	else {
		strtokPointer = strtok(NULL, ",");
	}
	int64_t res = 0;
	if (strtokPointer != NULL) {
		res = atoi(strtokPointer);
		printf("\n ¤¤¤¤¤ %lld", res);
	}
	return res;
}

packProfile readProfileFromLine(FILE* file) {
	char line[10000];
	fgets(line, sizeof(line), file);
	removeLineFeeds(line);
	strtokPointer = NULL;

	packProfile p;
	p.rle_ratio = getCommaSeparatedInt(line);
	p.twobyte_ratio = getCommaSeparatedInt(line);
	p.recursive_limit = getCommaSeparatedInt(line);
	p.twobyte_threshold_max = getCommaSeparatedInt(line);
	p.twobyte_threshold_divide = getCommaSeparatedInt(line);
	p.twobyte_threshold_min = getCommaSeparatedInt(line);
	p.superSlimSeqlenMinLimit3 = getCommaSeparatedInt(line);
	p.seqlenMinLimit3 = getCommaSeparatedInt(line);
	p.seqlenMinLimit4 = getCommaSeparatedInt(line);
	p.blockSizeMinus = getCommaSeparatedInt(line);
	p.sizeMaxForCanonicalHeaderPack = getCommaSeparatedInt(line);
	p.sizeMinForSeqPack = getCommaSeparatedInt(line);
	p.sizeMinForCanonical = getCommaSeparatedInt(line);
	p.sizeMaxForSuperslim = getCommaSeparatedInt(line);
	p.metaCompressionFactor = getCommaSeparatedInt(line);
	p.offsetLimit1 = getCommaSeparatedInt(line);
	p.offsetLimit2 = getCommaSeparatedInt(line);
	p.offsetLimit3 = getCommaSeparatedInt(line);
	p.bytesWonMin = getCommaSeparatedInt(line);
	return p;
}


void removeExtensionFromFile(wchar_t* ext) {
	FILE* file = openRead(L"c:/test/best.txt");
	FILE* fileOut = openWrite(L"c:/test/best2.txt");
	char line[10000];
	char outbuf[1000];

	while (fgets(line, sizeof(line), file)) {
		/* note that fgets don't strip the terminating \n, checking its
		   presence would allow to handle lines longer that sizeof(line) */
		printf(">>>> %s", line);
		removeLineFeeds(line);
		if (strlen(line) == 0) {
			continue;
		}
		if (line[0] == '#') {
			char* thisExt = line + 1;
			printf("\n thisExt=%s", thisExt);
			if (equalsNormalAndUni(thisExt, ext)) {

				// skip this extension!
				fgets(line, sizeof(line), file);			
				fgets(line, sizeof(line), file);
				fgets(line, sizeof(line), file);
				fgets(line, sizeof(line), file);
				fgets(line, sizeof(line), file);
			}
			else {
				// just copy this extension normally!

				//copy extension line   #xmi
				fputs(line, fileOut);
				fputs("\n", fileOut);

				//copy size line  
				fgets(line, sizeof(line), file);
				fputs(line, fileOut);

				fgets(line, sizeof(line), file);
				fputs(line, fileOut);

				fgets(line, sizeof(line), file);
				fputs(line, fileOut);

				fgets(line, sizeof(line), file);
				fputs(line, fileOut);

				fgets(line, sizeof(line), file);
				fputs(line, fileOut);
			}
		}
	}
	fclose(file);
	fclose(fileOut);
	_wremove(L"c:/test/best.txt");
	myRename(L"c:/test/best2.txt", L"c:/test/best.txt");
}


void updateExtensionInFile(uint64_t size, completePackProfile profile, wchar_t* ext) {
	removeExtensionFromFile(ext);
	FILE* utfil = _wfopen(L"c:/test/best.txt", L"a");
	fprintf(utfil, "\n#%ls", ext);
	fprintf(utfil, "\n%llu", size);
	fprintProfile2(utfil, &profile.main);
	fprintProfile2(utfil, &profile.seqlen);
	fprintProfile2(utfil, &profile.offset);
	fprintProfile2(utfil, &profile.distance);
	fclose(utfil);
}


completePackProfile getProfileForExtensionOrDefault(wchar_t* ext, completePackProfile def) {

	FILE* file = openRead(L"c:/test/best.txt");
	char line[10000];
	char thisExt[100];
	completePackProfile res;
	while (fgets(line, sizeof(line), file)) {
		/* note that fgets don't strip the terminating \n, checking its
		   presence would allow to handle lines longer that sizeof(line) */
		printf(">>>> %s", line);
		removeLineFeeds(line);
		if (strlen(line) == 0) {
			continue;
		}

		if (line[0] == '#') {
			strcpy(thisExt, line + 1);
			printf("\n thisExt=%s", thisExt);

			// read the profiles
			fgets(line, sizeof(line), file);
			removeLineFeeds(line);

			res.size = atoi(line);

			res.main = readProfileFromLine(file);
			res.seqlen = readProfileFromLine(file);
			res.offset = readProfileFromLine(file);
			res.distance = readProfileFromLine(file);

			printf("\n comparing to %ls", ext);
			if (equalsNormalAndUni(thisExt, ext)) {
				fclose(file);
				printf("\n Found match!!");

				int64_t seqlenMinLimit4 = res.main.seqlenMinLimit4;
				int64_t seqlenMinLimit3 = res.main.seqlenMinLimit3;
				int64_t superSlimSeqlenMinLimit3 = res.main.superSlimSeqlenMinLimit3;

				res.seqlen.seqlenMinLimit3 = seqlenMinLimit3;
				res.seqlen.seqlenMinLimit4 = seqlenMinLimit4;
				res.seqlen.superSlimSeqlenMinLimit3 = superSlimSeqlenMinLimit3;

				res.offset.seqlenMinLimit3 = seqlenMinLimit3;
				res.offset.seqlenMinLimit4 = seqlenMinLimit4;
				res.offset.superSlimSeqlenMinLimit3 = superSlimSeqlenMinLimit3;

				res.distance.seqlenMinLimit3 = seqlenMinLimit3;
				res.distance.seqlenMinLimit4 = seqlenMinLimit4;
				res.distance.superSlimSeqlenMinLimit3 = superSlimSeqlenMinLimit3;

				fixPackProfile(&(res.main));
				fixPackProfile(&(res.seqlen));
				fixPackProfile(&(res.offset));
				fixPackProfile(&(res.distance));

				return res;
			}
		}
	}
	fclose(file);

	// -------------------------------------------------------------------------
	// Default profile for the rest
	// -------------------------------------------------------------------------

	int64_t seqlenMinLimit3 = def.main.seqlenMinLimit3;
	int64_t seqlenMinLimit4 = def.main.seqlenMinLimit4;
	int64_t superSlimSeqlenMinLimit3 = def.main.superSlimSeqlenMinLimit3;
	
	def.seqlen.seqlenMinLimit3 = seqlenMinLimit3;
	def.seqlen.seqlenMinLimit4 = seqlenMinLimit4;
	def.seqlen.superSlimSeqlenMinLimit3 = superSlimSeqlenMinLimit3;

	def.offset.seqlenMinLimit3 = seqlenMinLimit3;
	def.offset.seqlenMinLimit4 = seqlenMinLimit4;
	def.offset.superSlimSeqlenMinLimit3 = superSlimSeqlenMinLimit3;

	def.distance.seqlenMinLimit3 = seqlenMinLimit3;
	def.distance.seqlenMinLimit4 = seqlenMinLimit4;
	def.distance.superSlimSeqlenMinLimit3 = superSlimSeqlenMinLimit3;

	fixPackProfile(&def.main);
	fixPackProfile(&def.seqlen);
	fixPackProfile(&def.offset);
	fixPackProfile(&def.distance);
	
	def.size = 0;
	return def;
}


fixPackProfile(packProfile* prof) {
	if (prof->seqlenMinLimit4 <= prof->seqlenMinLimit3) {
		prof->seqlenMinLimit4 = 65000;
	}
	if (prof->superSlimSeqlenMinLimit3 == 0) {
		prof->superSlimSeqlenMinLimit3 = 111;
	}
}



completePackProfile getProfileForExtension(wchar_t* ext) {

	packProfile mainProfile = {
.rle_ratio = 97,
.twobyte_ratio = 87,
.recursive_limit = 234,
.twobyte_threshold_max = 5292,
.twobyte_threshold_divide = 2642,
.twobyte_threshold_min = 739,
.seqlenMinLimit3 = 213,
.seqlenMinLimit4 = 65360,
.blockSizeMinus = 0,
.sizeMaxForCanonicalHeaderPack = 83,
.sizeMinForSeqPack = 3744,
.sizeMinForCanonical = 594,
.sizeMaxForSuperslim = 70177,
.metaCompressionFactor = 75,
.offsetLimit1 = 253,
.offsetLimit2 = 1101,
.offsetLimit3 = 65340,
.bytesWonMin = -11
	},
		seqlenProfile = {
	   .rle_ratio = 39,
	   .twobyte_ratio = 96,
	   .recursive_limit = 48,
	   .twobyte_threshold_max = 3860,
	   .twobyte_threshold_divide = 2041,
	   .twobyte_threshold_min = 44,
	   .seqlenMinLimit3 = 214,
	   .seqlenMinLimit4 = 65360,
	   .blockSizeMinus = 53,
	   .sizeMaxForCanonicalHeaderPack = 174,
	   .sizeMinForSeqPack = 10,
	   .sizeMinForCanonical = 137,
	   .sizeMaxForSuperslim = 27490,
	   .metaCompressionFactor = 56,
	   .offsetLimit1 = 247,
	   .offsetLimit2 = 1119,
	   .offsetLimit3 = 64743,
	   .bytesWonMin = 75
	},
		offsetProfile = {
	   .rle_ratio = 83,
	   .twobyte_ratio = 83,
	   .recursive_limit = 20,
	   .twobyte_threshold_max = 9024,
	   .twobyte_threshold_divide = 2657,
	   .twobyte_threshold_min = 337,
	   .seqlenMinLimit3 = 223,
	   .seqlenMinLimit4 = 65360,
	   .blockSizeMinus = 38,
	   .sizeMaxForCanonicalHeaderPack = 534,
	   .sizeMinForSeqPack = 10,
	   .sizeMinForCanonical = 158,
	   .sizeMaxForSuperslim = 14120,
	   .metaCompressionFactor = 52,
	   .offsetLimit1 = 256,
	   .offsetLimit2 = 929,
	   .offsetLimit3 = 64876,
	   .bytesWonMin = 0
	},
		distanceProfile = {
	   .rle_ratio = 70,
	   .twobyte_ratio = 86,
	   .recursive_limit = 20,
	   .twobyte_threshold_max = 4946,
	   .twobyte_threshold_divide = 3766,
	   .twobyte_threshold_min = 69,
	   .seqlenMinLimit3 = 204,
	   .seqlenMinLimit4 = 65360,
	   .blockSizeMinus = 0,
	   .sizeMaxForCanonicalHeaderPack = 588,
	   .sizeMinForSeqPack = 10,
	   .sizeMinForCanonical = 384,
	   .sizeMaxForSuperslim = 321,
	   .metaCompressionFactor = 59,
	   .offsetLimit1 = 256,
	   .offsetLimit2 = 1101,
	   .offsetLimit3 = 68153,
	   .bytesWonMin = -20
	};


	completePackProfile prof = getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);

	completePackProfile def = getProfileForExtensionOrDefault(ext, prof);

	int64_t seqlenMinLimit3 = def.main.seqlenMinLimit3;
	int64_t seqlenMinLimit4 = def.main.seqlenMinLimit4;
	int64_t superSlimSeqlenMinLimit3 = def.main.superSlimSeqlenMinLimit3;

	def.seqlen.seqlenMinLimit3 = seqlenMinLimit3;
	def.seqlen.seqlenMinLimit4 = seqlenMinLimit4;
	def.seqlen.superSlimSeqlenMinLimit3 = superSlimSeqlenMinLimit3;

	def.offset.seqlenMinLimit3 = seqlenMinLimit3;
	def.offset.seqlenMinLimit4 = seqlenMinLimit4;
	def.offset.superSlimSeqlenMinLimit3 = superSlimSeqlenMinLimit3;

	def.distance.seqlenMinLimit3 = seqlenMinLimit3;
	def.distance.seqlenMinLimit4 = seqlenMinLimit4;
	def.distance.superSlimSeqlenMinLimit3 = superSlimSeqlenMinLimit3;

	fixPackProfile(&def.main);
	fixPackProfile(&def.seqlen);
	fixPackProfile(&def.offset);
	fixPackProfile(&def.distance);

	return def;
}