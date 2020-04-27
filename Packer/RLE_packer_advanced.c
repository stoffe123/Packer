#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#define TRUE 1
#define FALSE 0
#define VERBOSE 
#define WRITE(x) putc(x, utfil)

/* runlength packer and unpacker */

FILE* infil, * utfil;

#define RL_MAX 259
#define MAX_SUPPLEN_MAX 30   /* special table for len 3 - max 17 */
#define SUPPLEN_OFFSET 2
#define SEQMAX 2000
#define RL_MIN 3

enum { RL_REALMAX = RL_MAX + 32768 + 127 };

int free_lens[256], no_of_freelen, count_desc;

int min_val(int x, int y)
{
    return ((x < y) ? (x) : (y));
}

void write_freelen(int maxc)
{
    static int count, i, maxcount;
    if (maxc > 0)
    {
        /* first call on this file, initialize! */
        count = 0;
        i = 0;
        maxcount = maxc;
        return;
    }
    if (count == 0)
    {
        /* check for directly descending sequence */
        for (count = 1; i + count + 1 < no_of_freelen && count < maxcount && free_lens[i + count - 1] == free_lens[i + count] + 1; count++);
        /* sequence found */
        WRITE(free_lens[i]);
        WRITE(--count);     /* write acutal count - 1 ... that's the way get_freelen wants it */
    }
    else /* count > 0 */
    {
        count--;
    }
    maxcount--;
    i++;
}



int get_freelen(void)
{
    static int value;
    if (count_desc == 0)
    {
        value = getc(infil);  /* start value */
        if (value == 0)
            return 0;
        count_desc = getc(infil);
        return value;
    }
    else /* count_desc >0 */
    {
        count_desc--;
        return --value;
    }
}


void optimize_freelen(int free_lens[], int no_of_freelen)
{
    int  new_free_lens[256], start_desc[256], len_desc[256], max, i, flag = 1, j = 0, k = 0;
    start_desc[0] = no_of_freelen - 1;
    for (i = no_of_freelen - 1; i >= 0; i--)
    {
        if (i > 0 && free_lens[i - 1] + 1 == free_lens[i])
            flag++;
        else
        {
            len_desc[j++] = flag;
            start_desc[j] = i - 1;
            flag = 1;
        }
    }

    while (1)
    {
        max = 0;
        for (i = 1; i < j; i++)
        {
            if (len_desc[i] > len_desc[max])
                max = i;
        }
        if (len_desc[max] == 0) break;
        do
        {
            new_free_lens[k++] = free_lens[start_desc[max]--];
        } while (--len_desc[max]);
        /* len_desc[max] is 0 now it has been used */
    }
    if (k != no_of_freelen) printf("error k=%d, no_of_freelen=%d\n", k, no_of_freelen);
    for (i = 0; i < no_of_freelen; i++)
        free_lens[i] = new_free_lens[i];
}


void sort(int f[], int g[], int n)
{
    int i, j, imin, temp;
    for (i = 0; i < n; i++) {
        /* sök index för det minsta bland elementen nr i, i+1, … */
        imin = i;
        for (j = i + 1; j < n; j++)
            if (f[j] < f[imin])
                imin = j;
        /* byt element så det minsta hamnar i pos i */
        if (imin != i) {
            temp = f[i]; f[i] = f[imin]; f[imin] = temp;
            temp = g[i]; g[i] = g[imin]; g[imin] = temp;
        }
    } /* end for i */

}

