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
#include "multi_packer.h"
#include "packer_commons.h"
#include "block_packer.h"
#include "file_tools.h"
#include "block_packer.h"
#include "archive_packer.h"
#include "profileFactory.h"
#include "memfile.h""


// for semiseparated threaded pack
typedef struct blob_t {
	wchar_t filename[500];
	completePackProfile profile;
	uint64_t singleFileIndex;
	HANDLE  handle;
} blob_t;

typedef struct wchar100_t {
	wchar_t s[100];
} wchar100_t;

typedef struct blobsInfo_t {
	wchar100_t* extList;
	uint64_t* sizesList;
	uint64_t count;
} blobsInfo_t;

__declspec(thread) static blob_t blobs[5000];

__declspec(thread) static HANDLE blobMutex;

HANDLE lockBlobMutex() {
	WaitForSingleObject(blobMutex, INFINITE);
}

HANDLE releaseBlobMutex() {
	ReleaseMutex(blobMutex);
}

completePackProfile getHeaderSizesProfile() {
	return getProfileForExtension(L"__headersizes__");
}

completePackProfile getHeaderNamesProfile() {
	return getProfileForExtension(L"__headernames__");
}

void threadBlockPack(void* arg)
{
	lockBlobMutex();
	blob_t* bc = (blob_t*)arg;
	completePackProfile profile = bc->profile;
	wchar_t* filename = bc->filename;	
	releaseBlobMutex();

	printf("\n Starting THREAD blockpack for file %ls", filename);
	blockPackAndReplaceFull(filename, profile);	
	printf("\n THREAD blockpack FOR %ls FINISHED!", filename);
}

void threadBlockUnPack(void* arg)
{
	lockBlobMutex();
	blob_t* bc = (blob_t*)arg;
	wchar_t* filename = bc->filename;
	releaseBlobMutex();

	printf("\n Starting THREAD block unpack for file %ls", filename);
	blockUnpackAndReplace(filename, bc->profile);
	printf("\n THREAD block unpack FOR %ls FINISHED!", filename);
}

#pragma comment(lib, "User32.lib")

#define EQUALFILE_POINTER_MAX  7935

uint64_t wcharToMyCoding(wchar_t* wcharBuf, uint8_t* codedBuf) {
	uint64_t wcharBufPos = 0;
	uint64_t codedBufPos = 0;
	wchar_t ch;
	while ((ch = wcharBuf[wcharBufPos++]) != 0) {
		if (ch <= 127) {
			codedBuf[codedBufPos++] = (uint8_t)ch;
		}
		else {
			codedBuf[codedBufPos++] = 128;
			codedBuf[codedBufPos++] = (ch / 256);
			codedBuf[codedBufPos++] = (ch % 256);
			//TODO wchar_t could be 32-bit
		}
	}
	codedBuf[codedBufPos] = 0;
	return codedBufPos;
}

uint64_t myCodingToWchar(FILE* in, wchar_t* wcharBuf) {
	uint64_t wcharBufPos = 0;
    uint64_t codedBufPos = 0;
	int ch;
	while (true) {
		ch = fgetc(in);
		if (ch == EOF || ch <= 31) {
			break;
		}
		if (ch <= 127) {
			wcharBuf[wcharBufPos++] = (wchar_t)ch;
		}
		else {
			int msb = fgetc(in);
			int lsb = fgetc(in);
			wcharBuf[wcharBufPos++] = (wchar_t)(msb * 256 + lsb);
		}
	}
	wcharBuf[wcharBufPos++] = 0;
	if (ch > 0 && ch != EOF) {
		uint8_t lastValue =  fgetc(in);
		return (((uint64_t)ch - 1) * 256 + lastValue);
	}
	return UINT64_MAX;
}

void diffSizes(fileListAndCount_t dirInfo) {
	file_t* fileList = dirInfo.fileList;
	uint64_t last = 0;
	for (int i = 0; i < dirInfo.count; i++) {
		uint64_t size = fileList[i].size;
		uint64_t diff = size - last;
		printf("\n %ls %lld %lld", fileList[i].name, size, diff);
		last = size;
		fileList[i].size = diff;
	}
}

void unDiffSizes(fileListAndCount_t dirInfo) {
	file_t* fileList = dirInfo.fileList;
	uint64_t last = 0;
	for (int i = 0; i < dirInfo.count; i++) {
		fileList[i].size += last;
		last = fileList[i].size;		
	}
}

