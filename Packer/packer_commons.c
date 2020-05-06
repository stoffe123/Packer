



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