void sort2(int f[][RL_MAX + 1], int g[][RL_MAX + 1])
{
    int temp, * last_elem = &f[255][RL_MAX];
    register int* p1 = &f[0][0], * p2 = &f[0][0], * g1 = &g[0][0], * g2 = &g[0][0],
        * max = 0, * gmax = 0;

    /* remove all elements <= 1 */
    while (p1 <= last_elem) {
        if (*p1 > 1)
        {
            *(p2++) = *p1;
            *(g2++) = *g1;
        }
        p1++;
        g1++;
    }
    *p2 = 0;

    for (p1 = &f[0][0], g1 = &g[0][0]; *p1; p1++, g1++) {
        /* sök index för det största bland elementen nr i, i+1, … */
        max = p1;
        for (p2 = p1 + 1, g2 = g1 + 1; *p2; p2++, g2++)
        {
            if (*p2 >= *max) {
                max = p2;
                gmax = g2;
            }
        }
        /* byt element så det största hamnar på pos i */
        if (max != p1) {
            temp = *p1; *p1 = *max; *max = temp;
            temp = *g1; *g1 = *gmax; *gmax = temp;
        }
    }
}

void WRITEn(char c, int n)
{
    static int i;   /* static for performance */
    for (i = 0; i < n; i++) {
        putc(c, utfil);
    }
}

void write_halfbyte(int halfbyte)
{
    static int byte, pos;
    switch (halfbyte)
    {
    case -1:  /* initialize */
        pos = 0;
        break;
    case -2:  /* flush */
        if (pos == 1) WRITE(byte);
        break;
    default:
        if (pos == 0)
        {
            byte = halfbyte * 16;
            pos = 1;
        }
        else
        {
            WRITE(byte + halfbyte);
            pos = 0;
        }
    }
}

void write_ext_halfbyte(int bt)
{
    if (bt < 15)
        write_halfbyte(bt);
    else
    {
        write_halfbyte(15);
        write_halfbyte(bt / 16);
        write_halfbyte(bt % 16);
    }
}


int read_halfbyte(int cmd)
{
    static int byte, pos;
    if (cmd == -1)
    {
        /* initialize */
        pos = 0;
        return 0;   /* the return value here should is not used */
    }
    else
    {
        if (pos == 0)
        {
            byte = getc(infil);
            pos = 1;
            return(byte / 16);
        }
        else
        {
            pos = 0;
            return(byte % 16);

        }
    }
}

int read_ext_halfbyte(int cmd)
{
    int x = read_halfbyte(cmd);
    if (x == 15)
    {
        int high = read_halfbyte(cmd);
        int low = read_halfbyte(cmd);
        return high * 16 + low;
    }
    return x;
}

int find_optimal_MAX_SUPPLEN(int seq_len[])
{
    /* find optimal MAX_SUPPLEN */
    int i = 0, bytes_gained = -1, best_len = SUPPLEN_OFFSET, best_res = -100;
    while (seq_len[i] && seq_len[i] < MAX_SUPPLEN_MAX)
    {
        bytes_gained += 2; /* for each len save one byte */
        if (seq_len[i + 1] != seq_len[i])
        {
            /* last one in that len! */

            if (bytes_gained > best_res)
            {
                best_res = bytes_gained;
                best_len = seq_len[i];
            }
            printf("\nlen=%d bytes_gained=%d", seq_len[i], bytes_gained);
            bytes_gained -= (seq_len[i + 1] - seq_len[i]);
        }
        i++;
    }
    return best_len;
}