memfile* createSizesHeader(wchar_t* dir, fileListAndCount_t dirInfo) 
{ 
	file_t* fileList = dirInfo.fileList;
	memfile* out = getEmptyMem(L"archivepacker_createsizes");
	//write sizes

	for (int k = 0; k < 8; k++) {

		for (int i = 0; i < dirInfo.count; i++) {
			//todo write MSB together and so on...
			//uint64_t size = fileList[i].size;
			uint8_t byte = getByteAtPos(fileList[i].size, k);
			memWrite(&byte, 1, out);
			//printf("\n writing size for %d as %d", i, size);
		}
	}
	return out;
}


uint64_t findEqualFileIndex(fileListAndCount_t dirInfo, uint64_t index) {
	uint64_t count = dirInfo.count;
	file_t* fileList = dirInfo.fileList;

	if (index == 0 || index >= count) {
		//no luck
		return UINT64_MAX;
	}

	for (int i = index - 1; i >= 0; i--) {
		if (index - i > EQUALFILE_POINTER_MAX) {
			break;
		}
		if (filesEqual(fileList[i].name, fileList[index].name)) {
			return index - i;
		}
	}
	return UINT64_MAX;
}

void identifyEqualFiles(wchar_t* dir, fileListAndCount_t dirInfo) {
	file_t* fileList = dirInfo.fileList;
	uint64_t count = dirInfo.count;
			
	printf("\n ********* identifyEqualFiles **********\n");
	for (int i = 0; i < count; i++) {
		printf("\nIdentifyEqualFiles: %ls  ,  %llu\n", fileList[i].name, fileList[i].size);

		uint64_t index = findEqualFileIndex(dirInfo, i);
		if (index <= EQUALFILE_POINTER_MAX) {			
			fileList[i].equalSizeNumber = index;
		}
		else {
			fileList[i].equalSizeNumber = UINT64_MAX;			
		}
	}
}

memfile* createNamesHeader(wchar_t* dir, fileListAndCount_t dirInfo) {	

	file_t* fileList = dirInfo.fileList;
	uint64_t count = dirInfo.count;
	memfile* out = getEmptyMem(L"archivepacker_names_header_mem");
	//write names separated by 0
	int lengthOfDirName = wcslen(dir);
	uint8_t multiByteStr[2000] = { 0 };
	printf("\n ********* Storing dir names **********\n");
	for (int i = 0; i < count; i++) {
		printf("\nCreateNamesHeader: %ls  ,  %llu\n", fileList[i].name, fileList[i].size);
		

		int j = lengthOfDirName + 1;

		wchar_t cc = fileList[i].name[j];
	
		//TODO write to "out" mem directly
		uint64_t size = wcharToMyCoding(&(fileList[i].name[j]), &multiByteStr);

		// the last byte of the name is special
	    // 0 = normal end
	    // 1-31  means a pointer to an equal file follows on the next byte
	    // 1 = 0-255 , 2 = 256-511 , 3 = 512-767  etc
		uint64_t index = fileList[i].equalSizeNumber;

		if (index < UINT64_MAX) {
			//TODO support for higher indexes by paging
			printf("\n FOUND EQUAL FILE NR %llu", index);
			multiByteStr[size++] = (uint8_t)(index / 256 + 1);
			multiByteStr[size++] = (uint8_t)(index % 256);		
		}
		else {			
			if (i < (count - 1)) {
				size++;
			}
		}
		memWrite(&multiByteStr, size, out);		
	}
	//memfileToFile(out, L"c:/test/memmm"); myExit();
	return out;
}

void tmpDirNameOf(const wchar_t* tmpBlocked, const wchar_t* name, const wchar_t* dir) {
	wchar_t* onlyName = name + wcslen(dir);
	concatw(tmpBlocked, TEMP_DIRW, onlyName);
	createMissingDirs(tmpBlocked, TEMP_DIRW);
}

memfile* createPackedSizesHeader(wchar_t* dir, fileListAndCount_t dirInfo) {
	
	memfile* sizesHeader = createSizesHeader(dir, dirInfo);

	completePackProfile comp = getHeaderSizesProfile();

	//memfileToFile(sizesHeader, L"c:/test/blobs/__headersizes__");

	return multiPackAndStorePackType(sizesHeader, comp);
}

