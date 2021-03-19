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

completePackProfile getProfileForExtensionOrDefault(wchar_t* ext, completePackProfile def) {

	if (equalsIgnoreCase(ext, L"xmi")) {

		//Size: 79831

		packProfile mainProfile = {
		.rle_ratio = 99,
		.twobyte_ratio = 87,
		.recursive_limit = 558,
		.twobyte_threshold_max = 8325,
		.twobyte_threshold_divide = 3129,
		.twobyte_threshold_min = 1000,
		.seqlenMinLimit3 = 180,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 294,
		.sizeMinForSeqPack = 16474,
		.sizeMinForCanonical = 371,
		.sizeMaxForSuperslim = 45061
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 100,
		   .recursive_limit = 487,
		   .twobyte_threshold_max = 12004,
		   .twobyte_threshold_divide = 1910,
		   .twobyte_threshold_min = 104,
		   .seqlenMinLimit3 = 203,
		   .blockSizeMinus = 149,
		   .sizeMaxForCanonicalHeaderPack = 557,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 374,
		   .sizeMaxForSuperslim = 32112
		},
			offsetProfile = {
		   .rle_ratio = 82,
		   .twobyte_ratio = 80,
		   .recursive_limit = 211,
		   .twobyte_threshold_max = 10087,
		   .twobyte_threshold_divide = 875,
		   .twobyte_threshold_min = 970,
		   .seqlenMinLimit3 = 34,
		   .blockSizeMinus = 109,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 24259,
		   .sizeMinForCanonical = 446,
		   .sizeMaxForSuperslim = 6707
		},
			distanceProfile = {
		   .rle_ratio = 30,
		   .twobyte_ratio = 79,
		   .recursive_limit = 239,
		   .twobyte_threshold_max = 7782,
		   .twobyte_threshold_divide = 2953,
		   .twobyte_threshold_min = 910,
		   .seqlenMinLimit3 = 223,
		   .blockSizeMinus = 88,
		   .sizeMaxForCanonicalHeaderPack = 560,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 549,
		   .sizeMaxForSuperslim = 17797
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"txt")) {

		//Size: 147550

		packProfile mainProfile = {
		.rle_ratio = 68,
		.twobyte_ratio = 67,
		.recursive_limit = 416,
		.twobyte_threshold_max = 6677,
		.twobyte_threshold_divide = 3163,
		.twobyte_threshold_min = 961,
		.seqlenMinLimit3 = 183,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 427,
		.sizeMinForSeqPack = 10,
		.sizeMinForCanonical = 555,
		.sizeMaxForSuperslim = 50570
		},
			seqlenProfile = {
		   .rle_ratio = 50,
		   .twobyte_ratio = 100,
		   .recursive_limit = 627,
		   .twobyte_threshold_max = 7614,
		   .twobyte_threshold_divide = 2133,
		   .twobyte_threshold_min = 234,
		   .seqlenMinLimit3 = 218,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 147,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 441,
		   .sizeMaxForSuperslim = 62557
		},
			offsetProfile = {
		   .rle_ratio = 95,
		   .twobyte_ratio = 74,
		   .recursive_limit = 295,
		   .twobyte_threshold_max = 6300,
		   .twobyte_threshold_divide = 2995,
		   .twobyte_threshold_min = 991,
		   .seqlenMinLimit3 = 92,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 516,
		   .sizeMaxForSuperslim = 56133
		},
			distanceProfile = {
		   .rle_ratio = 54,
		   .twobyte_ratio = 82,
		   .recursive_limit = 425,
		   .twobyte_threshold_max = 8725,
		   .twobyte_threshold_divide = 2554,
		   .twobyte_threshold_min = 674,
		   .seqlenMinLimit3 = 145,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 1122,
		   .sizeMinForSeqPack = 20603,
		   .sizeMinForCanonical = 700,
		   .sizeMaxForSuperslim = 13749
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"doc")) {

		//Size: 693327

		packProfile mainProfile = {
		.rle_ratio = 76,
		.twobyte_ratio = 50,
		.recursive_limit = 437,
		.twobyte_threshold_max = 7410,
		.twobyte_threshold_divide = 4000,
		.twobyte_threshold_min = 989,
		.seqlenMinLimit3 = 255,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 98,
		.sizeMinForSeqPack = 9756,
		.sizeMinForCanonical = 485,
		.sizeMaxForSuperslim = 15609
		},
			seqlenProfile = {
		   .rle_ratio = 13,
		   .twobyte_ratio = 94,
		   .recursive_limit = 514,
		   .twobyte_threshold_max = 9748,
		   .twobyte_threshold_divide = 2182,
		   .twobyte_threshold_min = 89,
		   .seqlenMinLimit3 = 211,
		   .blockSizeMinus = 222,
		   .sizeMaxForCanonicalHeaderPack = 385,
		   .sizeMinForSeqPack = 507,
		   .sizeMinForCanonical = 241,
		   .sizeMaxForSuperslim = 24327
		},
			offsetProfile = {
		   .rle_ratio = 91,
		   .twobyte_ratio = 75,
		   .recursive_limit = 187,
		   .twobyte_threshold_max = 9699,
		   .twobyte_threshold_divide = 996,
		   .twobyte_threshold_min = 1000,
		   .seqlenMinLimit3 = 90,
		   .blockSizeMinus = 33,
		   .sizeMaxForCanonicalHeaderPack = 86,
		   .sizeMinForSeqPack = 19592,
		   .sizeMinForCanonical = 403,
		   .sizeMaxForSuperslim = 13833
		},
			distanceProfile = {
		   .rle_ratio = 32,
		   .twobyte_ratio = 100,
		   .recursive_limit = 167,
		   .twobyte_threshold_max = 8536,
		   .twobyte_threshold_divide = 3877,
		   .twobyte_threshold_min = 865,
		   .seqlenMinLimit3 = 176,
		   .blockSizeMinus = 153,
		   .sizeMaxForCanonicalHeaderPack = 644,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 579,
		   .sizeMaxForSuperslim = 10
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

		//Size: 5804502

		packProfile mainProfile = {
		.rle_ratio = 81,
		.twobyte_ratio = 90,
		.recursive_limit = 622,
		.twobyte_threshold_max = 10679,
		.twobyte_threshold_divide = 1738,
		.twobyte_threshold_min = 891,
		.seqlenMinLimit3 = 10,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 439,
		.sizeMinForSeqPack = 16045,
		.sizeMinForCanonical = 700,
		.sizeMaxForSuperslim = 76593
		},
			seqlenProfile = {
		   .rle_ratio = 12,
		   .twobyte_ratio = 97,
		   .recursive_limit = 691,
		   .twobyte_threshold_max = 7567,
		   .twobyte_threshold_divide = 1552,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 173,
		   .blockSizeMinus = 55,
		   .sizeMaxForCanonicalHeaderPack = 412,
		   .sizeMinForSeqPack = 4497,
		   .sizeMinForCanonical = 444,
		   .sizeMaxForSuperslim = 48939
		},
			offsetProfile = {
		   .rle_ratio = 98,
		   .twobyte_ratio = 97,
		   .recursive_limit = 319,
		   .twobyte_threshold_max = 12560,
		   .twobyte_threshold_divide = 1822,
		   .twobyte_threshold_min = 1000,
		   .seqlenMinLimit3 = 66,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 716,
		   .sizeMinForSeqPack = 38287,
		   .sizeMinForCanonical = 309,
		   .sizeMaxForSuperslim = 17205
		},
			distanceProfile = {
		   .rle_ratio = 48,
		   .twobyte_ratio = 78,
		   .recursive_limit = 287,
		   .twobyte_threshold_max = 3368,
		   .twobyte_threshold_divide = 3301,
		   .twobyte_threshold_min = 957,
		   .seqlenMinLimit3 = 206,
		   .blockSizeMinus = 56,
		   .sizeMaxForCanonicalHeaderPack = 590,
		   .sizeMinForSeqPack = 9535,
		   .sizeMinForCanonical = 578,
		   .sizeMaxForSuperslim = 10
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
		.blockSizeMinus = 0,
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

		//Size: 2845517

		packProfile mainProfile = {
		.rle_ratio = 96,
		.twobyte_ratio = 100,
		.recursive_limit = 483,
		.twobyte_threshold_max = 3679,
		.twobyte_threshold_divide = 1428,
		.twobyte_threshold_min = 872,
		.seqlenMinLimit3 = 0,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 804,
		.sizeMinForSeqPack = 22722,
		.sizeMinForCanonical = 243,
		.sizeMaxForSuperslim = 58433
		},
			seqlenProfile = {
		   .rle_ratio = 22,
		   .twobyte_ratio = 100,
		   .recursive_limit = 537,
		   .twobyte_threshold_max = 10401,
		   .twobyte_threshold_divide = 2382,
		   .twobyte_threshold_min = 54,
		   .seqlenMinLimit3 = 241,
		   .blockSizeMinus = 152,
		   .sizeMaxForCanonicalHeaderPack = 483,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 376,
		   .sizeMaxForSuperslim = 32995
		},
			offsetProfile = {
		   .rle_ratio = 85,
		   .twobyte_ratio = 72,
		   .recursive_limit = 304,
		   .twobyte_threshold_max = 11438,
		   .twobyte_threshold_divide = 915,
		   .twobyte_threshold_min = 877,
		   .seqlenMinLimit3 = 94,
		   .blockSizeMinus = 48,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 29322,
		   .sizeMinForCanonical = 483,
		   .sizeMaxForSuperslim = 5294
		},
			distanceProfile = {
		   .rle_ratio = 26,
		   .twobyte_ratio = 78,
		   .recursive_limit = 308,
		   .twobyte_threshold_max = 6429,
		   .twobyte_threshold_divide = 2795,
		   .twobyte_threshold_min = 957,
		   .seqlenMinLimit3 = 238,
		   .blockSizeMinus = 103,
		   .sizeMaxForCanonicalHeaderPack = 494,
		   .sizeMinForSeqPack = 7915,
		   .sizeMinForCanonical = 523,
		   .sizeMaxForSuperslim = 1980
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"fdt")) {

		//Size: 1999.986

		packProfile mainProfile = {
		.rle_ratio = 83,
		.twobyte_ratio = 71,
		.recursive_limit = 178,
		.twobyte_threshold_max = 2187,
		.twobyte_threshold_divide = 1075,
		.twobyte_threshold_min = 775,
		.seqlenMinLimit3 = 135,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 394,
		.sizeMinForSeqPack = 14087,
		.sizeMinForCanonical = 586,
		.sizeMaxForSuperslim = 15999
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 91,
		   .recursive_limit = 59,
		   .twobyte_threshold_max = 4777,
		   .twobyte_threshold_divide = 1789,
		   .twobyte_threshold_min = 456,
		   .seqlenMinLimit3 = 3,
		   .blockSizeMinus = 99,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 17756,
		   .sizeMinForCanonical = 65,
		   .sizeMaxForSuperslim = 36468
		},
			offsetProfile = {
		   .rle_ratio = 68,
		   .twobyte_ratio = 70,
		   .recursive_limit = 202,
		   .twobyte_threshold_max = 10551,
		   .twobyte_threshold_divide = 2991,
		   .twobyte_threshold_min = 387,
		   .seqlenMinLimit3 = 49,
		   .blockSizeMinus = 30,
		   .sizeMaxForCanonicalHeaderPack = 452,
		   .sizeMinForSeqPack = 12923,
		   .sizeMinForCanonical = 145,
		   .sizeMaxForSuperslim = 20704
		},
			distanceProfile = {
		   .rle_ratio = 91,
		   .twobyte_ratio = 100,
		   .recursive_limit = 214,
		   .twobyte_threshold_max = 4702,
		   .twobyte_threshold_divide = 3210,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 12,
		   .blockSizeMinus = 45,
		   .sizeMaxForCanonicalHeaderPack = 296,
		   .sizeMinForSeqPack = 9043,
		   .sizeMinForCanonical = 275,
		   .sizeMaxForSuperslim = 5704
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"xml")) {

		//Size: 517370

		packProfile mainProfile = {
		.rle_ratio = 92,
		.twobyte_ratio = 75,
		.recursive_limit = 197,
		.twobyte_threshold_max = 10203,
		.twobyte_threshold_divide = 195,
		.twobyte_threshold_min = 699,
		.seqlenMinLimit3 = 255,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 377,
		.sizeMinForSeqPack = 13190,
		.sizeMinForCanonical = 564,
		.sizeMaxForSuperslim = 84262
		},
			seqlenProfile = {
		   .rle_ratio = 13,
		   .twobyte_ratio = 95,
		   .recursive_limit = 166,
		   .twobyte_threshold_max = 3767,
		   .twobyte_threshold_divide = 3293,
		   .twobyte_threshold_min = 511,
		   .seqlenMinLimit3 = 17,
		   .blockSizeMinus = 4,
		   .sizeMaxForCanonicalHeaderPack = 231,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 10,
		   .sizeMaxForSuperslim = 17867
		},
			offsetProfile = {
		   .rle_ratio = 77,
		   .twobyte_ratio = 93,
		   .recursive_limit = 12,
		   .twobyte_threshold_max = 9775,
		   .twobyte_threshold_divide = 2229,
		   .twobyte_threshold_min = 576,
		   .seqlenMinLimit3 = 63,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 439,
		   .sizeMinForSeqPack = 5440,
		   .sizeMinForCanonical = 337,
		   .sizeMaxForSuperslim = 20081
		},
			distanceProfile = {
		   .rle_ratio = 68,
		   .twobyte_ratio = 100,
		   .recursive_limit = 45,
		   .twobyte_threshold_max = 876,
		   .twobyte_threshold_divide = 4000,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 16,
		   .blockSizeMinus = 29,
		   .sizeMaxForCanonicalHeaderPack = 179,
		   .sizeMinForSeqPack = 18789,
		   .sizeMinForCanonical = 287,
		   .sizeMaxForSuperslim = 28109
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"tis")) {

		//Size: 17528.028

		packProfile mainProfile = {
		.rle_ratio = 91,
		.twobyte_ratio = 72,
		.recursive_limit = 50,
		.twobyte_threshold_max = 3,
		.twobyte_threshold_divide = 3007,
		.twobyte_threshold_min = 1000,
		.seqlenMinLimit3 = 32,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 466,
		.sizeMinForSeqPack = 30511,
		.sizeMinForCanonical = 10,
		.sizeMaxForSuperslim = 65129
		},
			seqlenProfile = {
		   .rle_ratio = 39,
		   .twobyte_ratio = 98,
		   .recursive_limit = 15,
		   .twobyte_threshold_max = 6931,
		   .twobyte_threshold_divide = 1911,
		   .twobyte_threshold_min = 66,
		   .seqlenMinLimit3 = 52,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 390,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 48,
		   .sizeMaxForSuperslim = 6020
		},
			offsetProfile = {
		   .rle_ratio = 57,
		   .twobyte_ratio = 93,
		   .recursive_limit = 70,
		   .twobyte_threshold_max = 10270,
		   .twobyte_threshold_divide = 1838,
		   .twobyte_threshold_min = 527,
		   .seqlenMinLimit3 = 55,
		   .blockSizeMinus = 10,
		   .sizeMaxForCanonicalHeaderPack = 412,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 47,
		   .sizeMaxForSuperslim = 34094
		},
			distanceProfile = {
		   .rle_ratio = 67,
		   .twobyte_ratio = 95,
		   .recursive_limit = 10,
		   .twobyte_threshold_max = 3835,
		   .twobyte_threshold_divide = 3309,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 0,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 369,
		   .sizeMinForSeqPack = 15733,
		   .sizeMinForCanonical = 77,
		   .sizeMaxForSuperslim = 16888
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
			.blockSizeMinus = 0,
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
		.blockSizeMinus = 0,
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

	else if (equalsIgnoreCase(ext, L"tii")) {

		//Size: 207719

		packProfile mainProfile = {
		.rle_ratio = 68,
		.twobyte_ratio = 94,
		.recursive_limit = 136,
		.twobyte_threshold_max = 3,
		.twobyte_threshold_divide = 2915,
		.twobyte_threshold_min = 65,
		.seqlenMinLimit3 = 254,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 631,
		.sizeMinForSeqPack = 10,
		.sizeMinForCanonical = 632,
		.sizeMaxForSuperslim = 64323
		},
			seqlenProfile = {
		   .rle_ratio = 28,
		   .twobyte_ratio = 57,
		   .recursive_limit = 435,
		   .twobyte_threshold_max = 3710,
		   .twobyte_threshold_divide = 2602,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 69,
		   .blockSizeMinus = 46,
		   .sizeMaxForCanonicalHeaderPack = 145,
		   .sizeMinForSeqPack = 13580,
		   .sizeMinForCanonical = 58,
		   .sizeMaxForSuperslim = 10
		},
			offsetProfile = {
		   .rle_ratio = 21,
		   .twobyte_ratio = 85,
		   .recursive_limit = 47,
		   .twobyte_threshold_max = 5972,
		   .twobyte_threshold_divide = 1801,
		   .twobyte_threshold_min = 690,
		   .seqlenMinLimit3 = 151,
		   .blockSizeMinus = 25,
		   .sizeMaxForCanonicalHeaderPack = 669,
		   .sizeMinForSeqPack = 17597,
		   .sizeMinForCanonical = 60,
		   .sizeMaxForSuperslim = 38591
		},
			distanceProfile = {
		   .rle_ratio = 74,
		   .twobyte_ratio = 100,
		   .recursive_limit = 138,
		   .twobyte_threshold_max = 3,
		   .twobyte_threshold_divide = 2047,
		   .twobyte_threshold_min = 442,
		   .seqlenMinLimit3 = 0,
		   .blockSizeMinus = 34,
		   .sizeMaxForCanonicalHeaderPack = 204,
		   .sizeMinForSeqPack = 5505,
		   .sizeMinForCanonical = 301,
		   .sizeMaxForSuperslim = 59910
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"wav")) {

		//Size: 26120357

		packProfile mainProfile = {
		.rle_ratio = 70,
		.twobyte_ratio = 97,
		.recursive_limit = 342,
		.twobyte_threshold_max = 3,
		.twobyte_threshold_divide = 2374,
		.twobyte_threshold_min = 1000,
		.seqlenMinLimit3 = 68,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 420,
		.sizeMinForSeqPack = 3274,
		.sizeMinForCanonical = 536,
		.sizeMaxForSuperslim = 85904
		},
			seqlenProfile = {
		   .rle_ratio = 43,
		   .twobyte_ratio = 86,
		   .recursive_limit = 265,
		   .twobyte_threshold_max = 9131,
		   .twobyte_threshold_divide = 859,
		   .twobyte_threshold_min = 230,
		   .seqlenMinLimit3 = 61,
		   .blockSizeMinus = 5,
		   .sizeMaxForCanonicalHeaderPack = 122,
		   .sizeMinForSeqPack = 22069,
		   .sizeMinForCanonical = 152,
		   .sizeMaxForSuperslim = 22450
		},
			offsetProfile = {
		   .rle_ratio = 63,
		   .twobyte_ratio = 92,
		   .recursive_limit = 127,
		   .twobyte_threshold_max = 8272,
		   .twobyte_threshold_divide = 2513,
		   .twobyte_threshold_min = 448,
		   .seqlenMinLimit3 = 110,
		   .blockSizeMinus = 29,
		   .sizeMaxForCanonicalHeaderPack = 333,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 469,
		   .sizeMaxForSuperslim = 10
		},
			distanceProfile = {
		   .rle_ratio = 100,
		   .twobyte_ratio = 84,
		   .recursive_limit = 31,
		   .twobyte_threshold_max = 2122,
		   .twobyte_threshold_divide = 3629,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 30,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 502,
		   .sizeMinForSeqPack = 6167,
		   .sizeMinForCanonical = 183,
		   .sizeMaxForSuperslim = 28498
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"pdb")) {

		//Size: 43922

		packProfile mainProfile = {
		.rle_ratio = 91,
		.twobyte_ratio = 84,
		.recursive_limit = 229,
		.twobyte_threshold_max = 83,
		.twobyte_threshold_divide = 3270,
		.twobyte_threshold_min = 786,
		.seqlenMinLimit3 = 166,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 670,
		.sizeMinForSeqPack = 10,
		.sizeMinForCanonical = 700,
		.sizeMaxForSuperslim = 61411
		},
			seqlenProfile = {
		   .rle_ratio = 51,
		   .twobyte_ratio = 98,
		   .recursive_limit = 459,
		   .twobyte_threshold_max = 1922,
		   .twobyte_threshold_divide = 1451,
		   .twobyte_threshold_min = 458,
		   .seqlenMinLimit3 = 56,
		   .blockSizeMinus = 32,
		   .sizeMaxForCanonicalHeaderPack = 279,
		   .sizeMinForSeqPack = 7780,
		   .sizeMinForCanonical = 63,
		   .sizeMaxForSuperslim = 7636
		},
			offsetProfile = {
		   .rle_ratio = 62,
		   .twobyte_ratio = 75,
		   .recursive_limit = 19,
		   .twobyte_threshold_max = 11428,
		   .twobyte_threshold_divide = 2212,
		   .twobyte_threshold_min = 349,
		   .seqlenMinLimit3 = 48,
		   .blockSizeMinus = 41,
		   .sizeMaxForCanonicalHeaderPack = 209,
		   .sizeMinForSeqPack = 16408,
		   .sizeMinForCanonical = 427,
		   .sizeMaxForSuperslim = 11500
		},
			distanceProfile = {
		   .rle_ratio = 55,
		   .twobyte_ratio = 76,
		   .recursive_limit = 175,
		   .twobyte_threshold_max = 3577,
		   .twobyte_threshold_divide = 3649,
		   .twobyte_threshold_min = 490,
		   .seqlenMinLimit3 = 7,
		   .blockSizeMinus = 20,
		   .sizeMaxForCanonicalHeaderPack = 144,
		   .sizeMinForSeqPack = 13940,
		   .sizeMinForCanonical = 236,
		   .sizeMaxForSuperslim = 5582
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"rss")) {

		//Size: 23918

		packProfile mainProfile = {
		.rle_ratio = 55,
		.twobyte_ratio = 100,
		.recursive_limit = 203,
		.twobyte_threshold_max = 774,
		.twobyte_threshold_divide = 2864,
		.twobyte_threshold_min = 836,
		.seqlenMinLimit3 = 57,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 80,
		.sizeMinForSeqPack = 11866,
		.sizeMinForCanonical = 388,
		.sizeMaxForSuperslim = 47083
		},
			seqlenProfile = {
		   .rle_ratio = 13,
		   .twobyte_ratio = 74,
		   .recursive_limit = 165,
		   .twobyte_threshold_max = 5093,
		   .twobyte_threshold_divide = 3207,
		   .twobyte_threshold_min = 270,
		   .seqlenMinLimit3 = 43,
		   .blockSizeMinus = 32,
		   .sizeMaxForCanonicalHeaderPack = 166,
		   .sizeMinForSeqPack = 4173,
		   .sizeMinForCanonical = 124,
		   .sizeMaxForSuperslim = 10
		},
			offsetProfile = {
		   .rle_ratio = 51,
		   .twobyte_ratio = 99,
		   .recursive_limit = 118,
		   .twobyte_threshold_max = 13000,
		   .twobyte_threshold_divide = 2827,
		   .twobyte_threshold_min = 559,
		   .seqlenMinLimit3 = 17,
		   .blockSizeMinus = 36,
		   .sizeMaxForCanonicalHeaderPack = 148,
		   .sizeMinForSeqPack = 28347,
		   .sizeMinForCanonical = 228,
		   .sizeMaxForSuperslim = 27418
		},
			distanceProfile = {
		   .rle_ratio = 79,
		   .twobyte_ratio = 90,
		   .recursive_limit = 34,
		   .twobyte_threshold_max = 4743,
		   .twobyte_threshold_divide = 3624,
		   .twobyte_threshold_min = 69,
		   .seqlenMinLimit3 = 36,
		   .blockSizeMinus = 91,
		   .sizeMaxForCanonicalHeaderPack = 423,
		   .sizeMinForSeqPack = 12988,
		   .sizeMinForCanonical = 186,
		   .sizeMaxForSuperslim = 28605
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"dll")) {

		//Size: 3760296

		packProfile mainProfile = {
		.rle_ratio = 88,
		.twobyte_ratio = 83,
		.recursive_limit = 480,
		.twobyte_threshold_max = 10495,
		.twobyte_threshold_divide = 3607,
		.twobyte_threshold_min = 792,
		.seqlenMinLimit3 = 60,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 961,
		.sizeMinForSeqPack = 22310,
		.sizeMinForCanonical = 539,
		.sizeMaxForSuperslim = 37319
		},
			seqlenProfile = {
		   .rle_ratio = 27,
		   .twobyte_ratio = 91,
		   .recursive_limit = 698,
		   .twobyte_threshold_max = 11315,
		   .twobyte_threshold_divide = 811,
		   .twobyte_threshold_min = 37,
		   .seqlenMinLimit3 = 170,
		   .blockSizeMinus = 158,
		   .sizeMaxForCanonicalHeaderPack = 302,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 462,
		   .sizeMaxForSuperslim = 56975
		},
			offsetProfile = {
		   .rle_ratio = 79,
		   .twobyte_ratio = 72,
		   .recursive_limit = 241,
		   .twobyte_threshold_max = 10211,
		   .twobyte_threshold_divide = 1736,
		   .twobyte_threshold_min = 722,
		   .seqlenMinLimit3 = 61,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 20124,
		   .sizeMinForCanonical = 488,
		   .sizeMaxForSuperslim = 4778
		},
			distanceProfile = {
		   .rle_ratio = 35,
		   .twobyte_ratio = 68,
		   .recursive_limit = 558,
		   .twobyte_threshold_max = 6607,
		   .twobyte_threshold_divide = 2864,
		   .twobyte_threshold_min = 590,
		   .seqlenMinLimit3 = 167,
		   .blockSizeMinus = 87,
		   .sizeMaxForCanonicalHeaderPack = 621,
		   .sizeMinForSeqPack = 6912,
		   .sizeMinForCanonical = 700,
		   .sizeMaxForSuperslim = 5240
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"exe")) {

		//Size: 14693481

		packProfile mainProfile = {
		.rle_ratio = 70,
		.twobyte_ratio = 100,
		.recursive_limit = 670,
		.twobyte_threshold_max = 11333,
		.twobyte_threshold_divide = 2846,
		.twobyte_threshold_min = 860,
		.seqlenMinLimit3 = 191,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 122,
		.sizeMinForSeqPack = 8919,
		.sizeMinForCanonical = 415,
		.sizeMaxForSuperslim = 78918
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 71,
		   .recursive_limit = 411,
		   .twobyte_threshold_max = 9749,
		   .twobyte_threshold_divide = 2355,
		   .twobyte_threshold_min = 184,
		   .seqlenMinLimit3 = 251,
		   .blockSizeMinus = 161,
		   .sizeMaxForCanonicalHeaderPack = 558,
		   .sizeMinForSeqPack = 360,
		   .sizeMinForCanonical = 457,
		   .sizeMaxForSuperslim = 10
		},
			offsetProfile = {
		   .rle_ratio = 100,
		   .twobyte_ratio = 61,
		   .recursive_limit = 14,
		   .twobyte_threshold_max = 13000,
		   .twobyte_threshold_divide = 2811,
		   .twobyte_threshold_min = 853,
		   .seqlenMinLimit3 = 77,
		   .blockSizeMinus = 26,
		   .sizeMaxForCanonicalHeaderPack = 186,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 281,
		   .sizeMaxForSuperslim = 13867
		},
			distanceProfile = {
		   .rle_ratio = 22,
		   .twobyte_ratio = 55,
		   .recursive_limit = 45,
		   .twobyte_threshold_max = 6425,
		   .twobyte_threshold_divide = 1794,
		   .twobyte_threshold_min = 596,
		   .seqlenMinLimit3 = 128,
		   .blockSizeMinus = 183,
		   .sizeMaxForCanonicalHeaderPack = 727,
		   .sizeMinForSeqPack = 18309,
		   .sizeMinForCanonical = 431,
		   .sizeMaxForSuperslim = 24235
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"nrm")) {

		//Size: 22682

		packProfile mainProfile = {
		.rle_ratio = 58,
		.twobyte_ratio = 70,
		.recursive_limit = 116,
		.twobyte_threshold_max = 4161,
		.twobyte_threshold_divide = 3689,
		.twobyte_threshold_min = 770,
		.seqlenMinLimit3 = 67,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 198,
		.sizeMinForSeqPack = 22513,
		.sizeMinForCanonical = 654,
		.sizeMaxForSuperslim = 33033
		},
			seqlenProfile = {
		   .rle_ratio = 36,
		   .twobyte_ratio = 75,
		   .recursive_limit = 135,
		   .twobyte_threshold_max = 6953,
		   .twobyte_threshold_divide = 2561,
		   .twobyte_threshold_min = 14,
		   .seqlenMinLimit3 = 65,
		   .blockSizeMinus = 51,
		   .sizeMaxForCanonicalHeaderPack = 503,
		   .sizeMinForSeqPack = 3842,
		   .sizeMinForCanonical = 70,
		   .sizeMaxForSuperslim = 12557
		},
			offsetProfile = {
		   .rle_ratio = 87,
		   .twobyte_ratio = 100,
		   .recursive_limit = 108,
		   .twobyte_threshold_max = 13000,
		   .twobyte_threshold_divide = 3358,
		   .twobyte_threshold_min = 367,
		   .seqlenMinLimit3 = 126,
		   .blockSizeMinus = 12,
		   .sizeMaxForCanonicalHeaderPack = 576,
		   .sizeMinForSeqPack = 14,
		   .sizeMinForCanonical = 293,
		   .sizeMaxForSuperslim = 44360
		},
			distanceProfile = {
		   .rle_ratio = 93,
		   .twobyte_ratio = 92,
		   .recursive_limit = 10,
		   .twobyte_threshold_max = 3640,
		   .twobyte_threshold_divide = 3814,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 100,
		   .blockSizeMinus = 7,
		   .sizeMaxForCanonicalHeaderPack = 313,
		   .sizeMinForSeqPack = 6301,
		   .sizeMinForCanonical = 147,
		   .sizeMaxForSuperslim = 34464
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"pack")) {

		//Size: 365341

		packProfile mainProfile = {
		.rle_ratio = 72,
		.twobyte_ratio = 78,
		.recursive_limit = 255,
		.twobyte_threshold_max = 3094,
		.twobyte_threshold_divide = 2576,
		.twobyte_threshold_min = 824,
		.seqlenMinLimit3 = 152,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 403,
		.sizeMinForSeqPack = 10544,
		.sizeMinForCanonical = 545,
		.sizeMaxForSuperslim = 76172
		},
			seqlenProfile = {
		   .rle_ratio = 49,
		   .twobyte_ratio = 98,
		   .recursive_limit = 137,
		   .twobyte_threshold_max = 6315,
		   .twobyte_threshold_divide = 2553,
		   .twobyte_threshold_min = 168,
		   .seqlenMinLimit3 = 53,
		   .blockSizeMinus = 43,
		   .sizeMaxForCanonicalHeaderPack = 193,
		   .sizeMinForSeqPack = 26345,
		   .sizeMinForCanonical = 92,
		   .sizeMaxForSuperslim = 31692
		},
			offsetProfile = {
		   .rle_ratio = 71,
		   .twobyte_ratio = 96,
		   .recursive_limit = 33,
		   .twobyte_threshold_max = 10247,
		   .twobyte_threshold_divide = 2605,
		   .twobyte_threshold_min = 391,
		   .seqlenMinLimit3 = 79,
		   .blockSizeMinus = 25,
		   .sizeMaxForCanonicalHeaderPack = 411,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 132,
		   .sizeMaxForSuperslim = 26636
		},
			distanceProfile = {
		   .rle_ratio = 77,
		   .twobyte_ratio = 100,
		   .recursive_limit = 14,
		   .twobyte_threshold_max = 5590,
		   .twobyte_threshold_divide = 4000,
		   .twobyte_threshold_min = 42,
		   .seqlenMinLimit3 = 47,
		   .blockSizeMinus = 11,
		   .sizeMaxForCanonicalHeaderPack = 231,
		   .sizeMinForSeqPack = 3488,
		   .sizeMinForCanonical = 270,
		   .sizeMaxForSuperslim = 7652
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"pdf")) {

		//Size: 1397694

		packProfile mainProfile = {
		.rle_ratio = 74,
		.twobyte_ratio = 51,
		.recursive_limit = 485,
		.twobyte_threshold_max = 3922,
		.twobyte_threshold_divide = 2957,
		.twobyte_threshold_min = 840,
		.seqlenMinLimit3 = 226,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 486,
		.sizeMinForSeqPack = 12903,
		.sizeMinForCanonical = 338,
		.sizeMaxForSuperslim = 64070
		},
			seqlenProfile = {
		   .rle_ratio = 25,
		   .twobyte_ratio = 100,
		   .recursive_limit = 69,
		   .twobyte_threshold_max = 1358,
		   .twobyte_threshold_divide = 3178,
		   .twobyte_threshold_min = 191,
		   .seqlenMinLimit3 = 95,
		   .blockSizeMinus = 63,
		   .sizeMaxForCanonicalHeaderPack = 291,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 204,
		   .sizeMaxForSuperslim = 10
		},
			offsetProfile = {
		   .rle_ratio = 96,
		   .twobyte_ratio = 72,
		   .recursive_limit = 58,
		   .twobyte_threshold_max = 13000,
		   .twobyte_threshold_divide = 1130,
		   .twobyte_threshold_min = 616,
		   .seqlenMinLimit3 = 113,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 651,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 370,
		   .sizeMaxForSuperslim = 10434
		},
			distanceProfile = {
		   .rle_ratio = 87,
		   .twobyte_ratio = 94,
		   .recursive_limit = 42,
		   .twobyte_threshold_max = 7302,
		   .twobyte_threshold_divide = 3253,
		   .twobyte_threshold_min = 525,
		   .seqlenMinLimit3 = 49,
		   .blockSizeMinus = 44,
		   .sizeMaxForCanonicalHeaderPack = 338,
		   .sizeMinForSeqPack = 6525,
		   .sizeMinForCanonical = 465,
		   .sizeMaxForSuperslim = 10
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}


	// -------------------------------------------------------------------------
	// Default profile for the rest
	// -------------------------------------------------------------------------

	return def;

}


completePackProfile getProfileForExtension(wchar_t* ext) {


	//Size: 39029.080

	packProfile mainProfile = {
.rle_ratio = 95,
.twobyte_ratio = 84,
.recursive_limit = 274,
.twobyte_threshold_max = 4062,
.twobyte_threshold_divide = 2429,
.twobyte_threshold_min = 789,
.seqlenMinLimit3 = 75,
.blockSizeMinus = 0,
.sizeMaxForCanonicalHeaderPack = 186,
.sizeMinForSeqPack = 10,
.sizeMinForCanonical = 513,
.sizeMaxForSuperslim = 62224
	},
		seqlenProfile = {
	   .rle_ratio = 22,
	   .twobyte_ratio = 97,
	   .recursive_limit = 141,
	   .twobyte_threshold_max = 4552,
	   .twobyte_threshold_divide = 2213,
	   .twobyte_threshold_min = 139,
	   .seqlenMinLimit3 = 35,
	   .blockSizeMinus = 0,
	   .sizeMaxForCanonicalHeaderPack = 126,
	   .sizeMinForSeqPack = 10,
	   .sizeMinForCanonical = 43,
	   .sizeMaxForSuperslim = 30152
	},
		offsetProfile = {
	   .rle_ratio = 64,
	   .twobyte_ratio = 93,
	   .recursive_limit = 25,
	   .twobyte_threshold_max = 11012,
	   .twobyte_threshold_divide = 2910,
	   .twobyte_threshold_min = 511,
	   .seqlenMinLimit3 = 116,
	   .blockSizeMinus = 0,
	   .sizeMaxForCanonicalHeaderPack = 530,
	   .sizeMinForSeqPack = 10,
	   .sizeMinForCanonical = 329,
	   .sizeMaxForSuperslim = 5049
	},
		distanceProfile = {
	   .rle_ratio = 76,
	   .twobyte_ratio = 93,
	   .recursive_limit = 10,
	   .twobyte_threshold_max = 3641,
	   .twobyte_threshold_divide = 3792,
	   .twobyte_threshold_min = 3,
	   .seqlenMinLimit3 = 74,
	   .blockSizeMinus = 0,
	   .sizeMaxForCanonicalHeaderPack = 337,
	   .sizeMinForSeqPack = 10,
	   .sizeMinForCanonical = 353,
	   .sizeMaxForSuperslim = 21580
	};



	completePackProfile prof = getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);

	return getProfileForExtensionOrDefault(ext, prof);

}