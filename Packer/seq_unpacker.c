#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "seq_unpacker.h"
#include "common_tools.h"
#define TRUE 1
#define FALSE 0

#define math_max(x,y) ((x) >= (y)) ? (x) : (y)
#define math_min(x,y) ((x) <= (y)) ? (x) : (y)

// Unpacker stuff below !!

//Global vars used in unpacker
unsigned char code;
static FILE* infil, * utfil;
static long long read_packedfile_pos , 
                seqlens_file_pos,
                offsets_file_pos;
static unsigned char* buf;
static unsigned long buf_pos = 0;
static unsigned buf_size = 30000000;

unsigned char read_byte_from_file() {
    read_packedfile_pos++;
    fseek(infil, -read_packedfile_pos, SEEK_END);
    return fgetc(infil);

}

void write_byte_to_file(unsigned char cc) {
    putc(cc, utfil);
}

void put_buf(unsigned char c) {
    buf[buf_pos] = c;
    buf_pos--;
}

FILE* seq_lens_file;
FILE* offsets_file;

unsigned char read_seqlen() {
    seqlens_file_pos++;
    fseek(seq_lens_file, -seqlens_file_pos, SEEK_END);
    return fgetc(seq_lens_file);
}

unsigned char read_offset() {
    offsets_file_pos++;
    fseek(offsets_file, -offsets_file_pos, SEEK_END);
    return fgetc(offsets_file);
}

//------------------------------------------------------------------------------
void seq_unpack(const char* source_filename, const char* dest_filename)
{

    seq_lens_file = fopen("c:/test/seqlens", "rb");
    offsets_file = fopen("c:/test/offsets", "rb");

    printf("\n\n Unpacking %s", source_filename);
    unsigned long i, cc;

    read_packedfile_pos = 0;
    seqlens_file_pos = 0;
    offsets_file_pos = 0;
       

    fopen_s(&infil, source_filename, "rb");
    if (!infil) {
        printf("\nHittade inte utfil: %s", source_filename);
        getchar();
        exit(1);
    }
    fopen_s(&utfil, dest_filename, "wb");
    if (!utfil) {
        puts("Hittade inte utfil!");
        getchar();
        exit(1);
    }
    long long total_size = get_file_size(infil);
    printf("\nLength of packed : %d", total_size);
    buf = (unsigned char*)malloc(buf_size * sizeof(unsigned char));
    fseek(infil, 0, SEEK_END);

    buf_pos = buf_size - 1;
    code = read_byte_from_file();

    while (total_size > read_packedfile_pos) {
        cc = read_byte_from_file();
        if (cc == code) {
            unsigned long long seq_len = read_seqlen(), offset;

            if (cc == code && seq_len == 0) {
                //occurrence of code in original
                
                    put_buf(code);
                
            }
            else {
               
                    seq_len += 2;
                    offset = read_offset();
               
                if (offset == 255) {
                    offset = (unsigned long long)254 + (unsigned long long)read_offset();
                }
                else if (offset == 254) {
                    offset = (unsigned long long)510 + (unsigned long long)read_offset();
                }
                
                unsigned long match_index = buf_pos + offset + seq_len;
                for (i = 0; i < seq_len; i++) {
                    put_buf(buf[match_index - i]); ;
                }
            }
        }
        else {
            put_buf(cc);
        }

    }
    fwrite(&buf[buf_pos + 1], (buf_size - (buf_pos + 1)) * sizeof(unsigned char), 1, utfil);

    fclose(infil);
    fclose(utfil);
    fclose(seq_lens_file);
    fclose(offsets_file);
}