memfile* createPackedNamesHeader(wchar_t* dir, fileListAndCount_t dirInfo) {
		
	memfile* namesHeader = createNamesHeader(dir, dirInfo);
	//memfileToFile(namesHeader, L"c:/test/namesheaders/namesHeader.bin");
	//myExit();

	completePackProfile comp = getHeaderNamesProfile();
	
	//memfileToFile(namesHeader, L"c:/test/blobs/__headernames__");
	
	return multiPackAndStorePackType(namesHeader, comp);
}

void writeArchiveHeader(FILE* out, fileListAndCount_t dirInfo, wchar_t* dir) {

	memfile* sizesHeader = createPackedSizesHeader(dir, dirInfo);
	memfile* namesHeader = createPackedNamesHeader(dir, dirInfo);	

	uint64_t headerNamesPackedSize = getMemSize(namesHeader)
		, headerSizesPackedSize = getMemSize(sizesHeader);
	printf("\n Archive header sizes packed to %lld", headerSizesPackedSize);		
	printf("\n Archive header names packed to %lld", headerNamesPackedSize);
	printf("\n Total packed header size is  %lld", headerNamesPackedSize + headerSizesPackedSize);

	writeDynamicSize16or64(headerSizesPackedSize, out);
	writeDynamicSize16or64(headerNamesPackedSize, out);
		
	append_mem_to_file(out, sizesHeader);
	append_mem_to_file(out, namesHeader);
}

blobsInfo_t fetchBlobsInfo(fileListAndCount_t dirInfo) {

	file_t* fileList = dirInfo.fileList;
	uint64_t count = dirInfo.count;
	uint64_t blobCount = 0;

	uint64_t* res = calloc(count, sizeof(uint64_t));
	wchar100_t* extList = calloc(count, sizeof(wchar100_t));
	if (res == NULL || extList == NULL) {
		printf("\n Out of memory in archive_packer.determineNumberOfBlobs");
		myExit();
		return;
	}
	wchar_t ext[20] = { 0 };
	wchar_t next_ext[20] = { 0 };
	int noOfFilesInBlobCount = 0;

	bool breakAndCreateBlob;
	for (int i = 0; i < count; i++) {
		breakAndCreateBlob = false;
		if (fileList[i].equalSizeNumber == UINT64_MAX && i < count - 1) {
			wchar_t* filename = fileList[i].name;
			//printf("\n%ls of size: %lld", filename, fileList[i].size);
			getFileExtension(ext, filename);
			getFileExtension(next_ext, fileList[i + 1].name);
			breakAndCreateBlob = !equalsIgnoreCase(next_ext, ext);
			if (!breakAndCreateBlob) {
				noOfFilesInBlobCount++;
			}
		}
		if (breakAndCreateBlob || (i == count - 1)) {
			
			wcscpy(extList[blobCount].s, (i == count - 1) ? next_ext : ext);			
			printf("\n BLOB nr %llu FOUND NOoFfILES=%d ext=%ls", blobCount, noOfFilesInBlobCount, extList[blobCount].s);

			if (noOfFilesInBlobCount == 0) {
				res[blobCount] = i;
			}
			else {
				res[blobCount] = UINT64_MAX;
			}

			blobCount++;

			noOfFilesInBlobCount = 0;
		}
	}
	blobsInfo_t blobsInfo;
	blobsInfo.count = blobCount;
	blobsInfo.sizesList = res;
	blobsInfo.extList = extList;
	return blobsInfo;
}

void getNewBlobFilename(wchar_t* blobFilename, wchar_t* name, wchar_t* ext) {
	wcscpy(name, L"archivepacker_semisepblob_");
	//wcscat(name, ext);   // an extension can contain a slash and that makes havoc !! 
	get_temp_filew(blobFilename, name);
}