int RLE_advanced_pack(const char* source_filename, const char* dest_filename)
{
    register int i, j;
    int freqtable[256], moredata, runlengths[256][RL_MAX + 1], code_len[256][RL_MAX + 1], seq_nn, k, esc;
    int last_len, tmp, cc, lastch, real_count, lcount = 1, flag, seq_len[SEQMAX], seq_chars[SEQMAX], code, len, MAX_SUPPLEN;

    infil = fopen(source_filename, "rb");
    if (!infil) {
        printf("Input file not found: %s\n", source_filename);
        return 1;
    }

    for (i = 0; i < 256; i++)
    {
        freqtable[i] = 0;
        for (j = 0; j < RL_MAX + 1; j++) runlengths[i][j] = 0;
    }

    lastch = getc(infil);
    do {
        moredata = ((cc = getc(infil)) != EOF);
        if (cc == lastch && lcount < RL_MAX && moredata)
        {
            lcount++;
        }
        else
        {
            if (lcount >= RL_MIN && runlengths[lastch][lcount] < INT_MAX - 1)
                runlengths[lastch][lcount]++;
            if (lcount == 1)  /* lcount == 2 gives no penalty */
                if (freqtable[lastch] < INT_MAX)
                    freqtable[lastch]++;

            lcount = 1;
            lastch = cc;
        }
    } while (moredata);

    for (i = 0; i < 256; i++)
    {
        if (runlengths[i][3] == 1)
        {
            freqtable[i]--;
        }
        else if (runlengths[i][3] > 1)
        {
            freqtable[i] -= 2;
        }
    }

    /* find the escape-char */
    esc = 0;
    for (i = 0; i < 256; i++) if (freqtable[i] < freqtable[esc]) esc = i;

#ifdef VERBOSE
    printf("\n code-value found='%c', with %d bytes penalty.\n", esc, freqtable[esc]);
#endif

    tmp = 0;
    no_of_freelen = 0;
    for (i = 5; i <= RL_MAX; i++) {
        flag = 0;
        for (j = 0; j < 256; j++) {
            if (runlengths[j][i] > 0)
            {
                flag = 1;
                break;
            }
            /* if (flag + runlengths[j][i] < INT_MAX-500)
                    flag += runlengths[j][i]; */
        }

        if (flag == 0 && i > 0) {
            tmp++;
            free_lens[no_of_freelen++] = i - 4;
        }
#ifdef VERBOSE
        else                  printf("length=%u appears total=%u times.\n", i, flag);
#endif
    }
#ifdef VERBOSE
    printf("\n total %d lengths had no occurrences.\n", no_of_freelen);

    printf("\n\n ----- freelen array ------------- \n\n");

    for (i = 0; i < no_of_freelen; i++) printf("%u,", free_lens[i]);
    puts("\n");
#endif

    /* init code_len array */
    for (i = 0; i < 256; i++)
        for (j = 0; j < RL_MAX + 1; j++)
            code_len[i][j] = i + 256 * j;

    sort2(runlengths, code_len);  /* sort the twodim array , tough job!! */


#ifdef VERBOSE
    printf("\n -------- analayze of BIG matrix ........ \n\n");
#endif

    seq_nn = 0;
    for (i = 0; i < 256; i++) {
        for (j = 0; j < RL_MAX + 1; j++) {
            if (runlengths[i][j] == 0) break;
            code = code_len[i][j] % 256;
            len = code_len[i][j] / 256;
            if ((runlengths[i][j] >= 3 || runlengths[i][j] == 2 && len <= MAX_SUPPLEN_MAX) &&
                seq_nn < no_of_freelen - 3 &&
                !(code == esc && len == 3))
            {
                seq_len[seq_nn] = len;
                seq_chars[seq_nn] = code;
#ifdef VERBOSE
                printf("code=%d rl=%d appears %d times.\n", code, len, runlengths[i][j]);
#endif
                seq_nn++;
            }
        }
        if (runlengths[i][j] == 0) break;
    }
    seq_len[seq_nn] = 0;

    sort(seq_len, seq_chars, seq_nn);

#ifdef VERBOSE
    printf("\n\n ---- after sort ----- \n\n");
    for (i = 0; i < seq_nn; i++) { printf("\n len='%u' char=%u", seq_len[i], seq_chars[i]); }
    printf("\ntotal no of runlength types >=3 is %u.\n\n", tmp);
#endif

    optimize_freelen(free_lens, no_of_freelen);

    rewind(infil);

    utfil = fopen(dest_filename, "wb");
    if (!utfil) {
        puts("Hittade inte utfil!"); getchar(); exit(1);
    }



    MAX_SUPPLEN = find_optimal_MAX_SUPPLEN(seq_len);


    /* write header in packed file */

    WRITE(esc);

    last_len = 3;

    /* write maxcount for the descending lens */
    write_freelen(seq_nn + 3);       /* init */

    write_freelen(0);  /* one escape */
    write_freelen(0);  /* two escape */
    write_freelen(0);  /* three escape */

    /* write number of codes for length 3,4 ... MAX_SUPPLEN */
    write_ext_halfbyte(-1); /* init */
    if (MAX_SUPPLEN % 2 == 1)
    {
        MAX_SUPPLEN++;
    }
#ifdef VERBOSE
    printf("\nMAX_SUPPLEN=%d\n", MAX_SUPPLEN);
#endif
    write_halfbyte((MAX_SUPPLEN - SUPPLEN_OFFSET) / 2);
    i = 0;
    for (k = 3; k <= MAX_SUPPLEN; k++)
    {
        j = 0;
        while (seq_len[i] == k) {
            i++;
            j++;
        }
        write_ext_halfbyte(j);
    }
    write_ext_halfbyte(seq_nn - i);
#ifdef VERBOSE
    printf("\nOther codes=%d\n", seq_nn - i);
#endif

    write_ext_halfbyte(-2);  /* flush */

    for (i = 0; i < no_of_freelen - 3 && seq_len[i] != 0; i++)
    {
        if (last_len <= MAX_SUPPLEN)
        {
            if (seq_len[i] == last_len)
            {
                write_freelen(0);
                WRITE(seq_chars[i]);
            }
            else if (seq_len[i] > last_len) /* should always be true */
            {
                if (seq_len[i] > MAX_SUPPLEN)
                {
                    write_freelen(0);
                    WRITE(seq_chars[i]);
                    WRITE(seq_len[i] - 4);
                }
                else
                {
                    write_freelen(0);
                    WRITE(seq_chars[i]);
                }
                last_len = seq_len[i];
            }
        }
        else /* last_len > MAX_SUPPLEN */
        {
            write_freelen(0);
            WRITE(seq_chars[i]);
            WRITE(seq_len[i] - 4);
            last_len = seq_len[i];
        }
    } /* end for */
    no_of_freelen = i;    /* number of actually used lengths */

    /* Finished writing header ... now read the file and pack it */

    lcount = 1;
    lastch = getc(infil);
    do {
        moredata = ((cc = getc(infil)) != EOF);
        if (cc == lastch && lcount < RL_REALMAX && moredata)
        {
            lcount++;
        }
        else
        {
            if (lcount <= 2 && lastch != esc)
            {
                /* this case is only here for speed */
                WRITEn(lastch, lcount);
            }
            else
            {
                if (lcount >= RL_MAX)
                {
                    real_count = lcount;
                    lcount = RL_MAX;
                }
                else
                {
                    real_count = 0;
                }
                /* check if lcount and lastch is in seq_len / seq_chars tables */
                for (i = 0; i < no_of_freelen && seq_len[i] < lcount; i++);
                for (; i < no_of_freelen && seq_len[i] == lcount && seq_chars[i] != lastch; i++);
                if (i < no_of_freelen && seq_len[i] == lcount)
                {
                    /* match!!! */
                    WRITE(esc);
                    WRITE(free_lens[i + 3]);
                }
                else if (lcount >= 4)
                {
                    WRITE(esc);
                    WRITE(lcount - 4);
                    WRITE(lastch);
                }
                else if (lastch == esc && lcount <= 3)
                {
                    WRITE(esc);
                    WRITE(free_lens[lcount - 1]);
                }
                else
                {
                    WRITEn(lastch, lcount);
                }
                if (real_count > 0)
                {
                    real_count -= RL_MAX;
                    if (real_count < 128)
                    {
                        /* one byte repr. for RL between RL_MAX and RL_MAX+127 */
                        WRITE(real_count);
                    }
                    else
                    {
                        /* two byte high-low repr. for RL = RL_MAX+128 and above*/
                        real_count -= 128;
                        WRITE(real_count / 256 + 128);
                        WRITE(real_count % 256);

                    }
                }
            }
            lcount = 1;
            lastch = cc;
        } /* end if */
    } while (moredata);

    fclose(infil);
    fclose(utfil);
    return 0;
}

