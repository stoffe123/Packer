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

 //Size: 78.956

 packProfile mainProfile = {
 .rle_ratio = 99,
 .twobyte_ratio = 93,
 .recursive_limit = 259,
 .twobyte_threshold_max = 4998,
 .twobyte_threshold_divide = 3846,
 .twobyte_threshold_min = 982,
 .seqlenMinLimit3 = 170,
 .seqlenMinLimit4 = 48299,
 .blockSizeMinus = 247,
 .sizeMaxForCanonicalHeaderPack = 193,
 .sizeMinForSeqPack = 20378,
 .sizeMinForCanonical = 414,
 .sizeMaxForSuperslim = 49338,
 .metaCompressionFactor = 70,
 .offsetLimit1 = 148,
 .offsetLimit2 = 1852,
 .offsetLimit3 = 67136,
 .bytesWonMin = -75
 },
	 seqlenProfile = {
	.rle_ratio = 10,
	.twobyte_ratio = 75,
	.recursive_limit = 653,
	.twobyte_threshold_max = 12084,
	.twobyte_threshold_divide = 2355,
	.twobyte_threshold_min = 66,
	.seqlenMinLimit3 = 255,
	.seqlenMinLimit4 = 31818,
	.blockSizeMinus = 63,
	.sizeMaxForCanonicalHeaderPack = 670,
	.sizeMinForSeqPack = 16315,
	.sizeMinForCanonical = 12,
	.sizeMaxForSuperslim = 16050,
	.metaCompressionFactor = 171,
	.offsetLimit1 = 229,
	.offsetLimit2 = 1006,
	.offsetLimit3 = 79337,
	.bytesWonMin = 154
 },
	 offsetProfile = {
	.rle_ratio = 78,
	.twobyte_ratio = 89,
	.recursive_limit = 28,
	.twobyte_threshold_max = 8082,
	.twobyte_threshold_divide = 1342,
	.twobyte_threshold_min = 1000,
	.seqlenMinLimit3 = 35,
	.seqlenMinLimit4 = 87010,
	.blockSizeMinus = 346,
	.sizeMaxForCanonicalHeaderPack = 488,
	.sizeMinForSeqPack = 27517,
	.sizeMinForCanonical = 324,
	.sizeMaxForSuperslim = 9167,
	.metaCompressionFactor = 19,
	.offsetLimit1 = 166,
	.offsetLimit2 = 827,
	.offsetLimit3 = 64704,
	.bytesWonMin = -314
 },
	 distanceProfile = {
	.rle_ratio = 66,
	.twobyte_ratio = 85,
	.recursive_limit = 105,
	.twobyte_threshold_max = 7445,
	.twobyte_threshold_divide = 2553,
	.twobyte_threshold_min = 1000,
	.seqlenMinLimit3 = 198,
	.seqlenMinLimit4 = 32569,
	.blockSizeMinus = 45,
	.sizeMaxForCanonicalHeaderPack = 556,
	.sizeMinForSeqPack = 48179,
	.sizeMinForCanonical = 652,
	.sizeMaxForSuperslim = 2280,
	.metaCompressionFactor = 90,
	.offsetLimit1 = 197,
	.offsetLimit2 = 1022,
	.offsetLimit3 = 70954,
	.bytesWonMin = -93
 };
 return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
}

	else if (equalsIgnoreCase(ext, L"txt")) {

		//Size: 144.301

		packProfile mainProfile = {
		.rle_ratio = 76,
		.twobyte_ratio = 78,
		.recursive_limit = 35,
		.twobyte_threshold_max = 3818,
		.twobyte_threshold_divide = 3246,
		.twobyte_threshold_min = 906,
		.seqlenMinLimit3 = 155,
		.blockSizeMinus = 370,
		.sizeMaxForCanonicalHeaderPack = 436,
		.sizeMinForSeqPack = 12481,
		.sizeMinForCanonical = 285,
		.sizeMaxForSuperslim = 40233,
		.metaCompressionFactor = 133,
		.offsetLimit1 = 173,
		.offsetLimit2 = 1982,
		.offsetLimit3 = 60073,
		.bytesWonMin = -11
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 78,
		   .recursive_limit = 548,
		   .twobyte_threshold_max = 9535,
		   .twobyte_threshold_divide = 1369,
		   .twobyte_threshold_min = 467,
		   .seqlenMinLimit3 = 159,
		   .blockSizeMinus = 117,
		   .sizeMaxForCanonicalHeaderPack = 139,
		   .sizeMinForSeqPack = 22665,
		   .sizeMinForCanonical = 695,
		   .sizeMaxForSuperslim = 91775,
		   .metaCompressionFactor = 133,
		   .offsetLimit1 = 224,
		   .offsetLimit2 = 1242,
		   .offsetLimit3 = 65807,
		   .bytesWonMin = 135
		},
			offsetProfile = {
		   .rle_ratio = 83,
		   .twobyte_ratio = 100,
		   .recursive_limit = 492,
		   .twobyte_threshold_max = 8220,
		   .twobyte_threshold_divide = 3741,
		   .twobyte_threshold_min = 958,
		   .seqlenMinLimit3 = 191,
		   .blockSizeMinus = 79,
		   .sizeMaxForCanonicalHeaderPack = 331,
		   .sizeMinForSeqPack = 295,
		   .sizeMinForCanonical = 420,
		   .sizeMaxForSuperslim = 83987,
		   .metaCompressionFactor = 87,
		   .offsetLimit1 = 224,
		   .offsetLimit2 = 1593,
		   .offsetLimit3 = 68577,
		   .bytesWonMin = -116
		},
			distanceProfile = {
		   .rle_ratio = 51,
		   .twobyte_ratio = 72,
		   .recursive_limit = 685,
		   .twobyte_threshold_max = 11970,
		   .twobyte_threshold_divide = 2587,
		   .twobyte_threshold_min = 561,
		   .seqlenMinLimit3 = 146,
		   .blockSizeMinus = 234,
		   .sizeMaxForCanonicalHeaderPack = 988,
		   .sizeMinForSeqPack = 4456,
		   .sizeMinForCanonical = 700,
		   .sizeMaxForSuperslim = 35015,
		   .metaCompressionFactor = 116,
		   .offsetLimit1 = 122,
		   .offsetLimit2 = 1751,
		   .offsetLimit3 = 79408,
		   .bytesWonMin = -164
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"doc")) {

		//Size: 691.455

		packProfile mainProfile = {
		.rle_ratio = 61,
		.twobyte_ratio = 35,
		.recursive_limit = 538,
		.twobyte_threshold_max = 11046,
		.twobyte_threshold_divide = 1944,
		.twobyte_threshold_min = 979,
		.seqlenMinLimit3 = 255,
		.blockSizeMinus = 101,
		.sizeMaxForCanonicalHeaderPack = 80,
		.sizeMinForSeqPack = 9201,
		.sizeMinForCanonical = 147,
		.sizeMaxForSuperslim = 7263,
		.metaCompressionFactor = 101,
		.offsetLimit1 = 125,
		.offsetLimit2 = 1610,
		.offsetLimit3 = 61839,
		.bytesWonMin = 22
		},
			seqlenProfile = {
		   .rle_ratio = 28,
		   .twobyte_ratio = 98,
		   .recursive_limit = 457,
		   .twobyte_threshold_max = 9335,
		   .twobyte_threshold_divide = 2819,
		   .twobyte_threshold_min = 258,
		   .seqlenMinLimit3 = 192,
		   .blockSizeMinus = 456,
		   .sizeMaxForCanonicalHeaderPack = 697,
		   .sizeMinForSeqPack = 10557,
		   .sizeMinForCanonical = 388,
		   .sizeMaxForSuperslim = 13903,
		   .metaCompressionFactor = 176,
		   .offsetLimit1 = 226,
		   .offsetLimit2 = 1830,
		   .offsetLimit3 = 68058,
		   .bytesWonMin = 69
		},
			offsetProfile = {
		   .rle_ratio = 42,
		   .twobyte_ratio = 87,
		   .recursive_limit = 359,
		   .twobyte_threshold_max = 11328,
		   .twobyte_threshold_divide = 110,
		   .twobyte_threshold_min = 820,
		   .seqlenMinLimit3 = 125,
		   .blockSizeMinus = 249,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 14509,
		   .sizeMinForCanonical = 654,
		   .sizeMaxForSuperslim = 25313,
		   .metaCompressionFactor = 128,
		   .offsetLimit1 = 168,
		   .offsetLimit2 = 1921,
		   .offsetLimit3 = 77592,
		   .bytesWonMin = 121
		},
			distanceProfile = {
		   .rle_ratio = 25,
		   .twobyte_ratio = 82,
		   .recursive_limit = 37,
		   .twobyte_threshold_max = 4477,
		   .twobyte_threshold_divide = 3921,
		   .twobyte_threshold_min = 1000,
		   .seqlenMinLimit3 = 199,
		   .blockSizeMinus = 78,
		   .sizeMaxForCanonicalHeaderPack = 583,
		   .sizeMinForSeqPack = 23910,
		   .sizeMinForCanonical = 534,
		   .sizeMaxForSuperslim = 21233,
		   .metaCompressionFactor = 126,
		   .offsetLimit1 = 133,
		   .offsetLimit2 = 1468,
		   .offsetLimit3 = 69201,
		   .bytesWonMin = 42
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
		.sizeMaxForSuperslim = 38323,
		.metaCompressionFactor = 108,
	.offsetLimit1 = 139,
	.offsetLimit2 = 1555,
	.offsetLimit3 = 62742
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
		   .sizeMaxForSuperslim = 32995,
		   .metaCompressionFactor = 108,
	.offsetLimit1 = 139,
	.offsetLimit2 = 1555,
	.offsetLimit3 = 62742
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
		   .sizeMaxForSuperslim = 11805,
		   .metaCompressionFactor = 108,
	.offsetLimit1 = 139,
	.offsetLimit2 = 1555,
	.offsetLimit3 = 62742
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
		   .sizeMaxForSuperslim = 7911,
		   .metaCompressionFactor = 108,
	.offsetLimit1 = 139,
	.offsetLimit2 = 1555,
	.offsetLimit3 = 62742
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"cs")) {

		//Size: 28.953

		packProfile mainProfile = {
		.rle_ratio = 82,
		.twobyte_ratio = 100,
		.recursive_limit = 535,
		.twobyte_threshold_max = 11059,
		.twobyte_threshold_divide = 3450,
		.twobyte_threshold_min = 918,
		.seqlenMinLimit3 = 67,
		.blockSizeMinus = 339,
		.sizeMaxForCanonicalHeaderPack = 271,
		.sizeMinForSeqPack = 56122,
		.sizeMinForCanonical = 478,
		.sizeMaxForSuperslim = 91239,
		.metaCompressionFactor = 123,
		.offsetLimit1 = 256,
		.offsetLimit2 = 833,
		.offsetLimit3 = 61927,
		.bytesWonMin = 28
		},
			seqlenProfile = {
		   .rle_ratio = 28,
		   .twobyte_ratio = 90,
		   .recursive_limit = 568,
		   .twobyte_threshold_max = 7349,
		   .twobyte_threshold_divide = 2304,
		   .twobyte_threshold_min = 637,
		   .seqlenMinLimit3 = 179,
		   .blockSizeMinus = 1000,
		   .sizeMaxForCanonicalHeaderPack = 475,
		   .sizeMinForSeqPack = 27273,
		   .sizeMinForCanonical = 317,
		   .sizeMaxForSuperslim = 12420,
		   .metaCompressionFactor = 27,
		   .offsetLimit1 = 125,
		   .offsetLimit2 = 1331,
		   .offsetLimit3 = 71877,
		   .bytesWonMin = 15
		},
			offsetProfile = {
		   .rle_ratio = 72,
		   .twobyte_ratio = 34,
		   .recursive_limit = 449,
		   .twobyte_threshold_max = 5920,
		   .twobyte_threshold_divide = 523,
		   .twobyte_threshold_min = 579,
		   .seqlenMinLimit3 = 89,
		   .blockSizeMinus = 29,
		   .sizeMaxForCanonicalHeaderPack = 290,
		   .sizeMinForSeqPack = 70453,
		   .sizeMinForCanonical = 187,
		   .sizeMaxForSuperslim = 33007,
		   .metaCompressionFactor = 24,
		   .offsetLimit1 = 194,
		   .offsetLimit2 = 1729,
		   .offsetLimit3 = 60305,
		   .bytesWonMin = 26
		},
			distanceProfile = {
		   .rle_ratio = 32,
		   .twobyte_ratio = 98,
		   .recursive_limit = 337,
		   .twobyte_threshold_max = 9078,
		   .twobyte_threshold_divide = 3195,
		   .twobyte_threshold_min = 785,
		   .seqlenMinLimit3 = 210,
		   .blockSizeMinus = 242,
		   .sizeMaxForCanonicalHeaderPack = 309,
		   .sizeMinForSeqPack = 5982,
		   .sizeMinForCanonical = 621,
		   .sizeMaxForSuperslim = 13771,
		   .metaCompressionFactor = 37,
		   .offsetLimit1 = 194,
		   .offsetLimit2 = 1222,
		   .offsetLimit3 = 70868,
		   .bytesWonMin = 4
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"prx")) {

		//Size: 5755.012

		packProfile mainProfile = {
		.rle_ratio = 77,
		.twobyte_ratio = 69,
		.recursive_limit = 325,
		.twobyte_threshold_max = 11387,
		.twobyte_threshold_divide = 950,
		.twobyte_threshold_min = 692,
		.seqlenMinLimit3 = 103,
		.blockSizeMinus = 203,
		.sizeMaxForCanonicalHeaderPack = 864,
		.sizeMinForSeqPack = 810,
		.sizeMinForCanonical = 660,
		.sizeMaxForSuperslim = 97677,
		.metaCompressionFactor = 180,
		.offsetLimit1 = 238,
		.offsetLimit2 = 1835,
		.offsetLimit3 = 60000,
		.bytesWonMin = 2
		},
			seqlenProfile = {
		   .rle_ratio = 21,
		   .twobyte_ratio = 61,
		   .recursive_limit = 512,
		   .twobyte_threshold_max = 10321,
		   .twobyte_threshold_divide = 1469,
		   .twobyte_threshold_min = 18,
		   .seqlenMinLimit3 = 157,
		   .blockSizeMinus = 78,
		   .sizeMaxForCanonicalHeaderPack = 265,
		   .sizeMinForSeqPack = 9361,
		   .sizeMinForCanonical = 127,
		   .sizeMaxForSuperslim = 34822,
		   .metaCompressionFactor = 76,
		   .offsetLimit1 = 226,
		   .offsetLimit2 = 1557,
		   .offsetLimit3 = 60284,
		   .bytesWonMin = 2
		},
			offsetProfile = {
		   .rle_ratio = 66,
		   .twobyte_ratio = 79,
		   .recursive_limit = 148,
		   .twobyte_threshold_max = 12519,
		   .twobyte_threshold_divide = 2006,
		   .twobyte_threshold_min = 755,
		   .seqlenMinLimit3 = 79,
		   .blockSizeMinus = 219,
		   .sizeMaxForCanonicalHeaderPack = 637,
		   .sizeMinForSeqPack = 1232,
		   .sizeMinForCanonical = 10,
		   .sizeMaxForSuperslim = 10120,
		   .metaCompressionFactor = 64,
		   .offsetLimit1 = 218,
		   .offsetLimit2 = 700,
		   .offsetLimit3 = 68255,
		   .bytesWonMin = 159
		},
			distanceProfile = {
		   .rle_ratio = 44,
		   .twobyte_ratio = 37,
		   .recursive_limit = 217,
		   .twobyte_threshold_max = 482,
		   .twobyte_threshold_divide = 3482,
		   .twobyte_threshold_min = 890,
		   .seqlenMinLimit3 = 255,
		   .blockSizeMinus = 291,
		   .sizeMaxForCanonicalHeaderPack = 567,
		   .sizeMinForSeqPack = 17084,
		   .sizeMinForCanonical = 560,
		   .sizeMaxForSuperslim = 26080,
		   .metaCompressionFactor = 102,
		   .offsetLimit1 = 207,
		   .offsetLimit2 = 772,
		   .offsetLimit3 = 68091,
		   .bytesWonMin = 133
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"mp3")) {

		//Size: 2135.535

		packProfile mainProfile = {
		.rle_ratio = 40,
		.twobyte_ratio = 73,
		.recursive_limit = 626,
		.twobyte_threshold_max = 10447,
		.twobyte_threshold_divide = 3520,
		.twobyte_threshold_min = 678,
		.seqlenMinLimit3 = 0,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 156,
		.sizeMinForSeqPack = 32464,
		.sizeMinForCanonical = 673,
		.sizeMaxForSuperslim = 63842,
		.metaCompressionFactor = 31,
		.offsetLimit1 = 137,
		.offsetLimit2 = 1081,
		.offsetLimit3 = 63785,
		.bytesWonMin = 48
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 72,
		   .recursive_limit = 547,
		   .twobyte_threshold_max = 9898,
		   .twobyte_threshold_divide = 2103,
		   .twobyte_threshold_min = 15,
		   .seqlenMinLimit3 = 235,
		   .blockSizeMinus = 26,
		   .sizeMaxForCanonicalHeaderPack = 675,
		   .sizeMinForSeqPack = 664,
		   .sizeMinForCanonical = 292,
		   .sizeMaxForSuperslim = 28364,
		   .metaCompressionFactor = 1,
		   .offsetLimit1 = 90,
		   .offsetLimit2 = 700,
		   .offsetLimit3 = 62667,
		   .bytesWonMin = 4
		},
			offsetProfile = {
		   .rle_ratio = 14,
		   .twobyte_ratio = 49,
		   .recursive_limit = 495,
		   .twobyte_threshold_max = 11694,
		   .twobyte_threshold_divide = 1189,
		   .twobyte_threshold_min = 913,
		   .seqlenMinLimit3 = 67,
		   .blockSizeMinus = 401,
		   .sizeMaxForCanonicalHeaderPack = 245,
		   .sizeMinForSeqPack = 10075,
		   .sizeMinForCanonical = 318,
		   .sizeMaxForSuperslim = 10914,
		   .metaCompressionFactor = 6,
		   .offsetLimit1 = 107,
		   .offsetLimit2 = 1229,
		   .offsetLimit3 = 63556,
		   .bytesWonMin = 24
		},
			distanceProfile = {
		   .rle_ratio = 26,
		   .twobyte_ratio = 96,
		   .recursive_limit = 10,
		   .twobyte_threshold_max = 13000,
		   .twobyte_threshold_divide = 1749,
		   .twobyte_threshold_min = 688,
		   .seqlenMinLimit3 = 125,
		   .blockSizeMinus = 407,
		   .sizeMaxForCanonicalHeaderPack = 917,
		   .sizeMinForSeqPack = 13405,
		   .sizeMinForCanonical = 700,
		   .sizeMaxForSuperslim = 25954,
		   .metaCompressionFactor = 56,
		   .offsetLimit1 = 147,
		   .offsetLimit2 = 888,
		   .offsetLimit3 = 61877,
		   .bytesWonMin = 6
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"resx")) {

		//Size: 2843.073

		packProfile mainProfile = {
		.rle_ratio = 100,
		.twobyte_ratio = 100,
		.recursive_limit = 393,
		.twobyte_threshold_max = 4191,
		.twobyte_threshold_divide = 1354,
		.twobyte_threshold_min = 1000,
		.seqlenMinLimit3 = 0,
		.blockSizeMinus = 99,
		.sizeMaxForCanonicalHeaderPack = 884,
		.sizeMinForSeqPack = 23071,
		.sizeMinForCanonical = 290,
		.sizeMaxForSuperslim = 36167,
		.metaCompressionFactor = 68,
		.offsetLimit1 = 93,
		.offsetLimit2 = 829,
		.offsetLimit3 = 61040,
		.bytesWonMin = 61
		},
			seqlenProfile = {
		   .rle_ratio = 33,
		   .twobyte_ratio = 100,
		   .recursive_limit = 568,
		   .twobyte_threshold_max = 10872,
		   .twobyte_threshold_divide = 2161,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 255,
		   .blockSizeMinus = 180,
		   .sizeMaxForCanonicalHeaderPack = 373,
		   .sizeMinForSeqPack = 11925,
		   .sizeMinForCanonical = 412,
		   .sizeMaxForSuperslim = 34415,
		   .metaCompressionFactor = 47,
		   .offsetLimit1 = 96,
		   .offsetLimit2 = 700,
		   .offsetLimit3 = 61012,
		   .bytesWonMin = -3
		},
			offsetProfile = {
		   .rle_ratio = 88,
		   .twobyte_ratio = 77,
		   .recursive_limit = 369,
		   .twobyte_threshold_max = 12565,
		   .twobyte_threshold_divide = 730,
		   .twobyte_threshold_min = 864,
		   .seqlenMinLimit3 = 67,
		   .blockSizeMinus = 53,
		   .sizeMaxForCanonicalHeaderPack = 167,
		   .sizeMinForSeqPack = 54469,
		   .sizeMinForCanonical = 426,
		   .sizeMaxForSuperslim = 1411,
		   .metaCompressionFactor = 36,
		   .offsetLimit1 = 95,
		   .offsetLimit2 = 772,
		   .offsetLimit3 = 61448,
		   .bytesWonMin = 5
		},
			distanceProfile = {
		   .rle_ratio = 16,
		   .twobyte_ratio = 97,
		   .recursive_limit = 287,
		   .twobyte_threshold_max = 7245,
		   .twobyte_threshold_divide = 3118,
		   .twobyte_threshold_min = 993,
		   .seqlenMinLimit3 = 255,
		   .blockSizeMinus = 305,
		   .sizeMaxForCanonicalHeaderPack = 377,
		   .sizeMinForSeqPack = 18755,
		   .sizeMinForCanonical = 451,
		   .sizeMaxForSuperslim = 10,
		   .metaCompressionFactor = 50,
		   .offsetLimit1 = 102,
		   .offsetLimit2 = 713,
		   .offsetLimit3 = 61780,
		   .bytesWonMin = -8
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"fdt")) {

	//Size: 1642.110

	packProfile mainProfile = {
	.rle_ratio = 84,
	.twobyte_ratio = 82,
	.recursive_limit = 109,
	.twobyte_threshold_max = 2457,
	.twobyte_threshold_divide = 770,
	.twobyte_threshold_min = 946,
	.seqlenMinLimit3 = 150,
	.seqlenMinLimit4 = 65000,
	.blockSizeMinus = 129,
	.sizeMaxForCanonicalHeaderPack = 614,
	.sizeMinForSeqPack = 3137,
	.sizeMinForCanonical = 700,
	.sizeMaxForSuperslim = 4317,
	.metaCompressionFactor = 70,
	.offsetLimit1 = 200,
	.offsetLimit2 = 1100,
	.offsetLimit3 = 66000,
	.bytesWonMin = 0
	},
		seqlenProfile = {
	   .rle_ratio = 23,
	   .twobyte_ratio = 82,
	   .recursive_limit = 315,
	   .twobyte_threshold_max = 2330,
	   .twobyte_threshold_divide = 1909,
	   .twobyte_threshold_min = 365,
	   .seqlenMinLimit3 = 150,
	   .seqlenMinLimit4 = 65000,
	   .blockSizeMinus = 151,
	   .sizeMaxForCanonicalHeaderPack = 247,
	   .sizeMinForSeqPack = 18572,
	   .sizeMinForCanonical = 90,
	   .sizeMaxForSuperslim = 41388,
	   .metaCompressionFactor = 50,
	   .offsetLimit1 = 150,
	   .offsetLimit2 = 1000,
	   .offsetLimit3 = 66000,
	   .bytesWonMin = 0
	},
		offsetProfile = {
	   .rle_ratio = 62,
	   .twobyte_ratio = 51,
	   .recursive_limit = 88,
	   .twobyte_threshold_max = 9825,
	   .twobyte_threshold_divide = 3455,
	   .twobyte_threshold_min = 378,
	   .seqlenMinLimit3 = 150,
	   .seqlenMinLimit4 = 65000,
	   .blockSizeMinus = 68,
	   .sizeMaxForCanonicalHeaderPack = 413,
	   .sizeMinForSeqPack = 6165,
	   .sizeMinForCanonical = 289,
	   .sizeMaxForSuperslim = 34275,
	   .metaCompressionFactor = 50,
	   .offsetLimit1 = 150,
	   .offsetLimit2 = 1000,
	   .offsetLimit3 = 66000,
	   .bytesWonMin = 0
	},
		distanceProfile = {
	   .rle_ratio = 95,
	   .twobyte_ratio = 61,
	   .recursive_limit = 279,
	   .twobyte_threshold_max = 648,
	   .twobyte_threshold_divide = 2773,
	   .twobyte_threshold_min = 3,
	   .seqlenMinLimit3 = 150,
	   .seqlenMinLimit4 = 65000,
	   .blockSizeMinus = 269,
	   .sizeMaxForCanonicalHeaderPack = 80,
	   .sizeMinForSeqPack = 14690,
	   .sizeMinForCanonical = 101,
	   .sizeMaxForSuperslim = 18452,
	   .metaCompressionFactor = 50,
	   .offsetLimit1 = 150,
	   .offsetLimit2 = 1000,
	   .offsetLimit3 = 66000,
	   .bytesWonMin = 0
	};
	return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
}

	else if (equalsIgnoreCase(ext, L"xml")) {

		//Size: 478.065

		packProfile mainProfile = {
		.rle_ratio = 89,
		.twobyte_ratio = 80,
		.recursive_limit = 194,
		.twobyte_threshold_max = 10416,
		.twobyte_threshold_divide = 20,
		.twobyte_threshold_min = 853,
		.seqlenMinLimit3 = 248,
		.blockSizeMinus = 92,
		.sizeMaxForCanonicalHeaderPack = 324,
		.sizeMinForSeqPack = 12472,
		.sizeMinForCanonical = 410,
		.sizeMaxForSuperslim = 88795,
		.metaCompressionFactor = 70,
		.offsetLimit1 = 240,
		.offsetLimit2 = 995,
		.offsetLimit3 = 67763
		},
			seqlenProfile = {
		   .rle_ratio = 10,
		   .twobyte_ratio = 70,
		   .recursive_limit = 206,
		   .twobyte_threshold_max = 3767,
		   .twobyte_threshold_divide = 3557,
		   .twobyte_threshold_min = 386,
		   .seqlenMinLimit3 = 61,
		   .blockSizeMinus = 14,
		   .sizeMaxForCanonicalHeaderPack = 232,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 44,
		   .sizeMaxForSuperslim = 24951,
		   .metaCompressionFactor = 49,
		   .offsetLimit1 = 178,
		   .offsetLimit2 = 1228,
		   .offsetLimit3 = 65960
		},
			offsetProfile = {
		   .rle_ratio = 98,
		   .twobyte_ratio = 97,
		   .recursive_limit = 10,
		   .twobyte_threshold_max = 10917,
		   .twobyte_threshold_divide = 2821,
		   .twobyte_threshold_min = 487,
		   .seqlenMinLimit3 = 54,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 327,
		   .sizeMinForSeqPack = 7391,
		   .sizeMinForCanonical = 370,
		   .sizeMaxForSuperslim = 32518,
		   .metaCompressionFactor = 59,
		   .offsetLimit1 = 228,
		   .offsetLimit2 = 901,
		   .offsetLimit3 = 66492
		},
			distanceProfile = {
		   .rle_ratio = 75,
		   .twobyte_ratio = 97,
		   .recursive_limit = 13,
		   .twobyte_threshold_max = 248,
		   .twobyte_threshold_divide = 4000,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 2,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 295,
		   .sizeMinForSeqPack = 25636,
		   .sizeMinForCanonical = 224,
		   .sizeMaxForSuperslim = 20693,
		   .metaCompressionFactor = 68,
		   .offsetLimit1 = 212,
		   .offsetLimit2 = 1196,
		   .offsetLimit3 = 66461
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"tis")) {

		//Size: 17290.850

		packProfile mainProfile = {
		.rle_ratio = 25,
		.twobyte_ratio = 80,
		.recursive_limit = 700,
		.twobyte_threshold_max = 1231,
		.twobyte_threshold_divide = 2888,
		.twobyte_threshold_min = 909,
		.seqlenMinLimit3 = 255,
		.blockSizeMinus = 887,
		.sizeMaxForCanonicalHeaderPack = 80,
		.sizeMinForSeqPack = 38005,
		.sizeMinForCanonical = 336,
		.sizeMaxForSuperslim = 83510,
		.metaCompressionFactor = 40,
		.offsetLimit1 = 245,
		.offsetLimit2 = 1153,
		.offsetLimit3 = 71954,
		.bytesWonMin = -198
		},
			seqlenProfile = {
		   .rle_ratio = 61,
		   .twobyte_ratio = 84,
		   .recursive_limit = 279,
		   .twobyte_threshold_max = 4295,
		   .twobyte_threshold_divide = 1125,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 139,
		   .blockSizeMinus = 406,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 10142,
		   .sizeMinForCanonical = 72,
		   .sizeMaxForSuperslim = 1906,
		   .metaCompressionFactor = 53,
		   .offsetLimit1 = 191,
		   .offsetLimit2 = 733,
		   .offsetLimit3 = 69298,
		   .bytesWonMin = -199
		},
			offsetProfile = {
		   .rle_ratio = 85,
		   .twobyte_ratio = 83,
		   .recursive_limit = 78,
		   .twobyte_threshold_max = 3,
		   .twobyte_threshold_divide = 2913,
		   .twobyte_threshold_min = 432,
		   .seqlenMinLimit3 = 67,
		   .blockSizeMinus = 1000,
		   .sizeMaxForCanonicalHeaderPack = 591,
		   .sizeMinForSeqPack = 34585,
		   .sizeMinForCanonical = 10,
		   .sizeMaxForSuperslim = 10,
		   .metaCompressionFactor = 50,
		   .offsetLimit1 = 238,
		   .offsetLimit2 = 779,
		   .offsetLimit3 = 69465,
		   .bytesWonMin = -199
		},
			distanceProfile = {
		   .rle_ratio = 83,
		   .twobyte_ratio = 42,
		   .recursive_limit = 236,
		   .twobyte_threshold_max = 9509,
		   .twobyte_threshold_divide = 3642,
		   .twobyte_threshold_min = 391,
		   .seqlenMinLimit3 = 19,
		   .blockSizeMinus = 54,
		   .sizeMaxForCanonicalHeaderPack = 1057,
		   .sizeMinForSeqPack = 64591,
		   .sizeMinForCanonical = 655,
		   .sizeMaxForSuperslim = 7295,
		   .metaCompressionFactor = 43,
		   .offsetLimit1 = 199,
		   .offsetLimit2 = 878,
		   .offsetLimit3 = 73641,
		   .bytesWonMin = -199
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"mark")) {

		//Size: 51.914

		packProfile mainProfile = {
		.rle_ratio = 57,
		.twobyte_ratio = 91,
		.recursive_limit = 246,
		.twobyte_threshold_max = 12276,
		.twobyte_threshold_divide = 503,
		.twobyte_threshold_min = 1000,
		.seqlenMinLimit3 = 132,
		.blockSizeMinus = 317,
		.sizeMaxForCanonicalHeaderPack = 187,
		.sizeMinForSeqPack = 30163,
		.sizeMinForCanonical = 505,
		.sizeMaxForSuperslim = 97975,
		.metaCompressionFactor = 102,
		.offsetLimit1 = 217,
		.offsetLimit2 = 700,
		.offsetLimit3 = 63655
		},
			seqlenProfile = {
		   .rle_ratio = 49,
		   .twobyte_ratio = 61,
		   .recursive_limit = 113,
		   .twobyte_threshold_max = 4101,
		   .twobyte_threshold_divide = 3042,
		   .twobyte_threshold_min = 676,
		   .seqlenMinLimit3 = 26,
		   .blockSizeMinus = 258,
		   .sizeMaxForCanonicalHeaderPack = 288,
		   .sizeMinForSeqPack = 23950,
		   .sizeMinForCanonical = 102,
		   .sizeMaxForSuperslim = 15990,
		   .metaCompressionFactor = 73,
		   .offsetLimit1 = 198,
		   .offsetLimit2 = 894,
		   .offsetLimit3 = 63242
		},
			offsetProfile = {
		   .rle_ratio = 53,
		   .twobyte_ratio = 72,
		   .recursive_limit = 42,
		   .twobyte_threshold_max = 10431,
		   .twobyte_threshold_divide = 2789,
		   .twobyte_threshold_min = 317,
		   .seqlenMinLimit3 = 132,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 880,
		   .sizeMinForSeqPack = 5508,
		   .sizeMinForCanonical = 57,
		   .sizeMaxForSuperslim = 10,
		   .metaCompressionFactor = 68,
		   .offsetLimit1 = 227,
		   .offsetLimit2 = 1003,
		   .offsetLimit3 = 61966
		},
			distanceProfile = {
		   .rle_ratio = 94,
		   .twobyte_ratio = 84,
		   .recursive_limit = 433,
		   .twobyte_threshold_max = 1086,
		   .twobyte_threshold_divide = 2262,
		   .twobyte_threshold_min = 510,
		   .seqlenMinLimit3 = 14,
		   .blockSizeMinus = 105,
		   .sizeMaxForCanonicalHeaderPack = 960,
		   .sizeMinForSeqPack = 10177,
		   .sizeMinForCanonical = 373,
		   .sizeMaxForSuperslim = 65928,
		   .metaCompressionFactor = 31,
		   .offsetLimit1 = 181,
		   .offsetLimit2 = 829,
		   .offsetLimit3 = 61726
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"csh")) {

		//Size: 672.258

		packProfile mainProfile = {
		.rle_ratio = 64,
		.twobyte_ratio = 100,
		.recursive_limit = 166,
		.twobyte_threshold_max = 9084,
		.twobyte_threshold_divide = 2360,
		.twobyte_threshold_min = 679,
		.seqlenMinLimit3 = 0,
		.blockSizeMinus = 258,
		.sizeMaxForCanonicalHeaderPack = 293,
		.sizeMinForSeqPack = 28958,
		.sizeMinForCanonical = 543,
		.sizeMaxForSuperslim = 71218,
		.metaCompressionFactor = 120,
		.offsetLimit1 = 256,
		.offsetLimit2 = 700,
		.offsetLimit3 = 60000
		},
			seqlenProfile = {
		   .rle_ratio = 11,
		   .twobyte_ratio = 75,
		   .recursive_limit = 201,
		   .twobyte_threshold_max = 3036,
		   .twobyte_threshold_divide = 3614,
		   .twobyte_threshold_min = 15,
		   .seqlenMinLimit3 = 100,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 768,
		   .sizeMinForSeqPack = 56114,
		   .sizeMinForCanonical = 77,
		   .sizeMaxForSuperslim = 45678,
		   .metaCompressionFactor = 105,
		   .offsetLimit1 = 171,
		   .offsetLimit2 = 759,
		   .offsetLimit3 = 63911
		},
			offsetProfile = {
		   .rle_ratio = 61,
		   .twobyte_ratio = 70,
		   .recursive_limit = 129,
		   .twobyte_threshold_max = 11306,
		   .twobyte_threshold_divide = 2231,
		   .twobyte_threshold_min = 479,
		   .seqlenMinLimit3 = 147,
		   .blockSizeMinus = 124,
		   .sizeMaxForCanonicalHeaderPack = 630,
		   .sizeMinForSeqPack = 25748,
		   .sizeMinForCanonical = 625,
		   .sizeMaxForSuperslim = 42260,
		   .metaCompressionFactor = 22,
		   .offsetLimit1 = 106,
		   .offsetLimit2 = 935,
		   .offsetLimit3 = 63233
		},
			distanceProfile = {
		   .rle_ratio = 30,
		   .twobyte_ratio = 72,
		   .recursive_limit = 60,
		   .twobyte_threshold_max = 2689,
		   .twobyte_threshold_divide = 2358,
		   .twobyte_threshold_min = 471,
		   .seqlenMinLimit3 = 131,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 659,
		   .sizeMinForSeqPack = 23828,
		   .sizeMinForCanonical = 567,
		   .sizeMaxForSuperslim = 6972,
		   .metaCompressionFactor = 55,
		   .offsetLimit1 = 241,
		   .offsetLimit2 = 879,
		   .offsetLimit3 = 64632
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"icc")) {

		//Size: 2185.387

		packProfile mainProfile = {
		.rle_ratio = 55,
		.twobyte_ratio = 89,
		.recursive_limit = 556,
		.twobyte_threshold_max = 4875,
		.twobyte_threshold_divide = 3714,
		.twobyte_threshold_min = 521,
		.seqlenMinLimit3 = 249,
		.blockSizeMinus = 127,
		.sizeMaxForCanonicalHeaderPack = 258,
		.sizeMinForSeqPack = 56749,
		.sizeMinForCanonical = 558,
		.sizeMaxForSuperslim = 16477,
		.metaCompressionFactor = 37,
		.offsetLimit1 = 256,
		.offsetLimit2 = 752,
		.offsetLimit3 = 61662
		},
			seqlenProfile = {
		   .rle_ratio = 21,
		   .twobyte_ratio = 93,
		   .recursive_limit = 19,
		   .twobyte_threshold_max = 5671,
		   .twobyte_threshold_divide = 1660,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 99,
		   .blockSizeMinus = 89,
		   .sizeMaxForCanonicalHeaderPack = 673,
		   .sizeMinForSeqPack = 23506,
		   .sizeMinForCanonical = 454,
		   .sizeMaxForSuperslim = 20325,
		   .metaCompressionFactor = 151,
		   .offsetLimit1 = 245,
		   .offsetLimit2 = 797,
		   .offsetLimit3 = 66804
		},
			offsetProfile = {
		   .rle_ratio = 49,
		   .twobyte_ratio = 83,
		   .recursive_limit = 256,
		   .twobyte_threshold_max = 3928,
		   .twobyte_threshold_divide = 1259,
		   .twobyte_threshold_min = 553,
		   .seqlenMinLimit3 = 92,
		   .blockSizeMinus = 626,
		   .sizeMaxForCanonicalHeaderPack = 617,
		   .sizeMinForSeqPack = 17800,
		   .sizeMinForCanonical = 115,
		   .sizeMaxForSuperslim = 10,
		   .metaCompressionFactor = 172,
		   .offsetLimit1 = 236,
		   .offsetLimit2 = 999,
		   .offsetLimit3 = 79243
		},
			distanceProfile = {
		   .rle_ratio = 71,
		   .twobyte_ratio = 67,
		   .recursive_limit = 385,
		   .twobyte_threshold_max = 4159,
		   .twobyte_threshold_divide = 3865,
		   .twobyte_threshold_min = 54,
		   .seqlenMinLimit3 = 24,
		   .blockSizeMinus = 398,
		   .sizeMaxForCanonicalHeaderPack = 215,
		   .sizeMinForSeqPack = 4021,
		   .sizeMinForCanonical = 460,
		   .sizeMaxForSuperslim = 1570,
		   .metaCompressionFactor = 166,
		   .offsetLimit1 = 164,
		   .offsetLimit2 = 820,
		   .offsetLimit3 = 62958
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"tii")) {

		//Size: 206.640

		packProfile mainProfile = {
		.rle_ratio = 68,
		.twobyte_ratio = 91,
		.recursive_limit = 197,
		.twobyte_threshold_max = 3,
		.twobyte_threshold_divide = 2619,
		.twobyte_threshold_min = 65,
		.seqlenMinLimit3 = 254,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 631,
		.sizeMinForSeqPack = 10,
		.sizeMinForCanonical = 632,
		.sizeMaxForSuperslim = 61468,
		.metaCompressionFactor = 40,
		.offsetLimit1 = 256,
		.offsetLimit2 = 868,
		.offsetLimit3 = 60149
		},
			seqlenProfile = {
		   .rle_ratio = 37,
		   .twobyte_ratio = 49,
		   .recursive_limit = 466,
		   .twobyte_threshold_max = 3093,
		   .twobyte_threshold_divide = 2379,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 68,
		   .blockSizeMinus = 65,
		   .sizeMaxForCanonicalHeaderPack = 113,
		   .sizeMinForSeqPack = 11759,
		   .sizeMinForCanonical = 110,
		   .sizeMaxForSuperslim = 1669,
		   .metaCompressionFactor = 31,
		   .offsetLimit1 = 256,
		   .offsetLimit2 = 700,
		   .offsetLimit3 = 60884
		},
			offsetProfile = {
		   .rle_ratio = 27,
		   .twobyte_ratio = 91,
		   .recursive_limit = 102,
		   .twobyte_threshold_max = 4841,
		   .twobyte_threshold_divide = 2118,
		   .twobyte_threshold_min = 690,
		   .seqlenMinLimit3 = 151,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 625,
		   .sizeMinForSeqPack = 14330,
		   .sizeMinForCanonical = 103,
		   .sizeMaxForSuperslim = 29025,
		   .metaCompressionFactor = 37,
		   .offsetLimit1 = 246,
		   .offsetLimit2 = 726,
		   .offsetLimit3 = 61662
		},
			distanceProfile = {
		   .rle_ratio = 75,
		   .twobyte_ratio = 100,
		   .recursive_limit = 138,
		   .twobyte_threshold_max = 3,
		   .twobyte_threshold_divide = 2047,
		   .twobyte_threshold_min = 352,
		   .seqlenMinLimit3 = 7,
		   .blockSizeMinus = 34,
		   .sizeMaxForCanonicalHeaderPack = 179,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 368,
		   .sizeMaxForSuperslim = 63684,
		   .metaCompressionFactor = 37,
		   .offsetLimit1 = 256,
		   .offsetLimit2 = 752,
		   .offsetLimit3 = 61598
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"wav")) {

		//Size: 26098.898

		packProfile mainProfile = {
		.rle_ratio = 78,
		.twobyte_ratio = 100,
		.recursive_limit = 334,
		.twobyte_threshold_max = 1069,
		.twobyte_threshold_divide = 2161,
		.twobyte_threshold_min = 838,
		.seqlenMinLimit3 = 79,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 441,
		.sizeMinForSeqPack = 8521,
		.sizeMinForCanonical = 655,
		.sizeMaxForSuperslim = 94389,
		.metaCompressionFactor = 73,
		.offsetLimit1 = 256,
		.offsetLimit2 = 788,
		.offsetLimit3 = 63572
		},
			seqlenProfile = {
		   .rle_ratio = 48,
		   .twobyte_ratio = 100,
		   .recursive_limit = 375,
		   .twobyte_threshold_max = 9445,
		   .twobyte_threshold_divide = 478,
		   .twobyte_threshold_min = 120,
		   .seqlenMinLimit3 = 36,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 484,
		   .sizeMinForSeqPack = 27810,
		   .sizeMinForCanonical = 154,
		   .sizeMaxForSuperslim = 10901,
		   .metaCompressionFactor = 39,
		   .offsetLimit1 = 256,
		   .offsetLimit2 = 733,
		   .offsetLimit3 = 62662
		},
			offsetProfile = {
		   .rle_ratio = 70,
		   .twobyte_ratio = 97,
		   .recursive_limit = 175,
		   .twobyte_threshold_max = 5810,
		   .twobyte_threshold_divide = 3494,
		   .twobyte_threshold_min = 540,
		   .seqlenMinLimit3 = 119,
		   .blockSizeMinus = 160,
		   .sizeMaxForCanonicalHeaderPack = 481,
		   .sizeMinForSeqPack = 6875,
		   .sizeMinForCanonical = 463,
		   .sizeMaxForSuperslim = 10,
		   .metaCompressionFactor = 41,
		   .offsetLimit1 = 250,
		   .offsetLimit2 = 765,
		   .offsetLimit3 = 62292
		},
			distanceProfile = {
		   .rle_ratio = 100,
		   .twobyte_ratio = 58,
		   .recursive_limit = 110,
		   .twobyte_threshold_max = 2156,
		   .twobyte_threshold_divide = 4000,
		   .twobyte_threshold_min = 25,
		   .seqlenMinLimit3 = 19,
		   .blockSizeMinus = 72,
		   .sizeMaxForCanonicalHeaderPack = 404,
		   .sizeMinForSeqPack = 14537,
		   .sizeMinForCanonical = 35,
		   .sizeMaxForSuperslim = 24250,
		   .metaCompressionFactor = 66,
		   .offsetLimit1 = 256,
		   .offsetLimit2 = 842,
		   .offsetLimit3 = 65448
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"pdb")) {

		//Size: 43.913

		packProfile mainProfile = {
		.rle_ratio = 75,
		.twobyte_ratio = 96,
		.recursive_limit = 219,
		.twobyte_threshold_max = 3,
		.twobyte_threshold_divide = 1642,
		.twobyte_threshold_min = 778,
		.seqlenMinLimit3 = 167,
		.blockSizeMinus = 107,
		.sizeMaxForCanonicalHeaderPack = 534,
		.sizeMinForSeqPack = 10,
		.sizeMinForCanonical = 700,
		.sizeMaxForSuperslim = 50910,
		.metaCompressionFactor = 1,
		.offsetLimit1 = 204,
		.offsetLimit2 = 1357,
		.offsetLimit3 = 62123
		},
			seqlenProfile = {
		   .rle_ratio = 43,
		   .twobyte_ratio = 100,
		   .recursive_limit = 348,
		   .twobyte_threshold_max = 2790,
		   .twobyte_threshold_divide = 1691,
		   .twobyte_threshold_min = 667,
		   .seqlenMinLimit3 = 74,
		   .blockSizeMinus = 143,
		   .sizeMaxForCanonicalHeaderPack = 862,
		   .sizeMinForSeqPack = 12678,
		   .sizeMinForCanonical = 36,
		   .sizeMaxForSuperslim = 32,
		   .metaCompressionFactor = 104,
		   .offsetLimit1 = 228,
		   .offsetLimit2 = 700,
		   .offsetLimit3 = 65013
		},
			offsetProfile = {
		   .rle_ratio = 67,
		   .twobyte_ratio = 56,
		   .recursive_limit = 181,
		   .twobyte_threshold_max = 11264,
		   .twobyte_threshold_divide = 3175,
		   .twobyte_threshold_min = 133,
		   .seqlenMinLimit3 = 6,
		   .blockSizeMinus = 13,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 7007,
		   .sizeMinForCanonical = 423,
		   .sizeMaxForSuperslim = 14728,
		   .metaCompressionFactor = 67,
		   .offsetLimit1 = 177,
		   .offsetLimit2 = 1300,
		   .offsetLimit3 = 60597
		},
			distanceProfile = {
		   .rle_ratio = 31,
		   .twobyte_ratio = 50,
		   .recursive_limit = 141,
		   .twobyte_threshold_max = 3267,
		   .twobyte_threshold_divide = 2927,
		   .twobyte_threshold_min = 641,
		   .seqlenMinLimit3 = 87,
		   .blockSizeMinus = 57,
		   .sizeMaxForCanonicalHeaderPack = 666,
		   .sizeMinForSeqPack = 12841,
		   .sizeMinForCanonical = 229,
		   .sizeMaxForSuperslim = 21715,
		   .metaCompressionFactor = 160,
		   .offsetLimit1 = 251,
		   .offsetLimit2 = 1014,
		   .offsetLimit3 = 61025
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"rss")) {

		//Size: 23.874

		packProfile mainProfile = {
		.rle_ratio = 47,
		.twobyte_ratio = 93,
		.recursive_limit = 117,
		.twobyte_threshold_max = 2253,
		.twobyte_threshold_divide = 2837,
		.twobyte_threshold_min = 881,
		.seqlenMinLimit3 = 66,
		.blockSizeMinus = 9,
		.sizeMaxForCanonicalHeaderPack = 87,
		.sizeMinForSeqPack = 1679,
		.sizeMinForCanonical = 371,
		.sizeMaxForSuperslim = 63028,
		.metaCompressionFactor = 71,
		.offsetLimit1 = 256,
		.offsetLimit2 = 798,
		.offsetLimit3 = 64979
		},
			seqlenProfile = {
		   .rle_ratio = 16,
		   .twobyte_ratio = 69,
		   .recursive_limit = 275,
		   .twobyte_threshold_max = 4093,
		   .twobyte_threshold_divide = 2627,
		   .twobyte_threshold_min = 337,
		   .seqlenMinLimit3 = 34,
		   .blockSizeMinus = 32,
		   .sizeMaxForCanonicalHeaderPack = 80,
		   .sizeMinForSeqPack = 1143,
		   .sizeMinForCanonical = 59,
		   .sizeMaxForSuperslim = 8562,
		   .metaCompressionFactor = 84,
		   .offsetLimit1 = 255,
		   .offsetLimit2 = 828,
		   .offsetLimit3 = 62744
		},
			offsetProfile = {
		   .rle_ratio = 54,
		   .twobyte_ratio = 94,
		   .recursive_limit = 202,
		   .twobyte_threshold_max = 13000,
		   .twobyte_threshold_divide = 3057,
		   .twobyte_threshold_min = 711,
		   .seqlenMinLimit3 = 47,
		   .blockSizeMinus = 13,
		   .sizeMaxForCanonicalHeaderPack = 135,
		   .sizeMinForSeqPack = 37280,
		   .sizeMinForCanonical = 85,
		   .sizeMaxForSuperslim = 33162,
		   .metaCompressionFactor = 97,
		   .offsetLimit1 = 256,
		   .offsetLimit2 = 870,
		   .offsetLimit3 = 65817
		},
			distanceProfile = {
		   .rle_ratio = 62,
		   .twobyte_ratio = 88,
		   .recursive_limit = 73,
		   .twobyte_threshold_max = 5507,
		   .twobyte_threshold_divide = 3616,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 0,
		   .blockSizeMinus = 87,
		   .sizeMaxForCanonicalHeaderPack = 290,
		   .sizeMinForSeqPack = 15442,
		   .sizeMinForCanonical = 110,
		   .sizeMaxForSuperslim = 17244,
		   .metaCompressionFactor = 41,
		   .offsetLimit1 = 256,
		   .offsetLimit2 = 781,
		   .offsetLimit3 = 63911
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"dll")) {

		//Size: 3513.346

		packProfile mainProfile = {
		.rle_ratio = 100,
		.twobyte_ratio = 56,
		.recursive_limit = 487,
		.twobyte_threshold_max = 5904,
		.twobyte_threshold_divide = 3349,
		.twobyte_threshold_min = 834,
		.seqlenMinLimit3 = 104,
		.blockSizeMinus = 23,
		.sizeMaxForCanonicalHeaderPack = 1200,
		.sizeMinForSeqPack = 31251,
		.sizeMinForCanonical = 428,
		.sizeMaxForSuperslim = 24607,
		.metaCompressionFactor = 175,
		.offsetLimit1 = 185,
		.offsetLimit2 = 1216,
		.offsetLimit3 = 61528
		},
			seqlenProfile = {
		   .rle_ratio = 39,
		   .twobyte_ratio = 88,
		   .recursive_limit = 700,
		   .twobyte_threshold_max = 4861,
		   .twobyte_threshold_divide = 507,
		   .twobyte_threshold_min = 35,
		   .seqlenMinLimit3 = 204,
		   .blockSizeMinus = 73,
		   .sizeMaxForCanonicalHeaderPack = 601,
		   .sizeMinForSeqPack = 1339,
		   .sizeMinForCanonical = 264,
		   .sizeMaxForSuperslim = 83142,
		   .metaCompressionFactor = 56,
		   .offsetLimit1 = 251,
		   .offsetLimit2 = 996,
		   .offsetLimit3 = 62946
		},
			offsetProfile = {
		   .rle_ratio = 90,
		   .twobyte_ratio = 81,
		   .recursive_limit = 135,
		   .twobyte_threshold_max = 12307,
		   .twobyte_threshold_divide = 2202,
		   .twobyte_threshold_min = 804,
		   .seqlenMinLimit3 = 85,
		   .blockSizeMinus = 145,
		   .sizeMaxForCanonicalHeaderPack = 323,
		   .sizeMinForSeqPack = 20157,
		   .sizeMinForCanonical = 399,
		   .sizeMaxForSuperslim = 17190,
		   .metaCompressionFactor = 78,
		   .offsetLimit1 = 216,
		   .offsetLimit2 = 1199,
		   .offsetLimit3 = 68174
		},
			distanceProfile = {
		   .rle_ratio = 24,
		   .twobyte_ratio = 68,
		   .recursive_limit = 473,
		   .twobyte_threshold_max = 8381,
		   .twobyte_threshold_divide = 2381,
		   .twobyte_threshold_min = 298,
		   .seqlenMinLimit3 = 180,
		   .blockSizeMinus = 329,
		   .sizeMaxForCanonicalHeaderPack = 605,
		   .sizeMinForSeqPack = 1646,
		   .sizeMinForCanonical = 481,
		   .sizeMaxForSuperslim = 7287,
		   .metaCompressionFactor = 62,
		   .offsetLimit1 = 213,
		   .offsetLimit2 = 874,
		   .offsetLimit3 = 71330
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"exe")) {

		//Size: 14692.743

		packProfile mainProfile = {
		.rle_ratio = 53,
		.twobyte_ratio = 100,
		.recursive_limit = 347,
		.twobyte_threshold_max = 5782,
		.twobyte_threshold_divide = 1889,
		.twobyte_threshold_min = 648,
		.seqlenMinLimit3 = 196,
		.blockSizeMinus = 411,
		.sizeMaxForCanonicalHeaderPack = 429,
		.sizeMinForSeqPack = 8550,
		.sizeMinForCanonical = 700,
		.sizeMaxForSuperslim = 70531,
		.metaCompressionFactor = 119,
		.offsetLimit1 = 224,
		.offsetLimit2 = 1595,
		.offsetLimit3 = 66909
		},
			seqlenProfile = {
		   .rle_ratio = 21,
		   .twobyte_ratio = 74,
		   .recursive_limit = 454,
		   .twobyte_threshold_max = 6597,
		   .twobyte_threshold_divide = 512,
		   .twobyte_threshold_min = 180,
		   .seqlenMinLimit3 = 134,
		   .blockSizeMinus = 330,
		   .sizeMaxForCanonicalHeaderPack = 1126,
		   .sizeMinForSeqPack = 24617,
		   .sizeMinForCanonical = 323,
		   .sizeMaxForSuperslim = 13393,
		   .metaCompressionFactor = 199,
		   .offsetLimit1 = 197,
		   .offsetLimit2 = 1167,
		   .offsetLimit3 = 73236
		},
			offsetProfile = {
		   .rle_ratio = 92,
		   .twobyte_ratio = 82,
		   .recursive_limit = 62,
		   .twobyte_threshold_max = 11607,
		   .twobyte_threshold_divide = 3717,
		   .twobyte_threshold_min = 829,
		   .seqlenMinLimit3 = 62,
		   .blockSizeMinus = 253,
		   .sizeMaxForCanonicalHeaderPack = 182,
		   .sizeMinForSeqPack = 28258,
		   .sizeMinForCanonical = 118,
		   .sizeMaxForSuperslim = 49577,
		   .metaCompressionFactor = 153,
		   .offsetLimit1 = 252,
		   .offsetLimit2 = 1717,
		   .offsetLimit3 = 61935
		},
			distanceProfile = {
		   .rle_ratio = 26,
		   .twobyte_ratio = 87,
		   .recursive_limit = 228,
		   .twobyte_threshold_max = 4241,
		   .twobyte_threshold_divide = 3879,
		   .twobyte_threshold_min = 433,
		   .seqlenMinLimit3 = 110,
		   .blockSizeMinus = 470,
		   .sizeMaxForCanonicalHeaderPack = 112,
		   .sizeMinForSeqPack = 24233,
		   .sizeMinForCanonical = 474,
		   .sizeMaxForSuperslim = 8132,
		   .metaCompressionFactor = 157,
		   .offsetLimit1 = 231,
		   .offsetLimit2 = 1057,
		   .offsetLimit3 = 74863
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"nrm")) {

		//Size: 22.383

		packProfile mainProfile = {
		.rle_ratio = 66,
		.twobyte_ratio = 81,
		.recursive_limit = 10,
		.twobyte_threshold_max = 4348,
		.twobyte_threshold_divide = 3625,
		.twobyte_threshold_min = 752,
		.seqlenMinLimit3 = 54,
		.blockSizeMinus = 51,
		.sizeMaxForCanonicalHeaderPack = 307,
		.sizeMinForSeqPack = 3493,
		.sizeMinForCanonical = 522,
		.sizeMaxForSuperslim = 58184,
		.metaCompressionFactor = 166,
		.offsetLimit1 = 255,
		.offsetLimit2 = 2000,
		.offsetLimit3 = 65301
		},
			seqlenProfile = {
		   .rle_ratio = 32,
		   .twobyte_ratio = 84,
		   .recursive_limit = 10,
		   .twobyte_threshold_max = 6335,
		   .twobyte_threshold_divide = 2028,
		   .twobyte_threshold_min = 27,
		   .seqlenMinLimit3 = 19,
		   .blockSizeMinus = 152,
		   .sizeMaxForCanonicalHeaderPack = 553,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 35,
		   .sizeMaxForSuperslim = 18005,
		   .metaCompressionFactor = 58,
		   .offsetLimit1 = 222,
		   .offsetLimit2 = 1263,
		   .offsetLimit3 = 67064
		},
			offsetProfile = {
		   .rle_ratio = 91,
		   .twobyte_ratio = 80,
		   .recursive_limit = 203,
		   .twobyte_threshold_max = 11980,
		   .twobyte_threshold_divide = 3051,
		   .twobyte_threshold_min = 312,
		   .seqlenMinLimit3 = 131,
		   .blockSizeMinus = 42,
		   .sizeMaxForCanonicalHeaderPack = 614,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 256,
		   .sizeMaxForSuperslim = 37198,
		   .metaCompressionFactor = 118,
		   .offsetLimit1 = 179,
		   .offsetLimit2 = 1396,
		   .offsetLimit3 = 68177
		},
			distanceProfile = {
		   .rle_ratio = 84,
		   .twobyte_ratio = 61,
		   .recursive_limit = 10,
		   .twobyte_threshold_max = 932,
		   .twobyte_threshold_divide = 3292,
		   .twobyte_threshold_min = 3,
		   .seqlenMinLimit3 = 45,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 589,
		   .sizeMinForSeqPack = 36288,
		   .sizeMinForCanonical = 10,
		   .sizeMaxForSuperslim = 45580,
		   .metaCompressionFactor = 94,
		   .offsetLimit1 = 202,
		   .offsetLimit2 = 1519,
		   .offsetLimit3 = 68760
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	else if (equalsIgnoreCase(ext, L"pack")) {

	//Size: 365.258

	packProfile mainProfile = {
	.rle_ratio = 100,
	.twobyte_ratio = 46,
	.recursive_limit = 146,
	.twobyte_threshold_max = 2901,
	.twobyte_threshold_divide = 2067,
	.twobyte_threshold_min = 846,
	.seqlenMinLimit3 = 139,
	.blockSizeMinus = 13,
	.sizeMaxForCanonicalHeaderPack = 484,
	.sizeMinForSeqPack = 1024,
	.sizeMinForCanonical = 662,
	.sizeMaxForSuperslim = 72381,
	.metaCompressionFactor = 40,
	.offsetLimit1 = 107,
	.offsetLimit2 = 1697,
	.offsetLimit3 = 67764,
	.bytesWonMin = 157
	},
		seqlenProfile = {
	   .rle_ratio = 73,
	   .twobyte_ratio = 89,
	   .recursive_limit = 133,
	   .twobyte_threshold_max = 6039,
	   .twobyte_threshold_divide = 2749,
	   .twobyte_threshold_min = 50,
	   .seqlenMinLimit3 = 89,
	   .blockSizeMinus = 0,
	   .sizeMaxForCanonicalHeaderPack = 406,
	   .sizeMinForSeqPack = 15185,
	   .sizeMinForCanonical = 286,
	   .sizeMaxForSuperslim = 809,
	   .metaCompressionFactor = 123,
	   .offsetLimit1 = 185,
	   .offsetLimit2 = 1601,
	   .offsetLimit3 = 63186,
	   .bytesWonMin = -33
	},
		offsetProfile = {
	   .rle_ratio = 91,
	   .twobyte_ratio = 92,
	   .recursive_limit = 72,
	   .twobyte_threshold_max = 4456,
	   .twobyte_threshold_divide = 3118,
	   .twobyte_threshold_min = 449,
	   .seqlenMinLimit3 = 4,
	   .blockSizeMinus = 420,
	   .sizeMaxForCanonicalHeaderPack = 80,
	   .sizeMinForSeqPack = 731,
	   .sizeMinForCanonical = 357,
	   .sizeMaxForSuperslim = 15470,
	   .metaCompressionFactor = 82,
	   .offsetLimit1 = 233,
	   .offsetLimit2 = 1058,
	   .offsetLimit3 = 75123,
	   .bytesWonMin = -78
	},
		distanceProfile = {
	   .rle_ratio = 67,
	   .twobyte_ratio = 100,
	   .recursive_limit = 50,
	   .twobyte_threshold_max = 6923,
	   .twobyte_threshold_divide = 3984,
	   .twobyte_threshold_min = 47,
	   .seqlenMinLimit3 = 19,
	   .blockSizeMinus = 199,
	   .sizeMaxForCanonicalHeaderPack = 186,
	   .sizeMinForSeqPack = 10,
	   .sizeMinForCanonical = 364,
	   .sizeMaxForSuperslim = 30704,
	   .metaCompressionFactor = 53,
	   .offsetLimit1 = 239,
	   .offsetLimit2 = 1407,
	   .offsetLimit3 = 69270,
	   .bytesWonMin = 0
	};
	return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
}

	else if (equalsIgnoreCase(ext, L"pdf")) {

		//Size: 1350.391

		packProfile mainProfile = {
		.rle_ratio = 48,
		.twobyte_ratio = 52,
		.recursive_limit = 594,
		.twobyte_threshold_max = 3503,
		.twobyte_threshold_divide = 3730,
		.twobyte_threshold_min = 880,
		.seqlenMinLimit3 = 255,
		.blockSizeMinus = 50,
		.sizeMaxForCanonicalHeaderPack = 793,
		.sizeMinForSeqPack = 17899,
		.sizeMinForCanonical = 371,
		.sizeMaxForSuperslim = 37320,
		.metaCompressionFactor = 101,
		.offsetLimit1 = 223,
		.offsetLimit2 = 1122,
		.offsetLimit3 = 73367,
		.bytesWonMin = 33
		},
			seqlenProfile = {
		   .rle_ratio = 37,
		   .twobyte_ratio = 80,
		   .recursive_limit = 48,
		   .twobyte_threshold_max = 3,
		   .twobyte_threshold_divide = 1455,
		   .twobyte_threshold_min = 226,
		   .seqlenMinLimit3 = 101,
		   .blockSizeMinus = 92,
		   .sizeMaxForCanonicalHeaderPack = 296,
		   .sizeMinForSeqPack = 453,
		   .sizeMinForCanonical = 13,
		   .sizeMaxForSuperslim = 28188,
		   .metaCompressionFactor = 61,
		   .offsetLimit1 = 256,
		   .offsetLimit2 = 1065,
		   .offsetLimit3 = 77871,
		   .bytesWonMin = 92
		},
			offsetProfile = {
		   .rle_ratio = 79,
		   .twobyte_ratio = 14,
		   .recursive_limit = 96,
		   .twobyte_threshold_max = 13000,
		   .twobyte_threshold_divide = 1518,
		   .twobyte_threshold_min = 519,
		   .seqlenMinLimit3 = 160,
		   .blockSizeMinus = 0,
		   .sizeMaxForCanonicalHeaderPack = 928,
		   .sizeMinForSeqPack = 31115,
		   .sizeMinForCanonical = 574,
		   .sizeMaxForSuperslim = 10,
		   .metaCompressionFactor = 108,
		   .offsetLimit1 = 204,
		   .offsetLimit2 = 1293,
		   .offsetLimit3 = 71963,
		   .bytesWonMin = 79
		},
			distanceProfile = {
		   .rle_ratio = 100,
		   .twobyte_ratio = 89,
		   .recursive_limit = 10,
		   .twobyte_threshold_max = 2820,
		   .twobyte_threshold_divide = 2939,
		   .twobyte_threshold_min = 509,
		   .seqlenMinLimit3 = 44,
		   .blockSizeMinus = 191,
		   .sizeMaxForCanonicalHeaderPack = 408,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 414,
		   .sizeMaxForSuperslim = 2338,
		   .metaCompressionFactor = 38,
		   .offsetLimit1 = 226,
		   .offsetLimit2 = 1574,
		   .offsetLimit3 = 61675,
		   .bytesWonMin = 73
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}

	// -------------------------------------------------------------------------
	// Default profile for the rest
	// -------------------------------------------------------------------------

	return def;

}


completePackProfile getProfileForExtension(wchar_t* ext) {


	//Size: 95826.560

	packProfile mainProfile = {
	.rle_ratio = 94,
	.twobyte_ratio = 88,
	.recursive_limit = 287,
	.twobyte_threshold_max = 5206,
	.twobyte_threshold_divide = 2642,
	.twobyte_threshold_min = 815,
	.seqlenMinLimit3 = 214,
	.seqlenMinLimit4 = 65360,
	.blockSizeMinus = 13,
	.sizeMaxForCanonicalHeaderPack = 80,
	.sizeMinForSeqPack = 7899,
	.sizeMinForCanonical = 594,
	.sizeMaxForSuperslim = 68210,
	.metaCompressionFactor = 77,
	.offsetLimit1 = 247,
	.offsetLimit2 = 1101,
	.offsetLimit3 = 64930,
	.bytesWonMin = -63
	},
		seqlenProfile = {
		.rle_ratio = 35,
		.twobyte_ratio = 100,
		.recursive_limit = 117,
		.twobyte_threshold_max = 4970,
		.twobyte_threshold_divide = 2086,
		.twobyte_threshold_min = 44,
		.seqlenMinLimit3 = 218,
		.seqlenMinLimit4 = 65360,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 174,
		.sizeMinForSeqPack = 10,
		.sizeMinForCanonical = 75,
		.sizeMaxForSuperslim = 27827,
		.metaCompressionFactor = 64,
		.offsetLimit1 = 241,
		.offsetLimit2 = 1175,
		.offsetLimit3 = 64113,
		.bytesWonMin = 75
	},
		offsetProfile = {
		.rle_ratio = 78,
		.twobyte_ratio = 83,
		.recursive_limit = 10,
		.twobyte_threshold_max = 9024,
		.twobyte_threshold_divide = 2707,
		.twobyte_threshold_min = 372,
		.seqlenMinLimit3 = 245,
		.seqlenMinLimit4 = 65360,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 559,
		.sizeMinForSeqPack = 10,
		.sizeMinForCanonical = 165,
		.sizeMaxForSuperslim = 11090,
		.metaCompressionFactor = 62,
		.offsetLimit1 = 245,
		.offsetLimit2 = 1029,
		.offsetLimit3 = 65389,
		.bytesWonMin = 0
	},
		distanceProfile = {
		.rle_ratio = 69,
		.twobyte_ratio = 92,
		.recursive_limit = 10,
		.twobyte_threshold_max = 5494,
		.twobyte_threshold_divide = 3897,
		.twobyte_threshold_min = 3,
		.seqlenMinLimit3 = 234,
		.seqlenMinLimit4 = 65360,
		.blockSizeMinus = 75,
		.sizeMaxForCanonicalHeaderPack = 528,
		.sizeMinForSeqPack = 10,
		.sizeMinForCanonical = 389,
		.sizeMaxForSuperslim = 9660,
		.metaCompressionFactor = 59,
		.offsetLimit1 = 255,
		.offsetLimit2 = 973,
		.offsetLimit3 = 67870,
		.bytesWonMin = -1
	};

	completePackProfile prof = getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);

	return getProfileForExtensionOrDefault(ext, prof);

}