void archivePackSemiSeparated(wchar_t* dest, completePackProfile profile, fileListAndCount_t dirInfo, wchar_t* dir) {

	file_t* fileList = dirInfo.fileList;
	uint64_t count = dirInfo.count;
	printf("\n Start archive pack semiseparated case");
	// write sizes header	
	wchar_t ext[50] = { 0 };
	wchar_t next_ext[50] = { 0 };

	const wchar_t blobFilename[200] = { 0 };
	const wchar_t name[200] = { 0 };
	int noOfFilesInBlobCount = 0;

	uint64_t blobCount = 0;
	bool breakAndCreateBlob;
	FILE* blobFile = NULL;
	for (uint64_t i = 0; i < count; i++) {
		breakAndCreateBlob = false;
				
		if (fileList[i].equalSizeNumber == UINT64_MAX) {
			wchar_t* filename = fileList[i].name;
			getFileExtension(ext, filename);
			if (i == 0) {
				getNewBlobFilename(blobFilename, name, ext);
				blobFile = openWrite(blobFilename);
			}
			printf("\n%ls of size: %lld", filename, fileList[i].size);
			appendFileToFile(blobFile, filename);
			if (i < count - 1) {
				getFileExtension(next_ext, fileList[i + 1].name);
				breakAndCreateBlob = !equalsIgnoreCase(next_ext, ext);
				if (!breakAndCreateBlob) {
					noOfFilesInBlobCount++;
				}
			}
		}
		if (breakAndCreateBlob || (i == count - 1)) {
			//pack and flush file and start over
			if (blobFile) {
				fclose(blobFile);
			}

			//TODO get the right profile depending on ext
			lockBlobMutex();		
			wcscpy(blobs[blobCount].filename, blobFilename);
			blobs[blobCount].profile = getProfileForExtensionOrDefault(ext, profile);
			releaseBlobMutex();

			HANDLE handle = _beginthread(threadBlockPack, 0, &blobs[blobCount]);

			lockBlobMutex();
			blobs[blobCount].handle = handle;
			releaseBlobMutex();
					
			printf("\n writing blob nr %llu", blobCount);
			printf("\n BLOB FOUND NOoFfILES=%d", noOfFilesInBlobCount);
			if (noOfFilesInBlobCount == 0) {
				// get the single file in this blob				
				blobs[blobCount].singleFileIndex = i;
				printf("\n BLOB IDENTIFIED AS SINGLE POINTING TO FILE %ls", fileList[i].name);
			}
			else {
				blobs[blobCount].singleFileIndex = UINT64_MAX;
			}
			blobCount++;
			noOfFilesInBlobCount = 0;
			//printf("\n Blob nr %lld has size %lld and for extension '%ls'", blobCount - 1, blobSizes[blobCount - 1], ext);	
			if (i < count - 1) {
				getNewBlobFilename(blobFilename, name, next_ext);
				blobFile = openWrite(blobFilename);
			}
		}
	
	}
	//myExit();
	//wait for all threads
	for (uint64_t i = 0; i < blobCount; i++) {
		WaitForSingleObject(blobs[i].handle, INFINITE);
	}

	//write sizes of single files
	for (uint64_t i = 0; i < blobCount; i++) {
		uint64_t ind = blobs[i].singleFileIndex;
		//for single blobs store blob size in fileList to save space
		if (ind < UINT64_MAX) {
			fileList[ind].size = getFileSizeByName(blobs[i].filename);
		}
    }

	// write headers now that we have adjusted the sizes!
	FILE* out = openWrite(dest);
	writeArchiveHeader(out, dirInfo, dir);

	//flush all files to big blob
	for (uint64_t i = 0; i < blobCount; i++) {		
		wchar_t* filename = blobs[i].filename;		

		if (i < blobCount - 1 && blobs[i].singleFileIndex == UINT64_MAX) {
			    uint64_t size = getFileSizeByName(filename);
				writeDynamicSize32or64(size, out);			
		}
		appendFileToFile(out, filename);
		_wremove(filename);
	}
	fclose(out);
}

void archivePackInternal(wchar_t* dir, const wchar_t* dest, completePackProfile profile) {

	printf("\n Starting archive pack for %ls", dir);
	fileListAndCount_t dirInfo = storeDirectoryFilenames(dir, true);
	file_t* fileList = dirInfo.fileList;
	int64_t count = dirInfo.count;
	
	printf("\n Count = %lld", count);
		
	quickSortCompareEndings(fileList, count);		
	identifyEqualFiles(dir, dirInfo);
	archivePackSemiSeparated(dest, profile, dirInfo, dir);		
}


fileListAndCount_t readSizesHeader(memfile* in) {
	fileListAndCount_t dirInfo;	
	rewindMem(in);	
	uint64_t count = getMemSize(in) / 8;
	dirInfo.fileList = malloc(count * sizeof(file_t));	
	for (int i = 0; i < count; i++) {
		dirInfo.fileList[i].size = 0;
	}
	if (dirInfo.fileList == NULL) {
		printf("\n out of memory in archive_packer.readHeader!!");
		myExit();
	}
		
	for (int k = 0; k < 8; k++) {
		for (int i = 0; i < count; i++) {			
			setByteAtPos(&(dirInfo.fileList[i].size), fgetcc(in), k);
		}
	}
	dirInfo.count = count;
	return dirInfo;
}




