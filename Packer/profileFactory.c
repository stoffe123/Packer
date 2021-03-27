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


#include <direct.h>
#define GetCurrentDir _getcwd

__declspec(thread) static char* strtokPointer;

__declspec(thread) static wchar_t* packProfileFilename = L"";

	const wchar_t* getPackProfilesFile() {
			if (equalsw(L"", packProfileFilename)) {
				char cwd[2000];
				if (GetCurrentDir(cwd, 2000)) {
					concat(cwd, cwd, "/../packprofiles.txt");
					packProfileFilename = calloc(strlen(cwd) + 1, sizeof(wchar_t));
					toUni(packProfileFilename, cwd);
				}
				return packProfileFilename;
			}
	}

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
			//printf("\n ¤¤¤¤¤ %lld", res);
		}
		return res;
	}

	packProfile readProfileFromLine(FILE * file) {
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
		FILE* file = openRead(getPackProfilesFile());
		
		wchar_t tempFilename[300];
		get_temp_filew(tempFilename, L"temppackprofiles");
		
		FILE* fileOut = openWrite(tempFilename);
		char line[10000];
		char outbuf[1000];

		while (fgets(line, sizeof(line), file)) {
			/* note that fgets don't strip the terminating \n, checking its
			   presence would allow to handle lines longer that sizeof(line) */
			//printf(">>>> %s", line);
			removeLineFeeds(line);
			if (strlen(line) == 0) {
				continue;
			}
			if (line[0] == '#') {
				char* thisExt = line + 1;
				//printf("\n thisExt=%s", thisExt);
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
		myRename(tempFilename, getPackProfilesFile());
	}


	void updateExtensionInFile(uint64_t size, completePackProfile profile, wchar_t* ext) {
		removeExtensionFromFile(ext);
		FILE* utfil = _wfopen(getPackProfilesFile(), L"a");
		fprintf(utfil, "\n#%ls", ext);
		fprintf(utfil, "\n%llu", size);
		fprintProfile2(utfil, &profile.main);
		fprintProfile2(utfil, &profile.seqlen);
		fprintProfile2(utfil, &profile.offset);
		fprintProfile2(utfil, &profile.distance);
		fclose(utfil);
	}


	completePackProfile getProfileForExtensionOrDefault(wchar_t* ext, completePackProfile def) {

		FILE* file = openRead(getPackProfilesFile());
		char line[10000];
		char thisExt[100];
		completePackProfile res;
		while (fgets(line, sizeof(line), file)) {
			/* note that fgets don't strip the terminating \n, checking its
			   presence would allow to handle lines longer that sizeof(line) */
			//printf(">>>> %s", line);
			removeLineFeeds(line);
			if (strlen(line) == 0) {
				continue;
			}

			if (line[0] == '#') {
				strcpy(thisExt, line + 1);
				//printf("\n thisExt=%s", thisExt);

				// read the profiles
				fgets(line, sizeof(line), file);
				removeLineFeeds(line);

				res.size = atoi(line);

				res.main = readProfileFromLine(file);
				res.seqlen = readProfileFromLine(file);
				res.offset = readProfileFromLine(file);
				res.distance = readProfileFromLine(file);

				//printf("\n comparing to %ls", ext);
				if (equalsNormalAndUni(thisExt, ext)) {
					fclose(file);
					//printf("\n Found match!!");



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

		if (def.size == UINT64_MAX) {
			def = getProfileForExtension(L"__default__");
		}




		fixPackProfile(&def.main);
		fixPackProfile(&def.seqlen);
		fixPackProfile(&def.offset);
		fixPackProfile(&def.distance);


		def.size = 0;
		return def;
	}


	fixPackProfile(packProfile * prof) {
		if (prof->seqlenMinLimit4 <= prof->seqlenMinLimit3) {
			prof->seqlenMinLimit4 = 65000;
		}
		if (prof->superSlimSeqlenMinLimit3 == 0) {
			prof->superSlimSeqlenMinLimit3 = 111;
		}
	}



	completePackProfile getProfileForExtension(wchar_t* ext) {

		completePackProfile def;
		def.size = UINT64_MAX;   // flag that it is empty

		completePackProfile res = getProfileForExtensionOrDefault(ext, def);
		return res;
	}