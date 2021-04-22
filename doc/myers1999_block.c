/*
 * Given two strings as command-line arguments, this program finds out
 * the edit distance between them.
 *
 * This is a pure C program for explanatory purposes, to illustrate how
 * Myers' block model works. For details, please read "4.2. THE BLOCKS
 * MODEL" in the following paper:
 *
 * > G. Myers. "A fast bit-vector algorithm for approximate string
 * > matching based on dynamic programming." Journal of the ACM, 1999.
 *
 * License:
 *
 *   MIT License (see polyleven/LICENSE)
 *
 * How to use:
 *
 *   $ gcc -O2 -o myers1999_block myers1999_block.c
 *   $ ./myers1999_block abcde abc
 *   2
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define CDIV(a,b) ((a) / (b) + ((a) % (b) > 0))
#define BIT(i,n) (((i) >> (n)) & 1)
#define FLIP(i,n) ((i) ^ ((uint64_t) 1 << (n)))

uint64_t *create_map(uint8_t *s, int64_t len)
{
    int64_t i;
    uint64_t *Peq;
    uint64_t *map;
    int64_t maplen = 256 * CDIV(len, 64);

    map = calloc(maplen, sizeof(uint64_t));
    if (map == NULL)
        return NULL;

    for (i = 0; i < len; i++) {
        Peq = map + (256 * (i / 64));
        Peq[s[i]] |= (uint64_t) 1 << (i % 64);
    }
    return map;
}

int64_t myers1999(uint8_t *s1, int64_t len1, uint8_t *s2, int64_t len2)
{
    uint64_t Eq, Xv, Xh, Ph, Mh, Pv, Mv, Last;
    uint64_t *Mhc, *Phc;
    int64_t i, b, hsize, vsize, Score;
    uint8_t Pb, Mb;
    uint64_t *map;
    uint64_t *Peq;

    hsize = CDIV(len1, 64);
    vsize = CDIV(len2, 64);
    Score = len2;

    map = create_map(s2, len2);
    if (map == NULL)
        return -1;

    Phc = malloc(hsize * 2 * sizeof(uint64_t));
    if (Phc == NULL)
        return -1;

    Mhc = Phc + hsize;
    memset(Phc, -1, hsize * sizeof(uint64_t));
    memset(Mhc, 0, hsize * sizeof(uint64_t));
    Last = (uint64_t) 1 << ((len2 - 1) % 64);

    for (b = 0; b < vsize; b++) {
        Peq = map + (256 * b);
        Mv = 0;
        Pv = (uint64_t) -1;
        Score = len2;

        for (i = 0; i < len1; i++) {
            Eq = Peq[s1[i]];

            Pb = BIT(Phc[i / 64], i % 64);
            Mb = BIT(Mhc[i / 64], i % 64);

            Xv = Eq | Mv;
            Xh = ((((Eq | Mb) & Pv) + Pv) ^ Pv) | Eq | Mb;

            Ph = Mv | ~ (Xh | Pv);
            Mh = Pv & Xh;

            if (Ph & Last) Score++;
            if (Mh & Last) Score--;

            if ((Ph >> 63) ^ Pb)
                Phc[i / 64] = FLIP(Phc[i / 64], i % 64);

            if ((Mh >> 63) ^ Mb)
                Mhc[i / 64] = FLIP(Mhc[i / 64], i % 64);

            Ph = (Ph << 1) | Pb;
            Mh = (Mh << 1) | Mb;

            Pv = Mh | ~ (Xv | Ph);
            Mv = Ph & Xv;
        }
    }
    free(Phc);
    free(map);
    return Score;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("usage: myers1999 str1 str2\n");
        return -1;
    }
    uint8_t *s1 = argv[1];
    uint8_t *s2 = argv[2];
    printf("%li\n", myers1999(s1, strlen(s1), s2, strlen(s2)));
    return 0;
}