void readNamesHeader(FILE* in, wchar_t* dir, fileListAndCount_t* dirInfo) {
	file_t* filenames = dirInfo->fileList;
	if (filenames == NULL) {
		printf("\n out of memory in archive_packer.readHeader!!");
		myExit();
		return;
	}

	// Read names
	int i = 0;
	wchar_t temp_wstr[2000] = { 0 };
	temp_wstr[0] = '/';
	while (i < dirInfo->count) {
		uint64_t lastValue = myCodingToWchar(in, &temp_wstr[1]);
		// lastValue could be 0 or index to equal file
		printf("\n lastValue %llu", lastValue);
		if (lastValue != UINT64_MAX) {
			filenames[i].equalSizeNumber = i - lastValue;
			filenames[i].size = 0;
			printf("\n THERE WAS AN EQUAL FILE nr %llu", lastValue);
		}
		else {
			filenames[i].equalSizeNumber = UINT64_MAX;
		}
		/*
		if (dir[wcslen(dir) - 1] == '/' || dir[wcslen(dir) - 1] == '\\') {
			dir[wcslen(dir) - 1] = 0;
		}
		*/
		wcscpy(filenames[i].name, dir);
		wchar_t* lastAdd = temp_wstr;
		if (dir[wcslen(dir) - 1] == '/' || dir[wcslen(dir) - 1] == '\\') {
			if (temp_wstr[0] == '/' || temp_wstr[0] == '\\') {
				lastAdd++;
			}
		}
		wcscat(filenames[i].name, lastAdd);
		printf("\n result: %ls %llu", filenames[i].name, filenames[i].size);
	
		i++;
	}	
}


fileListAndCount_t readPackedSizesHeader(FILE* in, uint32_t headerSize) {
	
	printf("\n Read packed sizes Header size: %d", headerSize);
		
	memfile* headerPackedMem = getEmptyMem(L"archiveunpack_headersizespacked");
	copy_chunk_to_mem(in, headerPackedMem, headerSize);
	completePackProfile headerSizesPackProfile = getHeaderSizesProfile();
	memfile* headerUnpacked = multiUnpack(headerPackedMem, headerSizesPackProfile);
	//everyOtherDecode(headerUnpacked1, headerUnpacked2);
	
	fileListAndCount_t res =  readSizesHeader(headerUnpacked);

	return res;
}

void readPackedNamesHeader(FILE* in, wchar_t* dir, uint32_t headerSize, fileListAndCount_t* fileList) {	
	
	printf("\n Read packed Names Header size: %d", headerSize);

	//const wchar_t headerUnpacked1[100] = { 0 };
	//get_temp_filew(headerUnpacked1, L"archiveuntar_headerunpackedhalf");
	const wchar_t headerUnpacked[200] = { 0 };
	get_temp_filew(headerUnpacked, L"archiveuntar_headernamesunpacked");

	//const wchar_t headerUnpacked1[200] = { 0 };
	//get_temp_filew(headerUnpacked1, L"archiveuntar_headernamesunpacked1");


	//TODO should pack to memfile instead
	completePackProfile headerNamesPackProfile = getHeaderNamesProfile();
	multiUnpackBlockToFile(in, headerUnpacked, headerSize, headerNamesPackProfile);
	
	//everyOtherDecode(headerUnpacked1, headerUnpacked);
	
	FILE* headerFile = openRead(headerUnpacked);
	readNamesHeader(headerFile, dir, fileList);

	fclose(headerFile);
	_wremove(headerUnpacked);		
}



uint64_t getBlobSize(uint64_t i, uint64_t nrOfBlobs, uint64_t singeBlobFileIndex, file_t* filenames, FILE *in) {
	if (i == nrOfBlobs - 1) {
		return UINT64_MAX;
	}
	else if (singeBlobFileIndex == UINT64_MAX) {
		return readDynamicSize32or64(in);
	}
	else {
		return filenames[singeBlobFileIndex].size;
	}
}