void RLE_advanced_unpack(const char* source_filename, const char* dest_filename)
{
    int i, j, k, seq_len[256] = { 0 }, cc, lcnt, one_escape, two_escape, three_escape;
    int seq_chars[256] = { 0 }, esc, rl_char, rl_count, MAX_SUPPLEN, no_of_codes[100] = { 0 };
    
    infil = fopen(source_filename, "rb");
    if (!infil) {
        printf("Can't create output file: %s\n", source_filename);
        return;
    }

    /* read header */
    esc = getc(infil);
    count_desc = 0;     /* global variable .. oups */
    one_escape = get_freelen();
    two_escape = get_freelen();
    three_escape = get_freelen();

    read_halfbyte(-1); /*initialize */
    MAX_SUPPLEN = read_halfbyte(0) * 2 + SUPPLEN_OFFSET;
    for (k = 3; k <= MAX_SUPPLEN + 1; k++)
        no_of_codes[k] = read_ext_halfbyte(0);

    for (k = 3; k <= MAX_SUPPLEN + 1; k++) {

        for (j = 1; j <= no_of_codes[k]; j++)
            if (k <= MAX_SUPPLEN)
            {
                cc = get_freelen();
                seq_len[cc] = k;  /* real length , not length-3 here */
                seq_chars[cc] = getc(infil);
            }
            else
            {
                cc = get_freelen();
                seq_chars[cc] = getc(infil);
                seq_len[cc] = getc(infil) + 4;
            }
    }
    /* finished reading header. Now create the output file */

    utfil = fopen(dest_filename, "wb");
    if (!utfil) {
        puts("Hittade inte utfil:");
        puts(dest_filename);
        getchar();
        exit(1);
    }

    /* start the unpacking */

    while ((cc = getc(infil)) != EOF)
    {
        if (cc == esc)
        {
            if ((lcnt = getc(infil)) == one_escape)
            {
                WRITE(esc);
            }
            else if (lcnt == two_escape)
            {
                WRITEn(esc, 2);
            }
            else if (lcnt == three_escape)
            {
                WRITEn(esc, 3);
            }
            else
            {
                /* now check if runlength is in table */
                if (seq_len[lcnt])
                {
                    rl_char = seq_chars[lcnt];
                    rl_count = seq_len[lcnt];
                }
                else
                {
                    rl_char = getc(infil);
                    rl_count = lcnt + 4;
                }
                if (rl_count == RL_MAX)
                {
                    rl_count = getc(infil);
                    if (rl_count < 128)
                    {
                        /* one byte repr. */
                        rl_count += RL_MAX;
                    }
                    else
                    {
                        /* two byte high-low repr. */
                        rl_count = 256 * (rl_count - 128) + getc(infil) + RL_MAX + 128;
                    }
                }
                WRITEn(rl_char, rl_count);
            }
        }
        else
        {
            WRITE(cc);
        }
    }
    fclose(infil);
    fclose(utfil);
}

int main_rle_advanced()
{
    const char* files[] = { "cv.doc", "fast_tele.xls", "book1.txt", "rapporten.doc", "pic", "letter.wav", "powerpnt.exe", "" };
    char file[20], f1[60], f2[60], f3[60];
    int i = 0;
    clock_t     startTime = clock();

    while (strlen(files[i]) > 0)
    {
        strcpy(file, files[i++]);
        strcpy(f3, "F:/packer/test/_unpacked_");
        strcpy(f2, "F:/packer/test/_packed_");
        strcpy(f1, "F:/packer/test/");

        strcat(f1, file);
        strcat(f2, file);
        strcat(f3, file);

        RLE_advanced_pack(f1, f2);
        RLE_advanced_unpack(f2, f3);
    }
    printf("\nExcecution time = %lf seconds", (clock() - startTime) / (double)CLOCKS_PER_SEC);
    getchar();

    return 0;
}



