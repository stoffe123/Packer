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


completePackProfile getProfileForExtension(wchar_t* ext) {

	if (equalsIgnoreCase(ext, L"xmi")) {
		packProfile mainProfile = {
			.rle_ratio = 100,
			.twobyte_ratio = 89,
			.recursive_limit = 539,
			.twobyte_threshold_max = 8490,
			.twobyte_threshold_divide = 3427,
			.twobyte_threshold_min = 996,
			.seqlenMinLimit3 = 175,
			.blockSizeMinus = 6,
			.sizeMaxForCanonicalHeaderPack = 327,
			.sizeMinForSeqPack = 15290,
			.sizeMinForCanonical = 418,
			.sizeMaxForSuperslim = 36223
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 100,
		   .recursive_limit = 605,
		   .twobyte_threshold_max = 10401,
		   .twobyte_threshold_divide = 2382,
		   .twobyte_threshold_min = 101,
		   .seqlenMinLimit3 = 227,
		   .blockSizeMinus = 179,
		   .sizeMaxForCanonicalHeaderPack = 532,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 376,
		   .sizeMaxForSuperslim = 32995
		},
			offsetProfile = {
		   .rle_ratio = 82,
		   .twobyte_ratio = 76,
		   .recursive_limit = 251,
		   .twobyte_threshold_max = 11438,
		   .twobyte_threshold_divide = 1280,
		   .twobyte_threshold_min = 933,
		   .seqlenMinLimit3 = 81,
		   .blockSizeMinus = 46,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 25274,
		   .sizeMinForCanonical = 467,
		   .sizeMaxForSuperslim = 11805
		},
			distanceProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 78,
		   .recursive_limit = 275,
		   .twobyte_threshold_max = 6429,
		   .twobyte_threshold_divide = 2904,
		   .twobyte_threshold_min = 910,
		   .seqlenMinLimit3 = 225,
		   .blockSizeMinus = 88,
		   .sizeMaxForCanonicalHeaderPack = 565,
		   .sizeMinForSeqPack = 585,
		   .sizeMinForCanonical = 591,
		   .sizeMaxForSuperslim = 7911
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"txt")) {
		packProfile mainProfile = {
			.rle_ratio = 58,
			.twobyte_ratio = 79,
			.recursive_limit = 421,
			.twobyte_threshold_max = 7382,
			.twobyte_threshold_divide = 3865,
			.twobyte_threshold_min = 913,
			.seqlenMinLimit3 = 191,
			.blockSizeMinus = 40,
			.sizeMaxForCanonicalHeaderPack = 427,
			.sizeMinForSeqPack = 5555,
			.sizeMinForCanonical = 541,
			.sizeMaxForSuperslim = 56167
		},
			seqlenProfile = {
		   .rle_ratio = 53,
		   .twobyte_ratio = 100,
		   .recursive_limit = 587,
		   .twobyte_threshold_max = 8777,
		   .twobyte_threshold_divide = 2133,
		   .twobyte_threshold_min = 234,
		   .seqlenMinLimit3 = 255,
		   .blockSizeMinus = 78,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 1198,
		   .sizeMinForCanonical = 441,
		   .sizeMaxForSuperslim = 61045
		},
			offsetProfile = {
		   .rle_ratio = 83,
		   .twobyte_ratio = 80,
		   .recursive_limit = 269,
		   .twobyte_threshold_max = 6826,
		   .twobyte_threshold_divide = 3238,
		   .twobyte_threshold_min = 892,
		   .seqlenMinLimit3 = 83,
		   .blockSizeMinus = 212,
		   .sizeMaxForCanonicalHeaderPack = 163,
		   .sizeMinForSeqPack = 613,
		   .sizeMinForCanonical = 531,
		   .sizeMaxForSuperslim = 43379
		},
			distanceProfile = {
		   .rle_ratio = 50,
		   .twobyte_ratio = 86,
		   .recursive_limit = 425,
		   .twobyte_threshold_max = 7881,
		   .twobyte_threshold_divide = 2385,
		   .twobyte_threshold_min = 570,
		   .seqlenMinLimit3 = 131,
		   .blockSizeMinus = 2,
		   .sizeMaxForCanonicalHeaderPack = 1063,
		   .sizeMinForSeqPack = 19130,
		   .sizeMinForCanonical = 700,
		   .sizeMaxForSuperslim = 20158
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"dll")) {
		packProfile mainProfile = {
			.rle_ratio = 93,
			.twobyte_ratio = 66,
			.recursive_limit = 590,
			.twobyte_threshold_max = 9489,
			.twobyte_threshold_divide = 3327,
			.twobyte_threshold_min = 789,
			.seqlenMinLimit3 = 71,
			.blockSizeMinus = 0,
			.sizeMaxForCanonicalHeaderPack = 436,
			.sizeMinForSeqPack = 12194,
			.sizeMinForCanonical = 338,
			.sizeMaxForSuperslim = 48117
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 100,
		   .recursive_limit = 605,
		   .twobyte_threshold_max = 10401,
		   .twobyte_threshold_divide = 2382,
		   .twobyte_threshold_min = 101,
		   .seqlenMinLimit3 = 227,
		   .blockSizeMinus = 179,
		   .sizeMaxForCanonicalHeaderPack = 532,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 376,
		   .sizeMaxForSuperslim = 32995
		},
			offsetProfile = {
		   .rle_ratio = 82,
		   .twobyte_ratio = 76,
		   .recursive_limit = 251,
		   .twobyte_threshold_max = 11438,
		   .twobyte_threshold_divide = 1280,
		   .twobyte_threshold_min = 933,
		   .seqlenMinLimit3 = 81,
		   .blockSizeMinus = 46,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 25274,
		   .sizeMinForCanonical = 467,
		   .sizeMaxForSuperslim = 11805
		},
			distanceProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 78,
		   .recursive_limit = 275,
		   .twobyte_threshold_max = 6429,
		   .twobyte_threshold_divide = 2904,
		   .twobyte_threshold_min = 910,
		   .seqlenMinLimit3 = 225,
		   .blockSizeMinus = 88,
		   .sizeMaxForCanonicalHeaderPack = 565,
		   .sizeMinForSeqPack = 585,
		   .sizeMinForCanonical = 591,
		   .sizeMaxForSuperslim = 7911
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"exe")) {

		//FOUND BEST !!! Size:14693595
		packProfile mainProfile = {
		.rle_ratio = 74,
		.twobyte_ratio = 100,
		.recursive_limit = 649,
		.twobyte_threshold_max = 11833,
		.twobyte_threshold_divide = 2070,
		.twobyte_threshold_min = 900,
		.seqlenMinLimit3 = 197,
		.blockSizeMinus = 171,
		.sizeMaxForCanonicalHeaderPack = 289,
		.sizeMinForSeqPack = 21044,
		.sizeMinForCanonical = 335,
		.sizeMaxForSuperslim = 54424
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 100,
		   .recursive_limit = 605,
		   .twobyte_threshold_max = 10401,
		   .twobyte_threshold_divide = 2382,
		   .twobyte_threshold_min = 101,
		   .seqlenMinLimit3 = 227,
		   .blockSizeMinus = 179,
		   .sizeMaxForCanonicalHeaderPack = 532,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 376,
		   .sizeMaxForSuperslim = 32995
		},
			offsetProfile = {
		   .rle_ratio = 82,
		   .twobyte_ratio = 76,
		   .recursive_limit = 251,
		   .twobyte_threshold_max = 11438,
		   .twobyte_threshold_divide = 1280,
		   .twobyte_threshold_min = 933,
		   .seqlenMinLimit3 = 81,
		   .blockSizeMinus = 46,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 25274,
		   .sizeMinForCanonical = 467,
		   .sizeMaxForSuperslim = 11805
		},
			distanceProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 78,
		   .recursive_limit = 275,
		   .twobyte_threshold_max = 6429,
		   .twobyte_threshold_divide = 2904,
		   .twobyte_threshold_min = 910,
		   .seqlenMinLimit3 = 225,
		   .blockSizeMinus = 88,
		   .sizeMaxForCanonicalHeaderPack = 565,
		   .sizeMinForSeqPack = 585,
		   .sizeMinForCanonical = 591,
		   .sizeMaxForSuperslim = 7911
		};

		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"doc")) {

		//Size: 693305
		packProfile mainProfile = {
		.rle_ratio = 89,
		.twobyte_ratio = 47,
		.recursive_limit = 447,
		.twobyte_threshold_max = 9132,
		.twobyte_threshold_divide = 4000,
		.twobyte_threshold_min = 1000,
		.seqlenMinLimit3 = 255,
		.blockSizeMinus = 17,
		.sizeMaxForCanonicalHeaderPack = 178,
		.sizeMinForSeqPack = 16964,
		.sizeMinForCanonical = 550,
		.sizeMaxForSuperslim = 14138
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 100,
		   .recursive_limit = 605,
		   .twobyte_threshold_max = 10401,
		   .twobyte_threshold_divide = 2382,
		   .twobyte_threshold_min = 101,
		   .seqlenMinLimit3 = 227,
		   .blockSizeMinus = 179,
		   .sizeMaxForCanonicalHeaderPack = 532,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 376,
		   .sizeMaxForSuperslim = 32995
		},
			offsetProfile = {
		   .rle_ratio = 82,
		   .twobyte_ratio = 76,
		   .recursive_limit = 251,
		   .twobyte_threshold_max = 11438,
		   .twobyte_threshold_divide = 1280,
		   .twobyte_threshold_min = 933,
		   .seqlenMinLimit3 = 81,
		   .blockSizeMinus = 46,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 25274,
		   .sizeMinForCanonical = 467,
		   .sizeMaxForSuperslim = 11805
		},
			distanceProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 78,
		   .recursive_limit = 275,
		   .twobyte_threshold_max = 6429,
		   .twobyte_threshold_divide = 2904,
		   .twobyte_threshold_min = 910,
		   .seqlenMinLimit3 = 225,
		   .blockSizeMinus = 88,
		   .sizeMaxForCanonicalHeaderPack = 565,
		   .sizeMinForSeqPack = 585,
		   .sizeMinForCanonical = 591,
		   .sizeMaxForSuperslim = 7911
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"html")) {

		//Size: 4127
		packProfile mainProfile = {
		.rle_ratio = 92,
		.twobyte_ratio = 84,
		.recursive_limit = 550,
		.twobyte_threshold_max = 7442,
		.twobyte_threshold_divide = 3427,
		.twobyte_threshold_min = 933,
		.seqlenMinLimit3 = 198,
		.blockSizeMinus = 19,
		.sizeMaxForCanonicalHeaderPack = 290,
		.sizeMinForSeqPack = 15290,
		.sizeMinForCanonical = 442,
		.sizeMaxForSuperslim = 38323
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 100,
		   .recursive_limit = 605,
		   .twobyte_threshold_max = 10401,
		   .twobyte_threshold_divide = 2382,
		   .twobyte_threshold_min = 101,
		   .seqlenMinLimit3 = 227,
		   .blockSizeMinus = 179,
		   .sizeMaxForCanonicalHeaderPack = 532,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 376,
		   .sizeMaxForSuperslim = 32995
		},
			offsetProfile = {
		   .rle_ratio = 82,
		   .twobyte_ratio = 76,
		   .recursive_limit = 251,
		   .twobyte_threshold_max = 11438,
		   .twobyte_threshold_divide = 1280,
		   .twobyte_threshold_min = 933,
		   .seqlenMinLimit3 = 81,
		   .blockSizeMinus = 46,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 25274,
		   .sizeMinForCanonical = 467,
		   .sizeMaxForSuperslim = 11805
		},
			distanceProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 78,
		   .recursive_limit = 275,
		   .twobyte_threshold_max = 6429,
		   .twobyte_threshold_divide = 2904,
		   .twobyte_threshold_min = 910,
		   .seqlenMinLimit3 = 225,
		   .blockSizeMinus = 88,
		   .sizeMaxForCanonicalHeaderPack = 565,
		   .sizeMinForSeqPack = 585,
		   .sizeMinForCanonical = 591,
		   .sizeMaxForSuperslim = 7911
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"cs")) {
		//Size: 29678
		packProfile mainProfile = {
		.rle_ratio = 93,
		.twobyte_ratio = 98,
		.recursive_limit = 615,
		.twobyte_threshold_max = 10333,
		.twobyte_threshold_divide = 3729,
		.twobyte_threshold_min = 895,
		.seqlenMinLimit3 = 187,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 262,
		.sizeMinForSeqPack = 10,
		.sizeMinForCanonical = 578,
		.sizeMaxForSuperslim = 30003
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 100,
		   .recursive_limit = 605,
		   .twobyte_threshold_max = 10401,
		   .twobyte_threshold_divide = 2382,
		   .twobyte_threshold_min = 101,
		   .seqlenMinLimit3 = 227,
		   .blockSizeMinus = 179,
		   .sizeMaxForCanonicalHeaderPack = 532,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 376,
		   .sizeMaxForSuperslim = 32995
		},
			offsetProfile = {
		   .rle_ratio = 82,
		   .twobyte_ratio = 76,
		   .recursive_limit = 251,
		   .twobyte_threshold_max = 11438,
		   .twobyte_threshold_divide = 1280,
		   .twobyte_threshold_min = 933,
		   .seqlenMinLimit3 = 81,
		   .blockSizeMinus = 46,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 25274,
		   .sizeMinForCanonical = 467,
		   .sizeMaxForSuperslim = 11805
		},
			distanceProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 78,
		   .recursive_limit = 275,
		   .twobyte_threshold_max = 6429,
		   .twobyte_threshold_divide = 2904,
		   .twobyte_threshold_min = 910,
		   .seqlenMinLimit3 = 225,
		   .blockSizeMinus = 88,
		   .sizeMaxForCanonicalHeaderPack = 565,
		   .sizeMinForSeqPack = 585,
		   .sizeMinForCanonical = 591,
		   .sizeMaxForSuperslim = 7911
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"prx")) {
		//Size: 5805557
		packProfile mainProfile = {
		.rle_ratio = 85,
		.twobyte_ratio = 95,
		.recursive_limit = 536,
		.twobyte_threshold_max = 12145,
		.twobyte_threshold_divide = 2350,
		.twobyte_threshold_min = 993,
		.seqlenMinLimit3 = 72,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 306,
		.sizeMinForSeqPack = 4244,
		.sizeMinForCanonical = 563,
		.sizeMaxForSuperslim = 85061
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 100,
		   .recursive_limit = 605,
		   .twobyte_threshold_max = 10401,
		   .twobyte_threshold_divide = 2382,
		   .twobyte_threshold_min = 101,
		   .seqlenMinLimit3 = 227,
		   .blockSizeMinus = 179,
		   .sizeMaxForCanonicalHeaderPack = 532,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 376,
		   .sizeMaxForSuperslim = 32995
		},
			offsetProfile = {
		   .rle_ratio = 82,
		   .twobyte_ratio = 76,
		   .recursive_limit = 251,
		   .twobyte_threshold_max = 11438,
		   .twobyte_threshold_divide = 1280,
		   .twobyte_threshold_min = 933,
		   .seqlenMinLimit3 = 81,
		   .blockSizeMinus = 46,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 25274,
		   .sizeMinForCanonical = 467,
		   .sizeMaxForSuperslim = 11805
		},
			distanceProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 78,
		   .recursive_limit = 275,
		   .twobyte_threshold_max = 6429,
		   .twobyte_threshold_divide = 2904,
		   .twobyte_threshold_min = 910,
		   .seqlenMinLimit3 = 225,
		   .blockSizeMinus = 88,
		   .sizeMaxForCanonicalHeaderPack = 565,
		   .sizeMinForSeqPack = 585,
		   .sizeMinForCanonical = 591,
		   .sizeMaxForSuperslim = 7911
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"prx")) {

		//Size: 1397677
		packProfile mainProfile = {
		.rle_ratio = 65,
		.twobyte_ratio = 21,
		.recursive_limit = 700,
		.twobyte_threshold_max = 8350,
		.twobyte_threshold_divide = 3007,
		.twobyte_threshold_min = 843,
		.seqlenMinLimit3 = 255,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 712,
		.sizeMinForSeqPack = 7610,
		.sizeMinForCanonical = 320,
		.sizeMaxForSuperslim = 87234
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 100,
		   .recursive_limit = 605,
		   .twobyte_threshold_max = 10401,
		   .twobyte_threshold_divide = 2382,
		   .twobyte_threshold_min = 101,
		   .seqlenMinLimit3 = 227,
		   .blockSizeMinus = 179,
		   .sizeMaxForCanonicalHeaderPack = 532,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 376,
		   .sizeMaxForSuperslim = 32995
		},
			offsetProfile = {
		   .rle_ratio = 82,
		   .twobyte_ratio = 76,
		   .recursive_limit = 251,
		   .twobyte_threshold_max = 11438,
		   .twobyte_threshold_divide = 1280,
		   .twobyte_threshold_min = 933,
		   .seqlenMinLimit3 = 81,
		   .blockSizeMinus = 46,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 25274,
		   .sizeMinForCanonical = 467,
		   .sizeMaxForSuperslim = 11805
		},
			distanceProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 78,
		   .recursive_limit = 275,
		   .twobyte_threshold_max = 6429,
		   .twobyte_threshold_divide = 2904,
		   .twobyte_threshold_min = 910,
		   .seqlenMinLimit3 = 225,
		   .blockSizeMinus = 88,
		   .sizeMaxForCanonicalHeaderPack = 565,
		   .sizeMinForSeqPack = 585,
		   .sizeMinForCanonical = 591,
		   .sizeMaxForSuperslim = 7911
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"mp3")) {

		//Size: 2135790
		packProfile mainProfile = {
		.rle_ratio = 79,
		.twobyte_ratio = 91,
		.recursive_limit = 693,
		.twobyte_threshold_max = 10116,
		.twobyte_threshold_divide = 3169,
		.twobyte_threshold_min = 942,
		.seqlenMinLimit3 = 109,
		.blockSizeMinus = 64,
		.sizeMaxForCanonicalHeaderPack = 215,
		.sizeMinForSeqPack = 25201,
		.sizeMinForCanonical = 465,
		.sizeMaxForSuperslim = 55836
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 100,
		   .recursive_limit = 605,
		   .twobyte_threshold_max = 10401,
		   .twobyte_threshold_divide = 2382,
		   .twobyte_threshold_min = 101,
		   .seqlenMinLimit3 = 227,
		   .blockSizeMinus = 179,
		   .sizeMaxForCanonicalHeaderPack = 532,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 376,
		   .sizeMaxForSuperslim = 32995
		},
			offsetProfile = {
		   .rle_ratio = 82,
		   .twobyte_ratio = 76,
		   .recursive_limit = 251,
		   .twobyte_threshold_max = 11438,
		   .twobyte_threshold_divide = 1280,
		   .twobyte_threshold_min = 933,
		   .seqlenMinLimit3 = 81,
		   .blockSizeMinus = 46,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 25274,
		   .sizeMinForCanonical = 467,
		   .sizeMaxForSuperslim = 11805
		},
			distanceProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 78,
		   .recursive_limit = 275,
		   .twobyte_threshold_max = 6429,
		   .twobyte_threshold_divide = 2904,
		   .twobyte_threshold_min = 910,
		   .seqlenMinLimit3 = 225,
		   .blockSizeMinus = 88,
		   .sizeMaxForCanonicalHeaderPack = 565,
		   .sizeMinForSeqPack = 585,
		   .sizeMinForCanonical = 591,
		   .sizeMaxForSuperslim = 7911
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"resx")) {
		//Size: 2845515
		packProfile mainProfile = {
		.rle_ratio = 96,
		.twobyte_ratio = 99,
		.recursive_limit = 458,
		.twobyte_threshold_max = 3928,
		.twobyte_threshold_divide = 1368,
		.twobyte_threshold_min = 966,
		.seqlenMinLimit3 = 0,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 871,
		.sizeMinForSeqPack = 22722,
		.sizeMinForCanonical = 265,
		.sizeMaxForSuperslim = 64239
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 100,
		   .recursive_limit = 605,
		   .twobyte_threshold_max = 10401,
		   .twobyte_threshold_divide = 2382,
		   .twobyte_threshold_min = 101,
		   .seqlenMinLimit3 = 227,
		   .blockSizeMinus = 179,
		   .sizeMaxForCanonicalHeaderPack = 532,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 376,
		   .sizeMaxForSuperslim = 32995
		},
			offsetProfile = {
		   .rle_ratio = 82,
		   .twobyte_ratio = 76,
		   .recursive_limit = 251,
		   .twobyte_threshold_max = 11438,
		   .twobyte_threshold_divide = 1280,
		   .twobyte_threshold_min = 933,
		   .seqlenMinLimit3 = 81,
		   .blockSizeMinus = 46,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 25274,
		   .sizeMinForCanonical = 467,
		   .sizeMaxForSuperslim = 11805
		},
			distanceProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 78,
		   .recursive_limit = 275,
		   .twobyte_threshold_max = 6429,
		   .twobyte_threshold_divide = 2904,
		   .twobyte_threshold_min = 910,
		   .seqlenMinLimit3 = 225,
		   .blockSizeMinus = 88,
		   .sizeMaxForCanonicalHeaderPack = 565,
		   .sizeMinForSeqPack = 585,
		   .sizeMinForCanonical = 591,
		   .sizeMaxForSuperslim = 7911
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"fdt")) {
		//Size: 1993306
		packProfile mainProfile = {
		.rle_ratio = 99,
		.twobyte_ratio = 67,
		.recursive_limit = 71,
		.twobyte_threshold_max = 4882,
		.twobyte_threshold_divide = 2340,
		.twobyte_threshold_min = 1000,
		.seqlenMinLimit3 = 67,
		.blockSizeMinus = 29,
		.sizeMaxForCanonicalHeaderPack = 301,
		.sizeMinForSeqPack = 8631,
		.sizeMinForCanonical = 584,
		.sizeMaxForSuperslim = 56728
		},
			seqlenProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 97,
		   .recursive_limit = 180,
		   .twobyte_threshold_max = 5226,
		   .twobyte_threshold_divide = 2233,
		   .twobyte_threshold_min = 185,
		   .seqlenMinLimit3 = 43,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 175,
		   .sizeMinForSeqPack = 2600,
		   .sizeMinForCanonical = 30,
		   .sizeMaxForSuperslim = 16384
		},
			offsetProfile = {
		   .rle_ratio = 74,
		   .twobyte_ratio = 95,
		   .recursive_limit = 61,
		   .twobyte_threshold_max = 11404,
		   .twobyte_threshold_divide = 2520,
		   .twobyte_threshold_min = 384,
		   .seqlenMinLimit3 = 82,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 530,
		   .sizeMinForSeqPack = 2600,
		   .sizeMinForCanonical = 261,
		   .sizeMaxForSuperslim = 16384
		},
			distanceProfile = {
		   .rle_ratio = 71,
		   .twobyte_ratio = 100,
		   .recursive_limit = 20,
		   .twobyte_threshold_max = 3641,
		   .twobyte_threshold_divide = 3972,
		   .twobyte_threshold_min = 37,
		   .seqlenMinLimit3 = 35,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 256,
		   .sizeMinForSeqPack = 2600,
		   .sizeMinForCanonical = 300,
		   .sizeMaxForSuperslim = 16384
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"xml")) {
		//Size: 517434
		packProfile mainProfile = {
		.rle_ratio = 90,
		.twobyte_ratio = 87,
		.recursive_limit = 214,
		.twobyte_threshold_max = 10203,
		.twobyte_threshold_divide = 20,
		.twobyte_threshold_min = 809,
		.seqlenMinLimit3 = 255,
		.blockSizeMinus = 29,
		.sizeMaxForCanonicalHeaderPack = 377,
		.sizeMinForSeqPack = 1763,
		.sizeMinForCanonical = 469,
		.sizeMaxForSuperslim = 76867
		},
			seqlenProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 97,
		   .recursive_limit = 180,
		   .twobyte_threshold_max = 5226,
		   .twobyte_threshold_divide = 2233,
		   .twobyte_threshold_min = 185,
		   .seqlenMinLimit3 = 43,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 175,
		   .sizeMinForSeqPack = 2600,
		   .sizeMinForCanonical = 30,
		   .sizeMaxForSuperslim = 16384
		},
			offsetProfile = {
		   .rle_ratio = 74,
		   .twobyte_ratio = 95,
		   .recursive_limit = 61,
		   .twobyte_threshold_max = 11404,
		   .twobyte_threshold_divide = 2520,
		   .twobyte_threshold_min = 384,
		   .seqlenMinLimit3 = 82,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 530,
		   .sizeMinForSeqPack = 2600,
		   .sizeMinForCanonical = 261,
		   .sizeMaxForSuperslim = 16384
		},
			distanceProfile = {
		   .rle_ratio = 71,
		   .twobyte_ratio = 100,
		   .recursive_limit = 20,
		   .twobyte_threshold_max = 3641,
		   .twobyte_threshold_divide = 3972,
		   .twobyte_threshold_min = 37,
		   .seqlenMinLimit3 = 35,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 256,
		   .sizeMinForSeqPack = 2600,
		   .sizeMinForCanonical = 300,
		   .sizeMaxForSuperslim = 16384
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"tis")) {

		//Size: 17346786
		packProfile mainProfile = {
		.rle_ratio = 85,
		.twobyte_ratio = 80,
		.recursive_limit = 370,
		.twobyte_threshold_max = 1168,
		.twobyte_threshold_divide = 2956,
		.twobyte_threshold_min = 909,
		.seqlenMinLimit3 = 26,
		.blockSizeMinus = 9,
		.sizeMaxForCanonicalHeaderPack = 407,
		.sizeMinForSeqPack = 17024,
		.sizeMinForCanonical = 700,
		.sizeMaxForSuperslim = 67031
		},
			seqlenProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 97,
		   .recursive_limit = 180,
		   .twobyte_threshold_max = 5226,
		   .twobyte_threshold_divide = 2233,
		   .twobyte_threshold_min = 185,
		   .seqlenMinLimit3 = 43,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 175,
		   .sizeMinForSeqPack = 2600,
		   .sizeMinForCanonical = 30,
		   .sizeMaxForSuperslim = 16384
		},
			offsetProfile = {
		   .rle_ratio = 74,
		   .twobyte_ratio = 95,
		   .recursive_limit = 61,
		   .twobyte_threshold_max = 11404,
		   .twobyte_threshold_divide = 2520,
		   .twobyte_threshold_min = 384,
		   .seqlenMinLimit3 = 82,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 530,
		   .sizeMinForSeqPack = 2600,
		   .sizeMinForCanonical = 261,
		   .sizeMaxForSuperslim = 16384
		},
			distanceProfile = {
		   .rle_ratio = 71,
		   .twobyte_ratio = 100,
		   .recursive_limit = 20,
		   .twobyte_threshold_max = 3641,
		   .twobyte_threshold_divide = 3972,
		   .twobyte_threshold_min = 37,
		   .seqlenMinLimit3 = 35,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 256,
		   .sizeMinForSeqPack = 2600,
		   .sizeMinForCanonical = 300,
		   .sizeMaxForSuperslim = 16384
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"mark")) {

		//Size: 52519
		packProfile mainProfile = {
			.rle_ratio = 95,
			.twobyte_ratio = 87,
			.recursive_limit = 207,
			.twobyte_threshold_max = 11491,
			.twobyte_threshold_divide = 407,
			.twobyte_threshold_min = 875,
			.seqlenMinLimit3 = 116,
			.blockSizeMinus = 16,
			.sizeMaxForCanonicalHeaderPack = 116,
			.sizeMinForSeqPack = 16884,
			.sizeMinForCanonical = 700,
			.sizeMaxForSuperslim = 79000
		},
			seqlenProfile = {
		   .rle_ratio = 30,
		   .twobyte_ratio = 67,
		   .recursive_limit = 10,
		   .twobyte_threshold_max = 6101,
		   .twobyte_threshold_divide = 3139,
		   .twobyte_threshold_min = 416,
		   .seqlenMinLimit3 = 9,
		   .blockSizeMinus = 9,
		   .sizeMaxForCanonicalHeaderPack = 297,
		   .sizeMinForSeqPack = 19225,
		   .sizeMinForCanonical = 169,
		   .sizeMaxForSuperslim = 7427
		},
			offsetProfile = {
		   .rle_ratio = 62,
		   .twobyte_ratio = 82,
		   .recursive_limit = 227,
		   .twobyte_threshold_max = 12430,
		   .twobyte_threshold_divide = 2716,
		   .twobyte_threshold_min = 584,
		   .seqlenMinLimit3 = 103,
		   .blockSizeMinus = 79,
		   .sizeMaxForCanonicalHeaderPack = 923,
		   .sizeMinForSeqPack = 21336,
		   .sizeMinForCanonical = 10,
		   .sizeMaxForSuperslim = 5524
		},
			distanceProfile = {
		   .rle_ratio = 93,
		   .twobyte_ratio = 60,
		   .recursive_limit = 292,
		   .twobyte_threshold_max = 3878,
		   .twobyte_threshold_divide = 3575,
		   .twobyte_threshold_min = 71,
		   .seqlenMinLimit3 = 28,
		   .blockSizeMinus = 17,
		   .sizeMaxForCanonicalHeaderPack = 592,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 407,
		   .sizeMaxForSuperslim = 70226
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"csh")) {//Size: 710130
		packProfile mainProfile = {
		.rle_ratio = 89,
		.twobyte_ratio = 83,
		.recursive_limit = 427,
		.twobyte_threshold_max = 5291,
		.twobyte_threshold_divide = 3026,
		.twobyte_threshold_min = 836,
		.seqlenMinLimit3 = 3,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 500,
		.sizeMinForSeqPack = 16959,
		.sizeMinForCanonical = 627,
		.sizeMaxForSuperslim = 50003
		},
			seqlenProfile = {
		   .rle_ratio = 21,
		   .twobyte_ratio = 93,
		   .recursive_limit = 194,
		   .twobyte_threshold_max = 3755,
		   .twobyte_threshold_divide = 2672,
		   .twobyte_threshold_min = 299,
		   .seqlenMinLimit3 = 4,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 366,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 11,
		   .sizeMaxForSuperslim = 13673
		},
			offsetProfile = {
		   .rle_ratio = 62,
		   .twobyte_ratio = 100,
		   .recursive_limit = 10,
		   .twobyte_threshold_max = 12815,
		   .twobyte_threshold_divide = 3451,
		   .twobyte_threshold_min = 434,
		   .seqlenMinLimit3 = 70,
		   .blockSizeMinus = 27,
		   .sizeMaxForCanonicalHeaderPack = 373,
		   .sizeMinForSeqPack = 18280,
		   .sizeMinForCanonical = 215,
		   .sizeMaxForSuperslim = 1184
		},
			distanceProfile = {
		   .rle_ratio = 35,
		   .twobyte_ratio = 89,
		   .recursive_limit = 10,
		   .twobyte_threshold_max = 3124,
		   .twobyte_threshold_divide = 3636,
		   .twobyte_threshold_min = 39,
		   .seqlenMinLimit3 = 42,
		   .blockSizeMinus = 24,
		   .sizeMaxForCanonicalHeaderPack = 492,
		   .sizeMinForSeqPack = 5698,
		   .sizeMinForCanonical = 233,
		   .sizeMaxForSuperslim = 19225
		}; return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"icc")) {

		//Size: 2200329

		packProfile mainProfile = {
		.rle_ratio = 85,
		.twobyte_ratio = 93,
		.recursive_limit = 445,
		.twobyte_threshold_max = 5645,
		.twobyte_threshold_divide = 3615,
		.twobyte_threshold_min = 983,
		.seqlenMinLimit3 = 140,
		.blockSizeMinus = 69,
		.sizeMaxForCanonicalHeaderPack = 256,
		.sizeMinForSeqPack = 48773,
		.sizeMinForCanonical = 682,
		.sizeMaxForSuperslim = 9721
		},
			seqlenProfile = {
		   .rle_ratio = 28,
		   .twobyte_ratio = 75,
		   .recursive_limit = 167,
		   .twobyte_threshold_max = 5806,
		   .twobyte_threshold_divide = 1309,
		   .twobyte_threshold_min = 39,
		   .seqlenMinLimit3 = 143,
		   .blockSizeMinus = 2,
		   .sizeMaxForCanonicalHeaderPack = 547,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 168,
		   .sizeMaxForSuperslim = 9986
		},
			offsetProfile = {
		   .rle_ratio = 55,
		   .twobyte_ratio = 93,
		   .recursive_limit = 71,
		   .twobyte_threshold_max = 10052,
		   .twobyte_threshold_divide = 1916,
		   .twobyte_threshold_min = 220,
		   .seqlenMinLimit3 = 26,
		   .blockSizeMinus = 83,
		   .sizeMaxForCanonicalHeaderPack = 644,
		   .sizeMinForSeqPack = 16801,
		   .sizeMinForCanonical = 15,
		   .sizeMaxForSuperslim = 25953
		},
			distanceProfile = {
		   .rle_ratio = 73,
		   .twobyte_ratio = 62,
		   .recursive_limit = 161,
		   .twobyte_threshold_max = 1396,
		   .twobyte_threshold_divide = 3850,
		   .twobyte_threshold_min = 149,
		   .seqlenMinLimit3 = 69,
		   .blockSizeMinus = 65,
		   .sizeMaxForCanonicalHeaderPack = 360,
		   .sizeMinForSeqPack = 10150,
		   .sizeMinForCanonical = 191,
		   .sizeMaxForSuperslim = 6037
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}


	// -------------------------------------
	// Default profile for the rest

	packProfile mainProfile = {
		 .rle_ratio = 85,
		 .twobyte_ratio = 82,
		 .recursive_limit = 289,
		 .twobyte_threshold_max = 3104,
		 .twobyte_threshold_divide = 2912,
		 .twobyte_threshold_min = 963,
		 .seqlenMinLimit3 = 70,
		  .blockSizeMinus = 0,
		 .sizeMaxForCanonicalHeaderPack = 390,
		 .sizeMinForSeqPack = 10,
		 .sizeMinForCanonical = 547,
		 .sizeMaxForSuperslim = 61251
	},

		seqlenProfile = {
			 .rle_ratio = 31,
			 .twobyte_ratio = 97,
			 .recursive_limit = 180,
			 .twobyte_threshold_max = 5226,
			 .twobyte_threshold_divide = 2233,
			 .twobyte_threshold_min = 185,
			 .seqlenMinLimit3 = 43,
			 .sizeMaxForCanonicalHeaderPack = 175,
			 .sizeMinForSeqPack = 2600,
			 .sizeMinForCanonical = 30,
			 .sizeMaxForSuperslim = 16384
	},

		offsetProfile = {
		.rle_ratio = 74,
		.twobyte_ratio = 95,
		.recursive_limit = 61,
		.twobyte_threshold_max = 11404,
		.twobyte_threshold_divide = 2520,
		.twobyte_threshold_min = 384,
		.seqlenMinLimit3 = 82,
		.sizeMaxForCanonicalHeaderPack = 530,
		.sizeMinForSeqPack = 2600,
		.sizeMinForCanonical = 261,
		.sizeMaxForSuperslim = 16384 },

		distanceProfile = {
		.rle_ratio = 71,
		.twobyte_ratio = 100,
		.recursive_limit = 20,
		.twobyte_threshold_max = 3641,
		.twobyte_threshold_divide = 3972,
		.twobyte_threshold_min = 37,
		.seqlenMinLimit3 = 35,
		.sizeMaxForCanonicalHeaderPack = 256,
		.sizeMinForSeqPack = 2600,
		.sizeMinForCanonical = 300,
		.sizeMaxForSuperslim = 16384 };

	return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);

}