#ifndef PACKER_COMMONS_H
#define PACKER_COMMONS_H

const char** get_test_filenames();

#endif

void CanonicalDecodeAndReplace(const char* src);

bool CanonicalEncodeAndTest(const char* src);

bool SeqPackAndTest(const char* src, int seqlen_pages, int offset_pages, int ratio_limit);