#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "seq_packer.h"
#include "common_tools.h"
#define TRUE 1
#define FALSE 0
#define END 999

#define math_max(x,y) ((x) >= (y)) ? (x) : (y)
#define math_min(x,y) ((x) <= (y)) ? (x) : (y)

/* sequence packer */

FILE* infil, * utfil;

unsigned char code;

void WRITE(unsigned long long c)
{
    fwrite(&c, 1, 1, utfil);
}


unsigned long long READ(unsigned char* cptr)
{
    return (fread(cptr, 1, 1, infil));
}

//global variables used in compressor
static unsigned long long buffer_endpos, buffer_startpos, buffer_min, buffer_size = 32768;
static unsigned char* buffer;

void move_buffer(unsigned int steps) {
    buffer_startpos += steps;
    if (buffer_endpos == buffer_size) {
        unsigned long long buffer_left = buffer_size - buffer_startpos;
        if (buffer_left < buffer_min) {
            //load new buffer!!
            unsigned char* new_buf = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
            for (unsigned long i = 0; i < (buffer_size - buffer_startpos); i++) {
                new_buf[i] = buffer[i + buffer_startpos];
            }
            unsigned long long res = fread(&new_buf[buffer_size - buffer_startpos], 1, buffer_startpos, infil);
            buffer_endpos = res + (buffer_size - buffer_startpos);
            free(buffer);
            buffer = new_buf;
            buffer_startpos = 0;
        }
    }
}

unsigned char find_best_code(long* char_freq) {
    unsigned char best;
    long lowest = 100000;
    for(unsigned int i = 0; i < 256; i++) {
        if (char_freq[i] < lowest) {
            lowest = char_freq[i];
            best = i;
            if (lowest == 0) {
                break;
            }
        }
    }//end for
    printf("\n Found code: %d that occured: %d times.", best, lowest);
    char_freq[best] = 100000; // mark it as used!
    return best;
}

//--------------------------------------------------------------------------------------------------------

void pack_internal(const char* source_filename, const char* dest_filename, unsigned char pass)
{
    printf("\n Sequence packing %s", source_filename);
    unsigned long long i, winsize = 1024, max_seq_len = 257, seq_len, best_seq_len,
                 best_seq_offset = 0,
                 min_seq_len = 3, offsets[1024] = { 0 }, seq_lens[258] = { 0 },
                 longest_offset = 765;
   
    long char_freq[256] = { 0 };
    buffer_startpos = 0;
    buffer_min = 1024;

    fopen_s(&infil, source_filename, "rb");
    if (!infil) {
        puts("Hittade inte infil!");
        getchar();
        exit(1);
    }    
    unsigned long total_size = get_file_size(infil);
    

    if (pass == 2) {
        fopen_s(&utfil, dest_filename, "wb");
        if (!utfil) {
            puts("Hittade inte utfil!"); getchar(); exit(1);
        }
    }

    /* start compression */

    buffer_endpos = fread(buffer, 1, buffer_size * sizeof(unsigned char), infil);

    //realloc(buffer, buffer_endpos * sizeof(unsigned char));

    while ((buffer_endpos - buffer_startpos) >= min_seq_len * 2) {
      best_seq_len = 2;
      for (i = 3; i < 1024; i++)
      {
        seq_len = 0;        
        // find matching sequence
        while (buffer_startpos + i + seq_len < buffer_endpos && buffer[buffer_startpos + seq_len] == buffer[buffer_startpos + i + seq_len] &&
             seq_len < max_seq_len && seq_len < i)
        {
            seq_len++;
        }
        //check if better than the best!
        int ok_seq_len = ((i - seq_len) <= longest_offset);
        if (pass == 1) {
            ok_seq_len = 1;
        }

        if (seq_len > best_seq_len && ok_seq_len) {
            best_seq_len = seq_len;
            best_seq_offset = i - seq_len;
        }
      }
      /* now we found the longest sequence in the window! */
      
      if (best_seq_len <= 2)
      {       /* no sequence found, move window 1 byte forward and read one more byte */
          if (pass == 1) {
              char_freq[buffer[buffer_startpos]]++;
              if (char_freq[buffer[buffer_startpos]] > 100000) {
                  char_freq[buffer[buffer_startpos]]--;
              }
          } else {
              // occurence of code in original is handled by {code, 0} pair
              if (buffer[buffer_startpos] == code) {
                  WRITE(0);
                  WRITE(code);
              }
              else
                 {
                      WRITE(buffer[buffer_startpos]);
                  }
          }
          move_buffer(1);
      }
      else { // insert code triple instead of the matching sequence!
 
          if (pass == 1) {                          
              offsets[best_seq_offset]++;// += (best_seq_len - 2);
                  seq_lens[best_seq_len]++;
          }
          if (pass == 2) {
              // write offset i.e. distance from end of match
              // now handles offsets from 0..765
              if (best_seq_offset < 254) {
                  WRITE(best_seq_offset); 
              }
              else if (best_seq_offset < 510) {
                  WRITE(best_seq_offset - 254); 
                  WRITE(255);
              }
              else {
                  WRITE(best_seq_offset - 510); 
                  WRITE(254);
              }
        
                  //subtract to so smallest will be 3 -2 = 1
                  //lens 0 and 1 are used for code occurences
                  WRITE(best_seq_len - 2);  /* seqlen */
                  WRITE(code);  /* note file is read backwards during unpack! */            
          }
          move_buffer(best_seq_len);
      }//end if
    }//end while

   
    if (pass == 1) {
       
        code = find_best_code(char_freq);
#if true
        printf("\n\n SEQ_LEN frequency:\n");
        for (int i = 0; i < 257; i++) {
            printf("%d,", seq_lens[i]);
        }

        printf("\n\n OFFSET :\n");
        for (int i = 0; i < 1024; i++) {
            printf("%d,", offsets[i]);
        }
#endif
        
    } else {
        //write remaining buffer
        for (i = buffer_startpos; i < buffer_endpos; i++) {
            WRITE(buffer[i]);
        }
        WRITE(code);
        fclose(utfil);
    }
    fclose(infil);
}

void seq_pack(const char* source_filename, const char* dest_filename)
{
    buffer = (unsigned char*)malloc(buffer_size * sizeof(unsigned char));
    pack_internal(source_filename, dest_filename, 1);
    pack_internal(source_filename, dest_filename, 2);
}