void archiveUnpackSemiSeparated(FILE* in, fileListAndCount_t dirInfo, wchar_t* dir, completePackProfile profile) {

	blobsInfo_t blobsInfo = fetchBlobsInfo(dirInfo);
	uint64_t* blobsInfoArr = blobsInfo.sizesList;
	uint64_t nrOfBlobs = blobsInfo.count,
		count = dirInfo.count;
	file_t* filenames = dirInfo.fileList;
	printf("\n archiveUnpackSemiSeparated .nrofBlobs=%llu", nrOfBlobs);

	wchar100_t* extList = blobsInfo.extList;

	for (uint64_t i = 0; i < count; i++) {

		createMissingDirs(filenames[i].name, dir);
	}

	// Create blob array
	for (uint64_t i = 0; i < nrOfBlobs; i++) {
		uint64_t singleBlobFileIndex = blobsInfoArr[i];
		uint64_t size = getBlobSize(i, nrOfBlobs, singleBlobFileIndex, filenames, in);
		wchar_t* ext = extList[i].s;

		//printf("\nBlob nr:%llu size:%llu ext:%ls", i, size, ext);

		lockBlobMutex();
		if (singleBlobFileIndex < UINT64_MAX) {
			//single blob			
			wcscpy(blobs[i].filename, filenames[singleBlobFileIndex].name);
		}
		else {
			get_temp_filew(blobs[i].filename, L"archive_unpack_blob_semisep");
		}
		releaseBlobMutex();
		
		completePackProfile comp = getProfileForExtensionOrDefault(ext, 
			profile);
		blobs[i].profile = comp;

		copyFileChunkToFile(in, blobs[i].filename, size);

		HANDLE handle = _beginthread(threadBlockUnPack, 0, &blobs[i]);
		//threadBlockUnPack(&blobs[i]);
		lockBlobMutex();
		blobs[i].handle = handle;
		releaseBlobMutex();
	}
	fclose(in);

	// Add unpacked files together
	const wchar_t masterFilename[200] = { 0 };	
	FILE* out = openTempFile(masterFilename, L"archive_unpack_semisep_master");

	for (uint64_t i = 0; i < nrOfBlobs; i++) {
		WaitForSingleObject(blobs[i].handle, INFINITE);
		lockBlobMutex();
		wchar_t* filename = blobs[i].filename;
		releaseBlobMutex();

		//don't add if blob contains just one file!
		if (blobsInfoArr[i] == UINT64_MAX) {
			printf("\n Blob nr %llu appending normally (>1 file)", i);
			appendFileToFile(out, filename);
			_wremove(filename);
		}
	}
	fclose(out);

	FILE* masterFile = openRead(masterFilename);

	
	for (int i = 0; i < count; i++) {

		
		//printf("\n Reading: %ls sized:%" PRId64, filenames[i].name, filenames[i].size);
		uint64_t equalIndex = filenames[i].equalSizeNumber;
		if (equalIndex != UINT64_MAX) {
			printf("\n  OOOOOOOOOOOOOOOOOOOOOOOOO EQUAL FOUND!!");
			copyFile(filenames[equalIndex].name, filenames[i].name);
		}
		else {
			
			bool justOneFile = false;
			for (int k = 0; k < nrOfBlobs; k++) {
				if (blobsInfoArr[k] == i) {
					justOneFile = true;
					break;
				}
			}
			if (!justOneFile) {

				copyFileChunkToFile(in, filenames[i].name, filenames[i].size);
			}
			
		}
	}
	free(blobsInfoArr);
	free(extList);
	_wremove(masterFilename);
}

void archiveUnpackInternal(const wchar_t* src, wchar_t* dir, completePackProfile profile) {

	printf("\n *** Archive Unpack *** ");			

	FILE* in = openRead(src);	
	
	uint64_t headerSizesPackedSize = readDynamicSize16or64(in);
	uint64_t headerNamesPackedSize = readDynamicSize16or64(in); 
				
	fileListAndCount_t fileList = readPackedSizesHeader(in, headerSizesPackedSize);
	readPackedNamesHeader(in, dir, headerNamesPackedSize, &fileList);	

	printf("\n Unpacked and read headers successfully! Now unpacking files...");
	
	file_t* filenames = fileList.fileList;
	int64_t count = fileList.count;
	const wchar_t blockUnpackFilename[200] = { 0 };
	archiveUnpackSemiSeparated(in, fileList, dir, profile);			
}


void archive_pack(const wchar_t* dir, const wchar_t* dest, completePackProfile profile) {
    archivePackInternal(dir, dest, profile);			
}

void archive_unpack(const wchar_t* src, wchar_t* dir, completePackProfile profile) {
	archiveUnpackInternal(src, dir, profile);	
}


