#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "common_tools.h"

static const unsigned long BUF_SIZE = 32768;
static unsigned char buf[32768];

long long get_file_size(const FILE* f) {
    fseek(f, 0, SEEK_END);
    long long res = ftell(f);
    fseek(f, 0, SEEK_SET);
    return res;
}

long long get_file_size_from_name(const char* name) {
    const FILE* f = fopen(name, "r");
    long long res = get_file_size(f);
    fclose(f);
    return res;
}

unsigned char isKthBitSet(unsigned char value, unsigned char bit)
{
    return (value & (1 << bit));
}

unsigned char setKthBit(unsigned char value, unsigned char bit)
{
    // kth bit of n is being set by this operation 
    return ((1 << bit) | value);
}

void copy_internal(FILE* source_file, const char* dest_filename, unsigned long long size_to_copy, bool copy_the_rest) {
    FILE* out = fopen(dest_filename, "wb");

    unsigned long long total_read = 0, bytes_to_read, bytes_got;
    do {
        if (!copy_the_rest && total_read + BUF_SIZE > size_to_copy) {
            bytes_to_read = (size_to_copy - total_read);
        }
        else {
            bytes_to_read = BUF_SIZE;
        }
        bytes_got = fread(&buf, 1, bytes_to_read, source_file);
        fwrite(&buf, 1, bytes_got, out);
        total_read += bytes_got;
    } while (bytes_got == BUF_SIZE);
    fclose(out);
}

void copy_chunk(FILE* source_file, const char* dest_filename, unsigned long long size_to_copy) {
    copy_internal(source_file, dest_filename, size_to_copy, false);
}

void copy_the_rest(FILE* source_file, const char* dest_filename) {
    copy_internal(source_file, dest_filename, 0, true);
}


void copy_file(const char* src, const char* dst) {
    FILE* f = fopen(src, "rb");
    copy_the_rest(f, dst);
}


void append_to_file(FILE* main_file, const char* append_filename) {
    FILE* append_file = fopen(append_filename, "rb");
    unsigned long long bytes_got;
    do {
        bytes_got = fread(&buf, 1, BUF_SIZE, append_file);
        fwrite(&buf, 1, bytes_got, main_file);
    } while (bytes_got == BUF_SIZE);
    fclose(append_file);
}

void assert(int x) {
    if (!x) {
        printf("\n\n ASSERTION FAILURE!");
        exit(0);
    }
}
