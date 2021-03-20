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

	//Size: 78.318

	packProfile mainProfile = {
	.rle_ratio = 100,
	.twobyte_ratio = 76,
	.recursive_limit = 665,
	.twobyte_threshold_max = 7016,
	.twobyte_threshold_divide = 3501,
	.twobyte_threshold_min = 1000,
	.seqlenMinLimit3 = 163,
	.blockSizeMinus = 228,
	.sizeMaxForCanonicalHeaderPack = 356,
	.sizeMinForSeqPack = 18105,
	.sizeMinForCanonical = 372,
	.sizeMaxForSuperslim = 62878,
	.metaCompressionFactor = 74,
	.offsetLimit1 = 209,
	.offsetLimit2 = 1559,
	.offsetLimit3 = 66201
	},
		seqlenProfile = {
	   .rle_ratio = 10,
	   .twobyte_ratio = 100,
	   .recursive_limit = 589,
	   .twobyte_threshold_max = 10602,
	   .twobyte_threshold_divide = 2076,
	   .twobyte_threshold_min = 114,
	   .seqlenMinLimit3 = 230,
	   .blockSizeMinus = 0,
	   .sizeMaxForCanonicalHeaderPack = 699,
	   .sizeMinForSeqPack = 365,
	   .sizeMinForCanonical = 224,
	   .sizeMaxForSuperslim = 24671,
	   .metaCompressionFactor = 76,
	   .offsetLimit1 = 256,
	   .offsetLimit2 = 1451,
	   .offsetLimit3 = 69768
	},
		offsetProfile = {
	   .rle_ratio = 84,
	   .twobyte_ratio = 89,
	   .recursive_limit = 136,
	   .twobyte_threshold_max = 9233,
	   .twobyte_threshold_divide = 251,
	   .twobyte_threshold_min = 934,
	   .seqlenMinLimit3 = 62,
	   .blockSizeMinus = 114,
	   .sizeMaxForCanonicalHeaderPack = 212,
	   .sizeMinForSeqPack = 23473,
	   .sizeMinForCanonical = 361,
	   .sizeMaxForSuperslim = 1032,
	   .metaCompressionFactor = 67,
	   .offsetLimit1 = 256,
	   .offsetLimit2 = 1069,
	   .offsetLimit3 = 67224
	},
		distanceProfile = {
	   .rle_ratio = 21,
	   .twobyte_ratio = 86,
	   .recursive_limit = 251,
	   .twobyte_threshold_max = 7961,
	   .twobyte_threshold_divide = 1925,
	   .twobyte_threshold_min = 828,
	   .seqlenMinLimit3 = 206,
	   .blockSizeMinus = 146,
	   .sizeMaxForCanonicalHeaderPack = 504,
	   .sizeMinForSeqPack = 22929,
	   .sizeMinForCanonical = 532,
	   .sizeMaxForSuperslim = 20889,
	   .metaCompressionFactor = 49,
	   .offsetLimit1 = 226,
	   .offsetLimit2 = 1273,
	   .offsetLimit3 = 68026
	};
	return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
}
	else if (equalsIgnoreCase(ext, L"txt")) {

		//Size: 144.625

		packProfile mainProfile = {
		.rle_ratio = 97,
		.twobyte_ratio = 74,
		.recursive_limit = 335,
		.twobyte_threshold_max = 1072,
		.twobyte_threshold_divide = 4000,
		.twobyte_threshold_min = 880,
		.seqlenMinLimit3 = 223,
		.blockSizeMinus = 0,
		.sizeMaxForCanonicalHeaderPack = 274,
		.sizeMinForSeqPack = 2450,
		.sizeMinForCanonical = 199,
		.sizeMaxForSuperslim = 60193,
		.metaCompressionFactor = 120,
		.offsetLimit1 = 172,
		.offsetLimit2 = 1990,
		.offsetLimit3 = 65013
		},
			seqlenProfile = {
		   .rle_ratio = 32,
		   .twobyte_ratio = 100,
		   .recursive_limit = 449,
		   .twobyte_threshold_max = 13000,
		   .twobyte_threshold_divide = 1201,
		   .twobyte_threshold_min = 391,
		   .seqlenMinLimit3 = 255,
		   .blockSizeMinus = 247,
		   .sizeMaxForCanonicalHeaderPack = 145,
		   .sizeMinForSeqPack = 17559,
		   .sizeMinForCanonical = 639,
		   .sizeMaxForSuperslim = 59398,
		   .metaCompressionFactor = 63,
		   .offsetLimit1 = 200,
		   .offsetLimit2 = 1382,
		   .offsetLimit3 = 67731
		},
			offsetProfile = {
		   .rle_ratio = 96,
		   .twobyte_ratio = 100,
		   .recursive_limit = 391,
		   .twobyte_threshold_max = 6635,
		   .twobyte_threshold_divide = 3171,
		   .twobyte_threshold_min = 829,
		   .seqlenMinLimit3 = 210,
		   .blockSizeMinus = 76,
		   .sizeMaxForCanonicalHeaderPack = 513,
		   .sizeMinForSeqPack = 10,
		   .sizeMinForCanonical = 542,
		   .sizeMaxForSuperslim = 100000,
		   .metaCompressionFactor = 39,
		   .offsetLimit1 = 226,
		   .offsetLimit2 = 1608,
		   .offsetLimit3 = 67212
		},
			distanceProfile = {
		   .rle_ratio = 36,
		   .twobyte_ratio = 72,
		   .recursive_limit = 643,
		   .twobyte_threshold_max = 12049,
		   .twobyte_threshold_divide = 2703,
		   .twobyte_threshold_min = 538,
		   .seqlenMinLimit3 = 230,
		   .blockSizeMinus = 152,
		   .sizeMaxForCanonicalHeaderPack = 1200,
		   .sizeMinForSeqPack = 13957,
		   .sizeMinForCanonical = 617,
		   .sizeMaxForSuperslim = 21046,
		   .metaCompressionFactor = 120,
		   .offsetLimit1 = 179,
		   .offsetLimit2 = 1595,
		   .offsetLimit3 = 76549
		};
		return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
	}
	else if (equalsIgnoreCase(ext, L"doc")) {

	//Size: 691.527

	packProfile mainProfile = {
	.rle_ratio = 59,
	.twobyte_ratio = 33,
	.recursive_limit = 463,
	.twobyte_threshold_max = 9838,
	.twobyte_threshold_divide = 2952,
	.twobyte_threshold_min = 915,
	.seqlenMinLimit3 = 255,
	.blockSizeMinus = 89,
	.sizeMaxForCanonicalHeaderPack = 172,
	.sizeMinForSeqPack = 2694,
	.sizeMinForCanonical = 258,
	.sizeMaxForSuperslim = 16072,
	.metaCompressionFactor = 108,
	.offsetLimit1 = 139,
	.offsetLimit2 = 1555,
	.offsetLimit3 = 62742
	},
		seqlenProfile = {
	   .rle_ratio = 24,
	   .twobyte_ratio = 100,
	   .recursive_limit = 514,
	   .twobyte_threshold_max = 9801,
	   .twobyte_threshold_divide = 3023,
	   .twobyte_threshold_min = 226,
	   .seqlenMinLimit3 = 152,
	   .blockSizeMinus = 422,
	   .sizeMaxForCanonicalHeaderPack = 781,
	   .sizeMinForSeqPack = 5748,
	   .sizeMinForCanonical = 374,
	   .sizeMaxForSuperslim = 38296,
	   .metaCompressionFactor = 120,
	   .offsetLimit1 = 192,
	   .offsetLimit2 = 1874,
	   .offsetLimit3 = 64506
	},
		offsetProfile = {
	   .rle_ratio = 57,
	   .twobyte_ratio = 73,
	   .recursive_limit = 378,
	   .twobyte_threshold_max = 7207,
	   .twobyte_threshold_divide = 401,
	   .twobyte_threshold_min = 701,
	   .seqlenMinLimit3 = 70,
	   .blockSizeMinus = 189,
	   .sizeMaxForCanonicalHeaderPack = 179,
	   .sizeMinForSeqPack = 11911,
	   .sizeMinForCanonical = 587,
	   .sizeMaxForSuperslim = 35487,
	   .metaCompressionFactor = 120,
	   .offsetLimit1 = 162,
	   .offsetLimit2 = 1795,
	   .offsetLimit3 = 75153
	},
		distanceProfile = {
	   .rle_ratio = 35,
	   .twobyte_ratio = 91,
	   .recursive_limit = 10,
	   .twobyte_threshold_max = 4765,
	   .twobyte_threshold_divide = 3765,
	   .twobyte_threshold_min = 1000,
	   .seqlenMinLimit3 = 132,
	   .blockSizeMinus = 138,
	   .sizeMaxForCanonicalHeaderPack = 695,
	   .sizeMinForSeqPack = 48,
	   .sizeMinForCanonical = 514,
	   .sizeMaxForSuperslim = 9040,
	   .metaCompressionFactor = 120,
	   .offsetLimit1 = 148,
	   .offsetLimit2 = 1622,
	   .offsetLimit3 = 68397
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

	//Size: 28.955

	packProfile mainProfile = {
	.rle_ratio = 94,
	.twobyte_ratio = 96,
	.recursive_limit = 669,
	.twobyte_threshold_max = 12926,
	.twobyte_threshold_divide = 3791,
	.twobyte_threshold_min = 933,
	.seqlenMinLimit3 = 63,
	.blockSizeMinus = 325,
	.sizeMaxForCanonicalHeaderPack = 80,
	.sizeMinForSeqPack = 49069,
	.sizeMinForCanonical = 475,
	.sizeMaxForSuperslim = 53722,
	.metaCompressionFactor = 117,
	.offsetLimit1 = 256,
	.offsetLimit2 = 1262,
	.offsetLimit3 = 61968
	},
		seqlenProfile = {
	   .rle_ratio = 21,
	   .twobyte_ratio = 93,
	   .recursive_limit = 599,
	   .twobyte_threshold_max = 6712,
	   .twobyte_threshold_divide = 3300,
	   .twobyte_threshold_min = 274,
	   .seqlenMinLimit3 = 159,
	   .blockSizeMinus = 388,
	   .sizeMaxForCanonicalHeaderPack = 564,
	   .sizeMinForSeqPack = 30309,
	   .sizeMinForCanonical = 474,
	   .sizeMaxForSuperslim = 968,
	   .metaCompressionFactor = 26,
	   .offsetLimit1 = 102,
	   .offsetLimit2 = 1138,
	   .offsetLimit3 = 69305
	},
		offsetProfile = {
	   .rle_ratio = 69,
	   .twobyte_ratio = 37,
	   .recursive_limit = 339,
	   .twobyte_threshold_max = 13000,
	   .twobyte_threshold_divide = 946,
	   .twobyte_threshold_min = 950,
	   .seqlenMinLimit3 = 46,
	   .blockSizeMinus = 95,
	   .sizeMaxForCanonicalHeaderPack = 148,
	   .sizeMinForSeqPack = 71051,
	   .sizeMinForCanonical = 418,
	   .sizeMaxForSuperslim = 44166,
	   .metaCompressionFactor = 24,
	   .offsetLimit1 = 151,
	   .offsetLimit2 = 1581,
	   .offsetLimit3 = 61537
	},
		distanceProfile = {
	   .rle_ratio = 11,
	   .twobyte_ratio = 100,
	   .recursive_limit = 344,
	   .twobyte_threshold_max = 10107,
	   .twobyte_threshold_divide = 3645,
	   .twobyte_threshold_min = 836,
	   .seqlenMinLimit3 = 233,
	   .blockSizeMinus = 375,
	   .sizeMaxForCanonicalHeaderPack = 133,
	   .sizeMinForSeqPack = 4082,
	   .sizeMinForCanonical = 700,
	   .sizeMaxForSuperslim = 10403,
	   .metaCompressionFactor = 1,
	   .offsetLimit1 = 177,
	   .offsetLimit2 = 1599,
	   .offsetLimit3 = 66725
	};
	return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
}

	else if (equalsIgnoreCase(ext, L"prx")) {

	//Size: 5767.455

	packProfile mainProfile = {
	.rle_ratio = 68,
	.twobyte_ratio = 92,
	.recursive_limit = 592,
	.twobyte_threshold_max = 8989,
	.twobyte_threshold_divide = 1833,
	.twobyte_threshold_min = 843,
	.seqlenMinLimit3 = 44,
	.blockSizeMinus = 119,
	.sizeMaxForCanonicalHeaderPack = 527,
	.sizeMinForSeqPack = 21328,
	.sizeMinForCanonical = 700,
	.sizeMaxForSuperslim = 90212,
	.metaCompressionFactor = 120,
	.offsetLimit1 = 250,
	.offsetLimit2 = 1167,
	.offsetLimit3 = 62959
	},
		seqlenProfile = {
	   .rle_ratio = 34,
	   .twobyte_ratio = 100,
	   .recursive_limit = 696,
	   .twobyte_threshold_max = 8797,
	   .twobyte_threshold_divide = 2127,
	   .twobyte_threshold_min = 16,
	   .seqlenMinLimit3 = 197,
	   .blockSizeMinus = 52,
	   .sizeMaxForCanonicalHeaderPack = 561,
	   .sizeMinForSeqPack = 10,
	   .sizeMinForCanonical = 314,
	   .sizeMaxForSuperslim = 49398,
	   .metaCompressionFactor = 120,
	   .offsetLimit1 = 241,
	   .offsetLimit2 = 1331,
	   .offsetLimit3 = 61340
	},
		offsetProfile = {
	   .rle_ratio = 92,
	   .twobyte_ratio = 100,
	   .recursive_limit = 365,
	   .twobyte_threshold_max = 13000,
	   .twobyte_threshold_divide = 1877,
	   .twobyte_threshold_min = 985,
	   .seqlenMinLimit3 = 87,
	   .blockSizeMinus = 0,
	   .sizeMaxForCanonicalHeaderPack = 739,
	   .sizeMinForSeqPack = 29000,
	   .sizeMinForCanonical = 216,
	   .sizeMaxForSuperslim = 10,
	   .metaCompressionFactor = 118,
	   .offsetLimit1 = 256,
	   .offsetLimit2 = 1167,
	   .offsetLimit3 = 65993
	},
		distanceProfile = {
	   .rle_ratio = 53,
	   .twobyte_ratio = 75,
	   .recursive_limit = 274,
	   .twobyte_threshold_max = 3925,
	   .twobyte_threshold_divide = 3218,
	   .twobyte_threshold_min = 1000,
	   .seqlenMinLimit3 = 244,
	   .blockSizeMinus = 22,
	   .sizeMaxForCanonicalHeaderPack = 653,
	   .sizeMinForSeqPack = 2434,
	   .sizeMinForCanonical = 578,
	   .sizeMaxForSuperslim = 7893,
	   .metaCompressionFactor = 109,
	   .offsetLimit1 = 253,
	   .offsetLimit2 = 966,
	   .offsetLimit3 = 63177
	};
	return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
}

	else if (equalsIgnoreCase(ext, L"mp3")) {

	//Size: 2135.550

	packProfile mainProfile = {
	.rle_ratio = 34,
	.twobyte_ratio = 73,
	.recursive_limit = 556,
	.twobyte_threshold_max = 10276,
	.twobyte_threshold_divide = 3643,
	.twobyte_threshold_min = 651,
	.seqlenMinLimit3 = 0,
	.blockSizeMinus = 108,
	.sizeMaxForCanonicalHeaderPack = 312,
	.sizeMinForSeqPack = 37372,
	.sizeMinForCanonical = 606,
	.sizeMaxForSuperslim = 63402,
	.metaCompressionFactor = 27,
	.offsetLimit1 = 124,
	.offsetLimit2 = 1081,
	.offsetLimit3 = 63785
	},
		seqlenProfile = {
	   .rle_ratio = 15,
	   .twobyte_ratio = 64,
	   .recursive_limit = 584,
	   .twobyte_threshold_max = 8771,
	   .twobyte_threshold_divide = 1543,
	   .twobyte_threshold_min = 15,
	   .seqlenMinLimit3 = 255,
	   .blockSizeMinus = 31,
	   .sizeMaxForCanonicalHeaderPack = 563,
	   .sizeMinForSeqPack = 10,
	   .sizeMinForCanonical = 266,
	   .sizeMaxForSuperslim = 40961,
	   .metaCompressionFactor = 22,
	   .offsetLimit1 = 97,
	   .offsetLimit2 = 780,
	   .offsetLimit3 = 61814
	},
		offsetProfile = {
	   .rle_ratio = 10,
	   .twobyte_ratio = 63,
	   .recursive_limit = 584,
	   .twobyte_threshold_max = 10333,
	   .twobyte_threshold_divide = 1506,
	   .twobyte_threshold_min = 810,
	   .seqlenMinLimit3 = 64,
	   .blockSizeMinus = 385,
	   .sizeMaxForCanonicalHeaderPack = 144,
	   .sizeMinForSeqPack = 11361,
	   .sizeMinForCanonical = 339,
	   .sizeMaxForSuperslim = 16267,
	   .metaCompressionFactor = 22,
	   .offsetLimit1 = 103,
	   .offsetLimit2 = 1136,
	   .offsetLimit3 = 64009
	},
		distanceProfile = {
	   .rle_ratio = 19,
	   .twobyte_ratio = 100,
	   .recursive_limit = 10,
	   .twobyte_threshold_max = 13000,
	   .twobyte_threshold_divide = 1678,
	   .twobyte_threshold_min = 688,
	   .seqlenMinLimit3 = 150,
	   .blockSizeMinus = 374,
	   .sizeMaxForCanonicalHeaderPack = 710,
	   .sizeMinForSeqPack = 13702,
	   .sizeMinForCanonical = 700,
	   .sizeMaxForSuperslim = 25954,
	   .metaCompressionFactor = 23,
	   .offsetLimit1 = 124,
	   .offsetLimit2 = 930,
	   .offsetLimit3 = 63403
	};
	return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
}

	else if (equalsIgnoreCase(ext, L"resx")) {

	//Size: 2843.650

	packProfile mainProfile = {
	.rle_ratio = 100,
	.twobyte_ratio = 100,
	.recursive_limit = 439,
	.twobyte_threshold_max = 3635,
	.twobyte_threshold_divide = 1428,
	.twobyte_threshold_min = 889,
	.seqlenMinLimit3 = 0,
	.blockSizeMinus = 74,
	.sizeMaxForCanonicalHeaderPack = 850,
	.sizeMinForSeqPack = 22311,
	.sizeMinForCanonical = 243,
	.sizeMaxForSuperslim = 51686,
	.metaCompressionFactor = 50,
	.offsetLimit1 = 90,
	.offsetLimit2 = 700,
	.offsetLimit3 = 60000
	},
		seqlenProfile = {
	   .rle_ratio = 29,
	   .twobyte_ratio = 100,
	   .recursive_limit = 492,
	   .twobyte_threshold_max = 10249,
	   .twobyte_threshold_divide = 2423,
	   .twobyte_threshold_min = 82,
	   .seqlenMinLimit3 = 255,
	   .blockSizeMinus = 152,
	   .sizeMaxForCanonicalHeaderPack = 372,
	   .sizeMinForSeqPack = 3945,
	   .sizeMinForCanonical = 398,
	   .sizeMaxForSuperslim = 31319,
	   .metaCompressionFactor = 50,
	   .offsetLimit1 = 90,
	   .offsetLimit2 = 700,
	   .offsetLimit3 = 60000
	},
		offsetProfile = {
	   .rle_ratio = 80,
	   .twobyte_ratio = 77,
	   .recursive_limit = 329,
	   .twobyte_threshold_max = 11255,
	   .twobyte_threshold_divide = 835,
	   .twobyte_threshold_min = 928,
	   .seqlenMinLimit3 = 81,
	   .blockSizeMinus = 48,
	   .sizeMaxForCanonicalHeaderPack = 80,
	   .sizeMinForSeqPack = 33605,
	   .sizeMinForCanonical = 486,
	   .sizeMaxForSuperslim = 5294,
	   .metaCompressionFactor = 50,
	   .offsetLimit1 = 90,
	   .offsetLimit2 = 700,
	   .offsetLimit3 = 60000
	},
		distanceProfile = {
	   .rle_ratio = 35,
	   .twobyte_ratio = 78,
	   .recursive_limit = 374,
	   .twobyte_threshold_max = 6429,
	   .twobyte_threshold_divide = 3128,
	   .twobyte_threshold_min = 952,
	   .seqlenMinLimit3 = 254,
	   .blockSizeMinus = 167,
	   .sizeMaxForCanonicalHeaderPack = 496,
	   .sizeMinForSeqPack = 10258,
	   .sizeMinForCanonical = 488,
	   .sizeMaxForSuperslim = 6383,
	   .metaCompressionFactor = 50,
	   .offsetLimit1 = 90,
	   .offsetLimit2 = 700,
	   .offsetLimit3 = 60000
	};
	return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
}


	else if (equalsIgnoreCase(ext, L"fdt")) {

	//Size: 1641.248

	packProfile mainProfile = {
	.rle_ratio = 84,
	.twobyte_ratio = 82,
	.recursive_limit = 109,
	.twobyte_threshold_max = 2457,
	.twobyte_threshold_divide = 770,
	.twobyte_threshold_min = 946,
	.seqlenMinLimit3 = 193,
	.blockSizeMinus = 129,
	.sizeMaxForCanonicalHeaderPack = 614,
	.sizeMinForSeqPack = 3137,
	.sizeMinForCanonical = 700,
	.sizeMaxForSuperslim = 4317,
	.metaCompressionFactor = 70,
	.offsetLimit1 = 200,
	.offsetLimit2 = 1100,
	.offsetLimit3 = 66000
	},
		seqlenProfile = {
	   .rle_ratio = 23,
	   .twobyte_ratio = 82,
	   .recursive_limit = 315,
	   .twobyte_threshold_max = 2330,
	   .twobyte_threshold_divide = 1909,
	   .twobyte_threshold_min = 365,
	   .seqlenMinLimit3 = 0,
	   .blockSizeMinus = 151,
	   .sizeMaxForCanonicalHeaderPack = 247,
	   .sizeMinForSeqPack = 18572,
	   .sizeMinForCanonical = 90,
	   .sizeMaxForSuperslim = 41388,
	   .metaCompressionFactor = 50,
	   .offsetLimit1 = 150,
	   .offsetLimit2 = 1000,
	   .offsetLimit3 = 66000
	},
		offsetProfile = {
	   .rle_ratio = 62,
	   .twobyte_ratio = 51,
	   .recursive_limit = 88,
	   .twobyte_threshold_max = 9825,
	   .twobyte_threshold_divide = 3455,
	   .twobyte_threshold_min = 378,
	   .seqlenMinLimit3 = 70,
	   .blockSizeMinus = 68,
	   .sizeMaxForCanonicalHeaderPack = 413,
	   .sizeMinForSeqPack = 6165,
	   .sizeMinForCanonical = 289,
	   .sizeMaxForSuperslim = 34275,
	   .metaCompressionFactor = 50,
	   .offsetLimit1 = 150,
	   .offsetLimit2 = 1000,
	   .offsetLimit3 = 66000
	},
		distanceProfile = {
	   .rle_ratio = 95,
	   .twobyte_ratio = 61,
	   .recursive_limit = 279,
	   .twobyte_threshold_max = 648,
	   .twobyte_threshold_divide = 2773,
	   .twobyte_threshold_min = 3,
	   .seqlenMinLimit3 = 10,
	   .blockSizeMinus = 269,
	   .sizeMaxForCanonicalHeaderPack = 80,
	   .sizeMinForSeqPack = 14690,
	   .sizeMinForCanonical = 101,
	   .sizeMaxForSuperslim = 18452,
	   .metaCompressionFactor = 50,
	   .offsetLimit1 = 150,
	   .offsetLimit2 = 1000,
	   .offsetLimit3 = 66000
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

	//Size: 17325.020

	packProfile mainProfile = {
	.rle_ratio = 34,
	.twobyte_ratio = 79,
	.recursive_limit = 555,
	.twobyte_threshold_max = 943,
	.twobyte_threshold_divide = 3248,
	.twobyte_threshold_min = 948,
	.seqlenMinLimit3 = 177,
	.blockSizeMinus = 892,
	.sizeMaxForCanonicalHeaderPack = 320,
	.sizeMinForSeqPack = 64411,
	.sizeMinForCanonical = 281,
	.sizeMaxForSuperslim = 100000,
	.metaCompressionFactor = 60,
	.offsetLimit1 = 239,
	.offsetLimit2 = 998,
	.offsetLimit3 = 67763
	},
		seqlenProfile = {
	   .rle_ratio = 56,
	   .twobyte_ratio = 100,
	   .recursive_limit = 238,
	   .twobyte_threshold_max = 5298,
	   .twobyte_threshold_divide = 1732,
	   .twobyte_threshold_min = 263,
	   .seqlenMinLimit3 = 52,
	   .blockSizeMinus = 187,
	   .sizeMaxForCanonicalHeaderPack = 110,
	   .sizeMinForSeqPack = 2742,
	   .sizeMinForCanonical = 10,
	   .sizeMaxForSuperslim = 9034,
	   .metaCompressionFactor = 71,
	   .offsetLimit1 = 237,
	   .offsetLimit2 = 1030,
	   .offsetLimit3 = 69230
	},
		offsetProfile = {
	   .rle_ratio = 87,
	   .twobyte_ratio = 82,
	   .recursive_limit = 134,
	   .twobyte_threshold_max = 2988,
	   .twobyte_threshold_divide = 3593,
	   .twobyte_threshold_min = 614,
	   .seqlenMinLimit3 = 35,
	   .blockSizeMinus = 686,
	   .sizeMaxForCanonicalHeaderPack = 588,
	   .sizeMinForSeqPack = 21114,
	   .sizeMinForCanonical = 122,
	   .sizeMaxForSuperslim = 8957,
	   .metaCompressionFactor = 74,
	   .offsetLimit1 = 252,
	   .offsetLimit2 = 934,
	   .offsetLimit3 = 68962
	},
		distanceProfile = {
	   .rle_ratio = 88,
	   .twobyte_ratio = 64,
	   .recursive_limit = 60,
	   .twobyte_threshold_max = 12454,
	   .twobyte_threshold_divide = 3191,
	   .twobyte_threshold_min = 252,
	   .seqlenMinLimit3 = 61,
	   .blockSizeMinus = 167,
	   .sizeMaxForCanonicalHeaderPack = 876,
	   .sizeMinForSeqPack = 75116,
	   .sizeMinForCanonical = 383,
	   .sizeMaxForSuperslim = 25753,
	   .metaCompressionFactor = 68,
	   .offsetLimit1 = 252,
	   .offsetLimit2 = 883,
	   .offsetLimit3 = 69169
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

	//Size: 365.264

	packProfile mainProfile = {
	.rle_ratio = 87,
	.twobyte_ratio = 46,
	.recursive_limit = 215,
	.twobyte_threshold_max = 927,
	.twobyte_threshold_divide = 1890,
	.twobyte_threshold_min = 954,
	.seqlenMinLimit3 = 155,
	.blockSizeMinus = 13,
	.sizeMaxForCanonicalHeaderPack = 434,
	.sizeMinForSeqPack = 3907,
	.sizeMinForCanonical = 647,
	.sizeMaxForSuperslim = 68724,
	.metaCompressionFactor = 40,
	.offsetLimit1 = 117,
	.offsetLimit2 = 2000,
	.offsetLimit3 = 65515
	},
		seqlenProfile = {
	   .rle_ratio = 84,
	   .twobyte_ratio = 90,
	   .recursive_limit = 133,
	   .twobyte_threshold_max = 5103,
	   .twobyte_threshold_divide = 2747,
	   .twobyte_threshold_min = 74,
	   .seqlenMinLimit3 = 108,
	   .blockSizeMinus = 79,
	   .sizeMaxForCanonicalHeaderPack = 391,
	   .sizeMinForSeqPack = 16145,
	   .sizeMinForCanonical = 237,
	   .sizeMaxForSuperslim = 1039,
	   .metaCompressionFactor = 129,
	   .offsetLimit1 = 174,
	   .offsetLimit2 = 1636,
	   .offsetLimit3 = 63034
	},
		offsetProfile = {
	   .rle_ratio = 89,
	   .twobyte_ratio = 99,
	   .recursive_limit = 18,
	   .twobyte_threshold_max = 4404,
	   .twobyte_threshold_divide = 3183,
	   .twobyte_threshold_min = 387,
	   .seqlenMinLimit3 = 31,
	   .blockSizeMinus = 535,
	   .sizeMaxForCanonicalHeaderPack = 80,
	   .sizeMinForSeqPack = 10,
	   .sizeMinForCanonical = 458,
	   .sizeMaxForSuperslim = 24722,
	   .metaCompressionFactor = 74,
	   .offsetLimit1 = 240,
	   .offsetLimit2 = 1227,
	   .offsetLimit3 = 71144
	},
		distanceProfile = {
	   .rle_ratio = 64,
	   .twobyte_ratio = 100,
	   .recursive_limit = 61,
	   .twobyte_threshold_max = 5741,
	   .twobyte_threshold_divide = 3984,
	   .twobyte_threshold_min = 47,
	   .seqlenMinLimit3 = 0,
	   .blockSizeMinus = 175,
	   .sizeMaxForCanonicalHeaderPack = 274,
	   .sizeMinForSeqPack = 1338,
	   .sizeMinForCanonical = 327,
	   .sizeMaxForSuperslim = 26568,
	   .metaCompressionFactor = 65,
	   .offsetLimit1 = 236,
	   .offsetLimit2 = 1455,
	   .offsetLimit3 = 69270
	};
	return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
}
	else if (equalsIgnoreCase(ext, L"pdf")) {

	//Size: 1350.448

	packProfile mainProfile = {
	.rle_ratio = 38,
	.twobyte_ratio = 45,
	.recursive_limit = 493,
	.twobyte_threshold_max = 5021,
	.twobyte_threshold_divide = 2924,
	.twobyte_threshold_min = 794,
	.seqlenMinLimit3 = 237,
	.blockSizeMinus = 77,
	.sizeMaxForCanonicalHeaderPack = 550,
	.sizeMinForSeqPack = 22226,
	.sizeMinForCanonical = 327,
	.sizeMaxForSuperslim = 39766,
	.metaCompressionFactor = 107,
	.offsetLimit1 = 206,
	.offsetLimit2 = 1371,
	.offsetLimit3 = 71820
	},
		seqlenProfile = {
	   .rle_ratio = 13,
	   .twobyte_ratio = 100,
	   .recursive_limit = 151,
	   .twobyte_threshold_max = 424,
	   .twobyte_threshold_divide = 2642,
	   .twobyte_threshold_min = 383,
	   .seqlenMinLimit3 = 100,
	   .blockSizeMinus = 80,
	   .sizeMaxForCanonicalHeaderPack = 528,
	   .sizeMinForSeqPack = 676,
	   .sizeMinForCanonical = 80,
	   .sizeMaxForSuperslim = 7052,
	   .metaCompressionFactor = 91,
	   .offsetLimit1 = 256,
	   .offsetLimit2 = 1074,
	   .offsetLimit3 = 69108
	},
		offsetProfile = {
	   .rle_ratio = 72,
	   .twobyte_ratio = 34,
	   .recursive_limit = 47,
	   .twobyte_threshold_max = 12023,
	   .twobyte_threshold_divide = 1088,
	   .twobyte_threshold_min = 544,
	   .seqlenMinLimit3 = 167,
	   .blockSizeMinus = 82,
	   .sizeMaxForCanonicalHeaderPack = 791,
	   .sizeMinForSeqPack = 17194,
	   .sizeMinForCanonical = 494,
	   .sizeMaxForSuperslim = 21992,
	   .metaCompressionFactor = 72,
	   .offsetLimit1 = 251,
	   .offsetLimit2 = 1324,
	   .offsetLimit3 = 76171
	},
		distanceProfile = {
	   .rle_ratio = 100,
	   .twobyte_ratio = 95,
	   .recursive_limit = 24,
	   .twobyte_threshold_max = 3372,
	   .twobyte_threshold_divide = 3484,
	   .twobyte_threshold_min = 451,
	   .seqlenMinLimit3 = 61,
	   .blockSizeMinus = 163,
	   .sizeMaxForCanonicalHeaderPack = 408,
	   .sizeMinForSeqPack = 23793,
	   .sizeMinForCanonical = 517,
	   .sizeMaxForSuperslim = 7894,
	   .metaCompressionFactor = 91,
	   .offsetLimit1 = 222,
	   .offsetLimit2 = 1351,
	   .offsetLimit3 = 64844
	};
	return getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);
}

	// -------------------------------------------------------------------------
	// Default profile for the rest
	// -------------------------------------------------------------------------

	return def;

}


completePackProfile getProfileForExtension(wchar_t* ext) {


	//Size: 38207.712

	packProfile mainProfile = {
	.rle_ratio = 95,
	.twobyte_ratio = 84,
	.recursive_limit = 342,
	.twobyte_threshold_max = 4062,
	.twobyte_threshold_divide = 2287,
	.twobyte_threshold_min = 764,
	.seqlenMinLimit3 = 234,
	
	.blockSizeMinus = 0,
	.sizeMaxForCanonicalHeaderPack = 144,
	.sizeMinForSeqPack = 10,
	.sizeMinForCanonical = 531,
	.sizeMaxForSuperslim = 68342,
	.metaCompressionFactor = 70,
	.offsetLimit1 = 250,
	.offsetLimit2 = 1018,
	.offsetLimit3 = 66000
	},
		seqlenProfile = {
	   .rle_ratio = 23,
	   .twobyte_ratio = 100,
	   .recursive_limit = 133,
	   .twobyte_threshold_max = 3946,
	   .twobyte_threshold_divide = 2213,
	   .twobyte_threshold_min = 139,
	   .seqlenMinLimit3 = 234,
	 
	   .blockSizeMinus = 0,
	   .sizeMaxForCanonicalHeaderPack = 215,
	   .sizeMinForSeqPack = 10,
	   .sizeMinForCanonical = 53,
	   .sizeMaxForSuperslim = 31836,
	   .metaCompressionFactor = 70,
	.offsetLimit1 = 250,
	.offsetLimit2 = 1018,
	.offsetLimit3 = 66000
	},
		offsetProfile = {
	   .rle_ratio = 73,
	   .twobyte_ratio = 97,
	   .recursive_limit = 10,
	   .twobyte_threshold_max = 10819,
	   .twobyte_threshold_divide = 2910,
	   .twobyte_threshold_min = 467,
	   .seqlenMinLimit3 = 234,
	
	   .blockSizeMinus = 0,
	   .sizeMaxForCanonicalHeaderPack = 503,
	   .sizeMinForSeqPack = 10,
	   .sizeMinForCanonical = 293,
	   .sizeMaxForSuperslim = 5049,
	   .metaCompressionFactor = 70,
	.offsetLimit1 = 250,
	.offsetLimit2 = 1018,
	.offsetLimit3 = 66000
	},
		distanceProfile = {
	   .rle_ratio = 70,
	   .twobyte_ratio = 91,
	   .recursive_limit = 10,
	   .twobyte_threshold_max = 3641,
	   .twobyte_threshold_divide = 3847,
	   .twobyte_threshold_min = 3,
	  .seqlenMinLimit3 = 234,
	  
	   .blockSizeMinus = 0,
	   .sizeMaxForCanonicalHeaderPack = 443,
	   .sizeMinForSeqPack = 8467,
	   .sizeMinForCanonical = 319,
	   .sizeMaxForSuperslim = 17981,
	   .metaCompressionFactor = 70,
	.offsetLimit1 = 250,
	.offsetLimit2 = 1018,
	.offsetLimit3 = 66000
	};



	completePackProfile prof = getCompletePackProfile(mainProfile, seqlenProfile, offsetProfile, distanceProfile);

	return getProfileForExtensionOrDefault(ext, prof);

}