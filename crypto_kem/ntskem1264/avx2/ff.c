/**
 *  ff.c
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
 *  Platform: AVX2
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#include <stdlib.h>
#include <string.h>
#include "ff.h"

void bitslice_mul12_64 (uint64_t* c, const uint64_t* a, const uint64_t* b);
void bitslice_mul12_256(__m256i*  d, const __m256i*  a, const __m256i*  b);

ff_unit ff_add_m(const FF2m* ff2m, ff_unit a, ff_unit b)
{
    return a ^ b;
}

ff_unit ff_reduce_12(uint32_t a)
{
    uint32_t u;
    
    /* GF(2^12), generated by f(x) = x^12 + x^3 + 1 */
    u  = a & 0x7F0000;
    a ^= (u >>  9);
    a ^= (u >> 12);
    
    u  = a & 0x00F000;
    a ^= (u >>  9);
    a ^= (u >> 12);
    
    return (ff_unit)(a & 0xFFF);
}

ff_unit ff_mul_12(const FF2m* ff2m, ff_unit a, ff_unit b)
{
    uint32_t t;
    
    /* Perform shift-and-add multiplication */
    t  = a * (b & 1);
    t ^= (a * (b & 0x0002));
    t ^= (a * (b & 0x0004));
    t ^= (a * (b & 0x0008));
    t ^= (a * (b & 0x0010));
    t ^= (a * (b & 0x0020));
    t ^= (a * (b & 0x0040));
    t ^= (a * (b & 0x0080));
    t ^= (a * (b & 0x0100));
    t ^= (a * (b & 0x0200));
    t ^= (a * (b & 0x0400));
    t ^= (a * (b & 0x0800));
    
    /* Return the modulo reduction of t */
    return ff_reduce_12(t);
}

ff_unit ff_sqr_12(const FF2m* ff2m, ff_unit a)
{
    uint32_t b = a;
    
    b = (b | (b << 8)) & 0x00FF00FF;
    b = (b | (b << 4)) & 0x0F0F0F0F;
    b = (b | (b << 2)) & 0x33333333;
    b = (b | (b << 1)) & 0x55555555;
    
    return ff_reduce_12(b);
}

ff_unit ff_inv_12(const FF2m* ff2m, ff_unit a)
{
    ff_unit a3, a15, b;
    
    a3 = ff_sqr_12(ff2m, a);        /* a^2 */
    a3 = ff_mul_12(ff2m, a3, a);    /* a^3 */
    
    a15 = ff_sqr_12(ff2m, a3);      /* a^6 */
    a15 = ff_sqr_12(ff2m, a15);     /* a^12 */
    a15 = ff_mul_12(ff2m, a15, a3); /* a^15 */
    
    b = ff_sqr_12(ff2m, a15);       /* a^30 */
    b = ff_sqr_12(ff2m, b);         /* a^60 */
    b = ff_sqr_12(ff2m, b);         /* a^120 */
    b = ff_sqr_12(ff2m, b);         /* a^240 */
    b = ff_mul_12(ff2m, b, a15);    /* a^255 */
    
    b = ff_sqr_12(ff2m, b);         /* a^510 */
    b = ff_sqr_12(ff2m, b);         /* a^1020 */
    b = ff_mul_12(ff2m, b, a3);     /* a^1023 */
    
    b = ff_sqr_12(ff2m, b);         /* a^2046 */
    b = ff_mul_12(ff2m, b, a);      /* a^2047 */
    
    return ff_sqr_12(ff2m, b);      /* a^4094 */
}

void vector_ff_mul_12(const FF2m* ff2m, vector* c, const vector* a, const vector* b)
{
    bitslice_mul12_256(c, a, b);
}

void vector_ff_sqr_12(const FF2m* ff2m, vector* b, const vector* a)
{
    b[ 0] = a[ 0] ^ a[ 6];
    b[ 1] = a[11];
    b[ 2] = a[ 1] ^ a[ 7];
    b[ 3] = a[ 6];
    b[ 4] = a[ 2] ^ a[ 8] ^ a[11];
    b[ 5] = a[ 7];
    b[ 6] = a[ 3] ^ a[ 9];
    b[ 7] = a[ 8];
    b[ 8] = a[ 4] ^ a[10];
    b[ 9] = a[ 9];
    b[10] = a[ 5] ^ a[11];
    b[11] = a[10];
}

void vector_ff_pow4_12(const FF2m* ff2m, vector* b, const vector* a)
{
    b[ 0] = a[ 0] ^ a[ 3] ^ a[ 6] ^ a[ 9];
    b[ 1] = a[10];
    b[ 2] = a[ 8] ^ a[11];
    b[ 3] = a[ 3] ^ a[ 9];
    b[ 4] = a[ 1] ^ a[ 4] ^ a[ 7];
    b[ 5] = a[ 8];
    b[ 6] = a[ 6] ^ a[ 9];
    b[ 7] = a[ 4] ^ a[10];
    b[ 8] = a[ 2] ^ a[ 5] ^ a[ 8];
    b[ 9] = a[ 9];
    b[10] = a[ 7] ^ a[10];
    b[11] = a[ 5] ^ a[11];
}

void vector_ff_pow8_12(const FF2m* ff2m, vector* b, const vector* a)
{
    b[ 0] = a[ 0] ^ a[ 3];
    b[ 1] = a[ 5] ^ a[11];
    b[ 2] = a[ 4];
    b[ 3] = a[ 6] ^ a[ 9];
    b[ 4] = a[ 2];
    b[ 5] = a[ 4] ^ a[10];
    b[ 6] = a[ 3];
    b[ 7] = a[ 2] ^ a[ 5] ^ a[ 8];
    b[ 8] = a[ 1] ^ b[ 5];
    b[ 9] = a[ 9];
    b[10] = b[ 1] ^ a[ 8];
    b[11] = a[ 7] ^ a[10];
}

void vector_ff_pow16_12(const FF2m* ff2m, vector* b, const vector* a)
{
    b[ 0] = a[ 0];
    b[ 1] = a[ 7] ^ a[10];
    b[ 2] = a[ 2] ^ a[ 8] ^ a[11];
    b[ 3] = a[ 3];
    b[ 4] = a[ 1] ^ a[ 7];
    b[ 5] = a[ 2] ^ a[ 5] ^ a[ 8];
    b[ 6] = a[ 6];
    b[ 7] = a[ 1] ^ a[ 4] ^ a[10];
    b[ 8] = b[ 5] ^ a[11];
    b[ 9] = a[ 9];
    b[10] = a[ 4] ^ a[ 7];
    b[11] = a[ 5] ^ a[ 8] ^ a[11];
}

void vector_ff_inv_12(const FF2m* ff2m, vector* b, const vector* a)
{
    vector a3[12], a15[12], t[12];
    
    vector_ff_sqr_12(ff2m, t, a);        /* a^2 */
    vector_ff_mul_12(ff2m, a3, t, a);    /* a^3 */
    
    vector_ff_pow4_12(ff2m, t, a3);      /* a^12 */
    vector_ff_mul_12(ff2m, a15, t, a3);  /* a^15 */
    
    vector_ff_pow16_12(ff2m, b, a15);    /* a^240 */
    vector_ff_mul_12(ff2m, t, b, a15);   /* a^255 */
    
    vector_ff_pow4_12(ff2m, b, t);       /* a^1020 */
    vector_ff_mul_12(ff2m, t, b, a3);    /* a^1023 */
    
    vector_ff_sqr_12(ff2m, b, t);        /* a^2046 */
    vector_ff_mul_12(ff2m, t, b, a);     /* a^2047 */
    vector_ff_sqr_12(ff2m, b, t);        /* a^4094 */
}

void vector_ff_sqr_inv_12(const FF2m* ff2m, vector* b, const vector* a)
{
    vector a3[12], a15[12], t[12];
    
    vector_ff_sqr_12(ff2m, t, a);        /* a^2 */
    vector_ff_mul_12(ff2m, a3, t, a);    /* a^3 */
    
    vector_ff_pow4_12(ff2m, t, a3);      /* a^12 */
    vector_ff_mul_12(ff2m, a15, t, a3);  /* a^15 */
    
    vector_ff_pow16_12(ff2m, b, a15);    /* a^240 */
    vector_ff_mul_12(ff2m, t, b, a15);   /* a^255 */
    
    vector_ff_pow4_12(ff2m, b, t);       /* a^1020 */
    vector_ff_mul_12(ff2m, t, b, a3);    /* a^1023 */
    
    vector_ff_sqr_12(ff2m, b, t);        /* a^2046 */
    vector_ff_mul_12(ff2m, t, b, a);     /* a^2047 */
    vector_ff_pow4_12(ff2m, b, t);       /* a^4093 */
}

ff_unit vector_ff_transpose_xor_12(const struct FF2m* ff2m,
                                   const vector* a)
{
    ff_unit s = 0;

    s  = (vector_popcount(a[ 0]) & 1);
    s |= (vector_popcount(a[ 1]) & 1) <<  1;
    s |= (vector_popcount(a[ 2]) & 1) <<  2;
    s |= (vector_popcount(a[ 3]) & 1) <<  3;
    s |= (vector_popcount(a[ 4]) & 1) <<  4;
    s |= (vector_popcount(a[ 5]) & 1) <<  5;
    s |= (vector_popcount(a[ 6]) & 1) <<  6;
    s |= (vector_popcount(a[ 7]) & 1) <<  7;
    s |= (vector_popcount(a[ 8]) & 1) <<  8;
    s |= (vector_popcount(a[ 9]) & 1) <<  9;
    s |= (vector_popcount(a[10]) & 1) << 10;
    s |= (vector_popcount(a[11]) & 1) << 11;
    
    return s;
}

FF2m* ff_create()
{
    int i;
    FF2m *ff2m = NULL;
    
    if (!(ff2m = (FF2m *)malloc(sizeof(FF2m))))
        return NULL;
    
    ff2m->m = 12;
    ff2m->ff_add = &ff_add_m;
    ff2m->ff_mul = &ff_mul_12;
    ff2m->ff_sqr = &ff_sqr_12;
    ff2m->ff_inv = &ff_inv_12;
    ff2m->vector_ff_mul = &vector_ff_mul_12;
    ff2m->vector_ff_sqr = &vector_ff_sqr_12;
    ff2m->vector_ff_inv = &vector_ff_inv_12;
    ff2m->vector_ff_sqr_inv = &vector_ff_sqr_inv_12;
    ff2m->vector_ff_transpose_xor = &vector_ff_transpose_xor_12;

    /**
     * Basis, B = <beta^{m-1},beta^{m-2},...,beta,1>
     **/
    if (!(ff2m->basis = (ff_unit *)calloc(ff2m->m, sizeof(ff_unit))))
        return NULL;
    for (i=0; i<ff2m->m; i++) {
        ff2m->basis[ff2m->m-i-1] = (1 << i);
    }
    
    return ff2m;
}

void ff_release(FF2m* ff2m)
{
    if (ff2m) {
        if (ff2m->basis) {
            memset(ff2m->basis, 0, ff2m->m*sizeof(ff_unit));
            free(ff2m->basis); ff2m->basis = NULL;
        }
        ff2m->m = 0;
        ff2m->ff_add = NULL;
        ff2m->ff_mul = NULL;
        ff2m->ff_sqr = NULL;
        ff2m->ff_inv = NULL;
        ff2m->vector_ff_mul = NULL;
        ff2m->vector_ff_sqr = NULL;
        ff2m->vector_ff_inv = NULL;
        ff2m->vector_ff_sqr_inv = NULL;
        ff2m->vector_ff_transpose_xor = NULL;
        free(ff2m);
    }
}

void bitslice_mul12_64(uint64_t* c, const uint64_t* a, const uint64_t* b)
{
    uint64_t t[12];
    
    /**
     * These sequences of & and ^ are obtained from here:
     * http://www.cs.yale.edu/homes/peralta/CircuitStuff/binary_pol_mult/CMT12.txt
     *
     * Circuit Minimization Work
     **/
    uint64_t h22 = a[11] & b[11];
    uint64_t y2 = a[11] & b[9];
    uint64_t y3 = a[11] & b[10];
    uint64_t y4 = a[9] & b[11];
    uint64_t y5 = a[10] & b[11];
    uint64_t y6 = a[10] & b[10];
    uint64_t y7 = a[10] & b[9];
    uint64_t y8 = a[9] & b[10];
    uint64_t y9 = a[9] & b[9];
    uint64_t y10 = y8 ^ y7;
    uint64_t h21 = y5 ^ y3;
    uint64_t y12 = a[8] & b[8];
    uint64_t y13 = a[8] & b[6];
    uint64_t y14 = a[8] & b[7];
    uint64_t y15 = a[6] & b[8];
    uint64_t y16 = y13 ^ y15;
    uint64_t y17 = a[7] & b[8];
    uint64_t y18 = a[7] & b[7];
    uint64_t y19 = a[7] & b[6];
    uint64_t y20 = a[6] & b[7];
    uint64_t y21 = a[6] & b[6];
    uint64_t y22 = y20 ^ y19;
    uint64_t y23 = y17 ^ y14;
    uint64_t y24 = a[5] & b[5];
    uint64_t y25 = a[5] & b[3];
    uint64_t y26 = a[5] & b[4];
    uint64_t y27 = a[3] & b[5];
    uint64_t y28 = a[4] & b[5];
    uint64_t y29 = a[4] & b[4];
    uint64_t y30 = a[4] & b[3];
    uint64_t y31 = a[3] & b[4];
    uint64_t y32 = a[3] & b[3];
    uint64_t y33 = y31 ^ y30;
    uint64_t y34 = y28 ^ y26;
    uint64_t y35 = a[2] & b[2];
    uint64_t y36 = a[2] & b[0];
    uint64_t y37 = a[2] & b[1];
    uint64_t y38 = a[0] & b[2];
    uint64_t y39 = a[1] & b[2];
    uint64_t y40 = a[1] & b[1];
    uint64_t y41 = a[1] & b[0];
    uint64_t y42 = a[0] & b[1];
    t[0] = a[0] & b[0];
    t[1] = y42 ^ y41;
    uint64_t y45 = y39 ^ y37;
    uint64_t y46 = y45 ^ y32;
    uint64_t y47 = y35 ^ y33;
    uint64_t y48 = y34 ^ y21;
    uint64_t y49 = y24 ^ y22;
    uint64_t y50 = y23 ^ y9;
    uint64_t y51 = y12 ^ y10;
    uint64_t y52 = b[6] ^ b[9];
    uint64_t y53 = b[7] ^ b[10];
    uint64_t y54 = b[8] ^ b[11];
    uint64_t y55 = a[6] ^ a[9];
    uint64_t y56 = a[7] ^ a[10];
    uint64_t y57 = a[8] ^ a[11];
    uint64_t y58 = y57 & y54;
    uint64_t y59 = y57 & y52;
    uint64_t y60 = y57 & y53;
    uint64_t y61 = y55 & y54;
    uint64_t y62 = y56 & y54;
    uint64_t y63 = y56 & y53;
    uint64_t y64 = y56 & y52;
    uint64_t y65 = y55 & y53;
    uint64_t y66 = y55 & y52;
    uint64_t y67 = y65 ^ y64;
    uint64_t y68 = y62 ^ y60;
    uint64_t y69 = b[0] ^ b[3];
    uint64_t y70 = b[1] ^ b[4];
    uint64_t y71 = b[2] ^ b[5];
    uint64_t y72 = a[0] ^ a[3];
    uint64_t y73 = a[1] ^ a[4];
    uint64_t y74 = a[2] ^ a[5];
    uint64_t y75 = y74 & y71;
    uint64_t y76 = y74 & y69;
    uint64_t y77 = y74 & y70;
    uint64_t y78 = y72 & y71;
    uint64_t y79 = y73 & y71;
    uint64_t y80 = y73 & y70;
    uint64_t y81 = y73 & y69;
    uint64_t y82 = y72 & y70;
    uint64_t y83 = y72 & y69;
    uint64_t y84 = y82 ^ y81;
    uint64_t y85 = y79 ^ y77;
    uint64_t y86 = y83 ^ t[0];
    uint64_t y87 = y47 ^ t[1];
    uint64_t y88 = y48 ^ y46;
    uint64_t y89 = y49 ^ y75;
    uint64_t y90 = y50 ^ y66;
    uint64_t y91 = y51 ^ y49;
    uint64_t y92 = h21 ^ y50;
    uint64_t y93 = h22 ^ y58;
    t[3] = y86 ^ y46;
    t[4] = y87 ^ y84;
    uint64_t y96 = y88 ^ y85;
    uint64_t y97 = y89 ^ y47;
    uint64_t y98 = y90 ^ y48;
    uint64_t y99 = y91 ^ y67;
    uint64_t h18 = y92 ^ y68;
    uint64_t h19 = y93 ^ y51;
    uint64_t y102 = b[3] ^ b[9];
    uint64_t y103 = b[4] ^ b[10];
    uint64_t y104 = b[5] ^ b[11];
    uint64_t y105 = b[0] ^ b[6];
    uint64_t y106 = b[1] ^ b[7];
    uint64_t y107 = b[2] ^ b[8];
    uint64_t y108 = a[3] ^ a[9];
    uint64_t y109 = a[4] ^ a[10];
    uint64_t y110 = a[5] ^ a[11];
    uint64_t y111 = a[0] ^ a[6];
    uint64_t y112 = a[1] ^ a[7];
    uint64_t y113 = a[2] ^ a[8];
    uint64_t y114 = y113 & y107;
    uint64_t y115 = y113 & y105;
    uint64_t y116 = y113 & y106;
    uint64_t y117 = y111 & y107;
    uint64_t y118 = y112 & y107;
    uint64_t y119 = y112 & y106;
    uint64_t y120 = y112 & y105;
    uint64_t y121 = y111 & y106;
    uint64_t y122 = y111 & y105;
    uint64_t y123 = y121 ^ y120;
    uint64_t y124 = y118 ^ y116;
    uint64_t y125 = y110 & y104;
    uint64_t y126 = y110 & y102;
    uint64_t y127 = y110 & y103;
    uint64_t y128 = y108 & y104;
    uint64_t y129 = y109 & y104;
    uint64_t y130 = y109 & y103;
    uint64_t y131 = y109 & y102;
    uint64_t y132 = y108 & y103;
    uint64_t y133 = y108 & y102;
    uint64_t y134 = y132 ^ y131;
    uint64_t y135 = y129 ^ y127;
    uint64_t y136 = y105 ^ y102;
    uint64_t y137 = y106 ^ y103;
    uint64_t y138 = y107 ^ y104;
    uint64_t y139 = y111 ^ y108;
    uint64_t y140 = y112 ^ y109;
    uint64_t y141 = y113 ^ y110;
    uint64_t y142 = y141 & y138;
    uint64_t y143 = y141 & y136;
    uint64_t y144 = y141 & y137;
    uint64_t y145 = y139 & y138;
    uint64_t y146 = y140 & y138;
    uint64_t y147 = y140 & y137;
    uint64_t y148 = y140 & y136;
    uint64_t y149 = y139 & y137;
    uint64_t y150 = y139 & y136;
    uint64_t y151 = y149 ^ y148;
    uint64_t y152 = y146 ^ y144;
    uint64_t y153 = y124 ^ y133;
    uint64_t y154 = y114 ^ y134;
    uint64_t y155 = y150 ^ y122;
    uint64_t y156 = y151 ^ y123;
    uint64_t y157 = y152 ^ y135;
    uint64_t y158 = y142 ^ y125;
    uint64_t y159 = y155 ^ y153;
    uint64_t y160 = y156 ^ y154;
    uint64_t y161 = y157 ^ y153;
    uint64_t y162 = y158 ^ y154;
    uint64_t y163 = y122 ^ t[0];
    uint64_t y164 = y123 ^ t[1];
    uint64_t y165 = y98 ^ t[3];
    uint64_t y166 = y99 ^ t[4];
    uint64_t y167 = h18 ^ y96;
    uint64_t y168 = h19 ^ y97;
    uint64_t y169 = h21 ^ y135;
    uint64_t y170 = h22 ^ y125;
    t[6] = y163 ^ y96;
    t[7] = y164 ^ y97;
    t[9] = y165 ^ y159;
    t[10] = y166 ^ y160;
    uint64_t h12 = y167 ^ y161;
    uint64_t h13 = y168 ^ y162;
    uint64_t h15 = y169 ^ y98;
    uint64_t h16 = y170 ^ y99;
    uint64_t y179 = y2 ^ y4;
    uint64_t h20 = y6 ^ y179;
    uint64_t y181 = y36 ^ y38;
    t[2] = y40 ^ y181;
    uint64_t y183 = y16 ^ y18;
    uint64_t y184 = y25 ^ y27;
    uint64_t y185 = y29 ^ y184;
    uint64_t y186 = h20 ^ y183;
    uint64_t y187 = t[2] ^ y185;
    uint64_t y188 = y59 ^ y61;
    uint64_t y189 = y63 ^ y188;
    uint64_t h17 = y186 ^ y189;
    uint64_t y191 = y76 ^ y78;
    uint64_t y192 = y80 ^ y191;
    t[5] = y187 ^ y192;
    uint64_t y194 = y115 ^ y117;
    uint64_t y195 = y119 ^ y194;
    uint64_t y196 = y126 ^ y128;
    uint64_t y197 = y130 ^ y196;
    uint64_t y198 = y183 ^ y187;
    t[8] = y195 ^ y198;
    uint64_t y200 = y185 ^ y186;
    uint64_t h14 = y197 ^ y200;
    uint64_t y202 = h17 ^ t[5];
    uint64_t y203 = y195 ^ y143;
    uint64_t y206 = y145 ^ y147;
    uint64_t y204 = y197 ^ y206;
    uint64_t y205 = y204 ^ y203;
    t[11] = y205 ^ y202;
    
    /* Modulo reduction */
    h13   ^= h22; t[10] ^= h22;
    h12   ^= h21; t[ 9] ^= h21;
    t[11] ^= h20; t[ 8] ^= h20;
    t[10] ^= h19; t[ 7] ^= h19;
    t[ 9] ^= h18; t[ 6] ^= h18;
    t[ 8] ^= h17; t[ 5] ^= h17;
    t[ 7] ^= h16; t[ 4] ^= h16;
    t[ 6] ^= h15; t[ 3] ^= h15;
    t[ 5] ^= h14; t[ 2] ^= h14;
    t[ 4] ^= h13; t[ 1] ^= h13;
    t[ 3] ^= h12; t[ 0] ^= h12;
    
    c[ 0] = t[ 0]; c[ 1] = t[ 1];
    c[ 2] = t[ 2]; c[ 3] = t[ 3];
    c[ 4] = t[ 4]; c[ 5] = t[ 5];
    c[ 6] = t[ 6]; c[ 7] = t[ 7];
    c[ 8] = t[ 8]; c[ 9] = t[ 9];
    c[10] = t[10]; c[11] = t[11];
}

void bitslice_mul12_128(__m128i* d, const __m128i* a, const __m128i* b)
{
    __m128i c[12] = {{0}};
    
    /**
     * These sequences of & and ^ are obtained from here:
     * http://www.cs.yale.edu/homes/peralta/CircuitStuff/binary_pol_mult/CMT12.txt
     *
     * Circuit Minimization Work
     **/
    __m128i h22 = a[11] & b[11];
    __m128i y2 = a[11] & b[9];
    __m128i y3 = a[11] & b[10];
    __m128i y4 = a[9] & b[11];
    __m128i y5 = a[10] & b[11];
    __m128i y6 = a[10] & b[10];
    __m128i y7 = a[10] & b[9];
    __m128i y8 = a[9] & b[10];
    __m128i y9 = a[9] & b[9];
    __m128i y10 = y8 ^ y7;
    __m128i h21 = y5 ^ y3;
    __m128i y12 = a[8] & b[8];
    __m128i y13 = a[8] & b[6];
    __m128i y14 = a[8] & b[7];
    __m128i y15 = a[6] & b[8];
    __m128i y16 = y13 ^ y15;
    __m128i y17 = a[7] & b[8];
    __m128i y18 = a[7] & b[7];
    __m128i y19 = a[7] & b[6];
    __m128i y20 = a[6] & b[7];
    __m128i y21 = a[6] & b[6];
    __m128i y22 = y20 ^ y19;
    __m128i y23 = y17 ^ y14;
    __m128i y24 = a[5] & b[5];
    __m128i y25 = a[5] & b[3];
    __m128i y26 = a[5] & b[4];
    __m128i y27 = a[3] & b[5];
    __m128i y28 = a[4] & b[5];
    __m128i y29 = a[4] & b[4];
    __m128i y30 = a[4] & b[3];
    __m128i y31 = a[3] & b[4];
    __m128i y32 = a[3] & b[3];
    __m128i y33 = y31 ^ y30;
    __m128i y34 = y28 ^ y26;
    __m128i y35 = a[2] & b[2];
    __m128i y36 = a[2] & b[0];
    __m128i y37 = a[2] & b[1];
    __m128i y38 = a[0] & b[2];
    __m128i y39 = a[1] & b[2];
    __m128i y40 = a[1] & b[1];
    __m128i y41 = a[1] & b[0];
    __m128i y42 = a[0] & b[1];
    c[0] = a[0] & b[0];
    c[1] = y42 ^ y41;
    __m128i y45 = y39 ^ y37;
    __m128i y46 = y45 ^ y32;
    __m128i y47 = y35 ^ y33;
    __m128i y48 = y34 ^ y21;
    __m128i y49 = y24 ^ y22;
    __m128i y50 = y23 ^ y9;
    __m128i y51 = y12 ^ y10;
    __m128i y52 = b[6] ^ b[9];
    __m128i y53 = b[7] ^ b[10];
    __m128i y54 = b[8] ^ b[11];
    __m128i y55 = a[6] ^ a[9];
    __m128i y56 = a[7] ^ a[10];
    __m128i y57 = a[8] ^ a[11];
    __m128i y58 = y57 & y54;
    __m128i y59 = y57 & y52;
    __m128i y60 = y57 & y53;
    __m128i y61 = y55 & y54;
    __m128i y62 = y56 & y54;
    __m128i y63 = y56 & y53;
    __m128i y64 = y56 & y52;
    __m128i y65 = y55 & y53;
    __m128i y66 = y55 & y52;
    __m128i y67 = y65 ^ y64;
    __m128i y68 = y62 ^ y60;
    __m128i y69 = b[0] ^ b[3];
    __m128i y70 = b[1] ^ b[4];
    __m128i y71 = b[2] ^ b[5];
    __m128i y72 = a[0] ^ a[3];
    __m128i y73 = a[1] ^ a[4];
    __m128i y74 = a[2] ^ a[5];
    __m128i y75 = y74 & y71;
    __m128i y76 = y74 & y69;
    __m128i y77 = y74 & y70;
    __m128i y78 = y72 & y71;
    __m128i y79 = y73 & y71;
    __m128i y80 = y73 & y70;
    __m128i y81 = y73 & y69;
    __m128i y82 = y72 & y70;
    __m128i y83 = y72 & y69;
    __m128i y84 = y82 ^ y81;
    __m128i y85 = y79 ^ y77;
    __m128i y86 = y83 ^ c[0];
    __m128i y87 = y47 ^ c[1];
    __m128i y88 = y48 ^ y46;
    __m128i y89 = y49 ^ y75;
    __m128i y90 = y50 ^ y66;
    __m128i y91 = y51 ^ y49;
    __m128i y92 = h21 ^ y50;
    __m128i y93 = h22 ^ y58;
    c[3] = y86 ^ y46;
    c[4] = y87 ^ y84;
    __m128i y96 = y88 ^ y85;
    __m128i y97 = y89 ^ y47;
    __m128i y98 = y90 ^ y48;
    __m128i y99 = y91 ^ y67;
    __m128i h18 = y92 ^ y68;
    __m128i h19 = y93 ^ y51;
    __m128i y102 = b[3] ^ b[9];
    __m128i y103 = b[4] ^ b[10];
    __m128i y104 = b[5] ^ b[11];
    __m128i y105 = b[0] ^ b[6];
    __m128i y106 = b[1] ^ b[7];
    __m128i y107 = b[2] ^ b[8];
    __m128i y108 = a[3] ^ a[9];
    __m128i y109 = a[4] ^ a[10];
    __m128i y110 = a[5] ^ a[11];
    __m128i y111 = a[0] ^ a[6];
    __m128i y112 = a[1] ^ a[7];
    __m128i y113 = a[2] ^ a[8];
    __m128i y114 = y113 & y107;
    __m128i y115 = y113 & y105;
    __m128i y116 = y113 & y106;
    __m128i y117 = y111 & y107;
    __m128i y118 = y112 & y107;
    __m128i y119 = y112 & y106;
    __m128i y120 = y112 & y105;
    __m128i y121 = y111 & y106;
    __m128i y122 = y111 & y105;
    __m128i y123 = y121 ^ y120;
    __m128i y124 = y118 ^ y116;
    __m128i y125 = y110 & y104;
    __m128i y126 = y110 & y102;
    __m128i y127 = y110 & y103;
    __m128i y128 = y108 & y104;
    __m128i y129 = y109 & y104;
    __m128i y130 = y109 & y103;
    __m128i y131 = y109 & y102;
    __m128i y132 = y108 & y103;
    __m128i y133 = y108 & y102;
    __m128i y134 = y132 ^ y131;
    __m128i y135 = y129 ^ y127;
    __m128i y136 = y105 ^ y102;
    __m128i y137 = y106 ^ y103;
    __m128i y138 = y107 ^ y104;
    __m128i y139 = y111 ^ y108;
    __m128i y140 = y112 ^ y109;
    __m128i y141 = y113 ^ y110;
    __m128i y142 = y141 & y138;
    __m128i y143 = y141 & y136;
    __m128i y144 = y141 & y137;
    __m128i y145 = y139 & y138;
    __m128i y146 = y140 & y138;
    __m128i y147 = y140 & y137;
    __m128i y148 = y140 & y136;
    __m128i y149 = y139 & y137;
    __m128i y150 = y139 & y136;
    __m128i y151 = y149 ^ y148;
    __m128i y152 = y146 ^ y144;
    __m128i y153 = y124 ^ y133;
    __m128i y154 = y114 ^ y134;
    __m128i y155 = y150 ^ y122;
    __m128i y156 = y151 ^ y123;
    __m128i y157 = y152 ^ y135;
    __m128i y158 = y142 ^ y125;
    __m128i y159 = y155 ^ y153;
    __m128i y160 = y156 ^ y154;
    __m128i y161 = y157 ^ y153;
    __m128i y162 = y158 ^ y154;
    __m128i y163 = y122 ^ c[0];
    __m128i y164 = y123 ^ c[1];
    __m128i y165 = y98 ^ c[3];
    __m128i y166 = y99 ^ c[4];
    __m128i y167 = h18 ^ y96;
    __m128i y168 = h19 ^ y97;
    __m128i y169 = h21 ^ y135;
    __m128i y170 = h22 ^ y125;
    c[6] = y163 ^ y96;
    c[7] = y164 ^ y97;
    c[9] = y165 ^ y159;
    c[10] = y166 ^ y160;
    __m128i h12 = y167 ^ y161;
    __m128i h13 = y168 ^ y162;
    __m128i h15 = y169 ^ y98;
    __m128i h16 = y170 ^ y99;
    __m128i y179 = y2 ^ y4;
    __m128i h20 = y6 ^ y179;
    __m128i y181 = y36 ^ y38;
    c[2] = y40 ^ y181;
    __m128i y183 = y16 ^ y18;
    __m128i y184 = y25 ^ y27;
    __m128i y185 = y29 ^ y184;
    __m128i y186 = h20 ^ y183;
    __m128i y187 = c[2] ^ y185;
    __m128i y188 = y59 ^ y61;
    __m128i y189 = y63 ^ y188;
    __m128i h17 = y186 ^ y189;
    __m128i y191 = y76 ^ y78;
    __m128i y192 = y80 ^ y191;
    c[5] = y187 ^ y192;
    __m128i y194 = y115 ^ y117;
    __m128i y195 = y119 ^ y194;
    __m128i y196 = y126 ^ y128;
    __m128i y197 = y130 ^ y196;
    __m128i y198 = y183 ^ y187;
    c[8] = y195 ^ y198;
    __m128i y200 = y185 ^ y186;
    __m128i h14 = y197 ^ y200;
    __m128i y202 = h17 ^ c[5];
    __m128i y203 = y195 ^ y143;
    __m128i y206 = y145 ^ y147;
    __m128i y204 = y197 ^ y206;
    __m128i y205 = y204 ^ y203;
    c[11] = y205 ^ y202;
    
    /* Modulo reduction */
    h13   ^= h22; c[10] ^= h22;
    h12   ^= h21; c[ 9] ^= h21;
    c[11] ^= h20; c[ 8] ^= h20;
    c[10] ^= h19; c[ 7] ^= h19;
    c[ 9] ^= h18; c[ 6] ^= h18;
    c[ 8] ^= h17; c[ 5] ^= h17;
    c[ 7] ^= h16; c[ 4] ^= h16;
    c[ 6] ^= h15; c[ 3] ^= h15;
    c[ 5] ^= h14; c[ 2] ^= h14;
    c[ 4] ^= h13; c[ 1] ^= h13;
    c[ 3] ^= h12; c[ 0] ^= h12;
    
    d[ 0] = c[ 0]; d[ 1] = c[ 1];
    d[ 2] = c[ 2]; d[ 3] = c[ 3];
    d[ 4] = c[ 4]; d[ 5] = c[ 5];
    d[ 6] = c[ 6]; d[ 7] = c[ 7];
    d[ 8] = c[ 8]; d[ 9] = c[ 9];
    d[10] = c[10]; d[11] = c[11];
}

void bitslice_mul12_256(__m256i* d, const __m256i* a, const __m256i* b)
{
    __m256i c[12] = {{0}};
    
    /**
     * These sequences of & and ^ are obtained from here:
     * http://www.cs.yale.edu/homes/peralta/CircuitStuff/binary_pol_mult/CMT12.txt
     *
     * Circuit Minimization Work
     **/
    __m256i h22 = a[11] & b[11];
    __m256i y2 = a[11] & b[9];
    __m256i y3 = a[11] & b[10];
    __m256i y4 = a[9] & b[11];
    __m256i y5 = a[10] & b[11];
    __m256i y6 = a[10] & b[10];
    __m256i y7 = a[10] & b[9];
    __m256i y8 = a[9] & b[10];
    __m256i y9 = a[9] & b[9];
    __m256i y10 = y8 ^ y7;
    __m256i h21 = y5 ^ y3;
    __m256i y12 = a[8] & b[8];
    __m256i y13 = a[8] & b[6];
    __m256i y14 = a[8] & b[7];
    __m256i y15 = a[6] & b[8];
    __m256i y16 = y13 ^ y15;
    __m256i y17 = a[7] & b[8];
    __m256i y18 = a[7] & b[7];
    __m256i y19 = a[7] & b[6];
    __m256i y20 = a[6] & b[7];
    __m256i y21 = a[6] & b[6];
    __m256i y22 = y20 ^ y19;
    __m256i y23 = y17 ^ y14;
    __m256i y24 = a[5] & b[5];
    __m256i y25 = a[5] & b[3];
    __m256i y26 = a[5] & b[4];
    __m256i y27 = a[3] & b[5];
    __m256i y28 = a[4] & b[5];
    __m256i y29 = a[4] & b[4];
    __m256i y30 = a[4] & b[3];
    __m256i y31 = a[3] & b[4];
    __m256i y32 = a[3] & b[3];
    __m256i y33 = y31 ^ y30;
    __m256i y34 = y28 ^ y26;
    __m256i y35 = a[2] & b[2];
    __m256i y36 = a[2] & b[0];
    __m256i y37 = a[2] & b[1];
    __m256i y38 = a[0] & b[2];
    __m256i y39 = a[1] & b[2];
    __m256i y40 = a[1] & b[1];
    __m256i y41 = a[1] & b[0];
    __m256i y42 = a[0] & b[1];
    c[0] = a[0] & b[0];
    c[1] = y42 ^ y41;
    __m256i y45 = y39 ^ y37;
    __m256i y46 = y45 ^ y32;
    __m256i y47 = y35 ^ y33;
    __m256i y48 = y34 ^ y21;
    __m256i y49 = y24 ^ y22;
    __m256i y50 = y23 ^ y9;
    __m256i y51 = y12 ^ y10;
    __m256i y52 = b[6] ^ b[9];
    __m256i y53 = b[7] ^ b[10];
    __m256i y54 = b[8] ^ b[11];
    __m256i y55 = a[6] ^ a[9];
    __m256i y56 = a[7] ^ a[10];
    __m256i y57 = a[8] ^ a[11];
    __m256i y58 = y57 & y54;
    __m256i y59 = y57 & y52;
    __m256i y60 = y57 & y53;
    __m256i y61 = y55 & y54;
    __m256i y62 = y56 & y54;
    __m256i y63 = y56 & y53;
    __m256i y64 = y56 & y52;
    __m256i y65 = y55 & y53;
    __m256i y66 = y55 & y52;
    __m256i y67 = y65 ^ y64;
    __m256i y68 = y62 ^ y60;
    __m256i y69 = b[0] ^ b[3];
    __m256i y70 = b[1] ^ b[4];
    __m256i y71 = b[2] ^ b[5];
    __m256i y72 = a[0] ^ a[3];
    __m256i y73 = a[1] ^ a[4];
    __m256i y74 = a[2] ^ a[5];
    __m256i y75 = y74 & y71;
    __m256i y76 = y74 & y69;
    __m256i y77 = y74 & y70;
    __m256i y78 = y72 & y71;
    __m256i y79 = y73 & y71;
    __m256i y80 = y73 & y70;
    __m256i y81 = y73 & y69;
    __m256i y82 = y72 & y70;
    __m256i y83 = y72 & y69;
    __m256i y84 = y82 ^ y81;
    __m256i y85 = y79 ^ y77;
    __m256i y86 = y83 ^ c[0];
    __m256i y87 = y47 ^ c[1];
    __m256i y88 = y48 ^ y46;
    __m256i y89 = y49 ^ y75;
    __m256i y90 = y50 ^ y66;
    __m256i y91 = y51 ^ y49;
    __m256i y92 = h21 ^ y50;
    __m256i y93 = h22 ^ y58;
    c[3] = y86 ^ y46;
    c[4] = y87 ^ y84;
    __m256i y96 = y88 ^ y85;
    __m256i y97 = y89 ^ y47;
    __m256i y98 = y90 ^ y48;
    __m256i y99 = y91 ^ y67;
    __m256i h18 = y92 ^ y68;
    __m256i h19 = y93 ^ y51;
    __m256i y102 = b[3] ^ b[9];
    __m256i y103 = b[4] ^ b[10];
    __m256i y104 = b[5] ^ b[11];
    __m256i y105 = b[0] ^ b[6];
    __m256i y106 = b[1] ^ b[7];
    __m256i y107 = b[2] ^ b[8];
    __m256i y108 = a[3] ^ a[9];
    __m256i y109 = a[4] ^ a[10];
    __m256i y110 = a[5] ^ a[11];
    __m256i y111 = a[0] ^ a[6];
    __m256i y112 = a[1] ^ a[7];
    __m256i y113 = a[2] ^ a[8];
    __m256i y114 = y113 & y107;
    __m256i y115 = y113 & y105;
    __m256i y116 = y113 & y106;
    __m256i y117 = y111 & y107;
    __m256i y118 = y112 & y107;
    __m256i y119 = y112 & y106;
    __m256i y120 = y112 & y105;
    __m256i y121 = y111 & y106;
    __m256i y122 = y111 & y105;
    __m256i y123 = y121 ^ y120;
    __m256i y124 = y118 ^ y116;
    __m256i y125 = y110 & y104;
    __m256i y126 = y110 & y102;
    __m256i y127 = y110 & y103;
    __m256i y128 = y108 & y104;
    __m256i y129 = y109 & y104;
    __m256i y130 = y109 & y103;
    __m256i y131 = y109 & y102;
    __m256i y132 = y108 & y103;
    __m256i y133 = y108 & y102;
    __m256i y134 = y132 ^ y131;
    __m256i y135 = y129 ^ y127;
    __m256i y136 = y105 ^ y102;
    __m256i y137 = y106 ^ y103;
    __m256i y138 = y107 ^ y104;
    __m256i y139 = y111 ^ y108;
    __m256i y140 = y112 ^ y109;
    __m256i y141 = y113 ^ y110;
    __m256i y142 = y141 & y138;
    __m256i y143 = y141 & y136;
    __m256i y144 = y141 & y137;
    __m256i y145 = y139 & y138;
    __m256i y146 = y140 & y138;
    __m256i y147 = y140 & y137;
    __m256i y148 = y140 & y136;
    __m256i y149 = y139 & y137;
    __m256i y150 = y139 & y136;
    __m256i y151 = y149 ^ y148;
    __m256i y152 = y146 ^ y144;
    __m256i y153 = y124 ^ y133;
    __m256i y154 = y114 ^ y134;
    __m256i y155 = y150 ^ y122;
    __m256i y156 = y151 ^ y123;
    __m256i y157 = y152 ^ y135;
    __m256i y158 = y142 ^ y125;
    __m256i y159 = y155 ^ y153;
    __m256i y160 = y156 ^ y154;
    __m256i y161 = y157 ^ y153;
    __m256i y162 = y158 ^ y154;
    __m256i y163 = y122 ^ c[0];
    __m256i y164 = y123 ^ c[1];
    __m256i y165 = y98 ^ c[3];
    __m256i y166 = y99 ^ c[4];
    __m256i y167 = h18 ^ y96;
    __m256i y168 = h19 ^ y97;
    __m256i y169 = h21 ^ y135;
    __m256i y170 = h22 ^ y125;
    c[6] = y163 ^ y96;
    c[7] = y164 ^ y97;
    c[9] = y165 ^ y159;
    c[10] = y166 ^ y160;
    __m256i h12 = y167 ^ y161;
    __m256i h13 = y168 ^ y162;
    __m256i h15 = y169 ^ y98;
    __m256i h16 = y170 ^ y99;
    __m256i y179 = y2 ^ y4;
    __m256i h20 = y6 ^ y179;
    __m256i y181 = y36 ^ y38;
    c[2] = y40 ^ y181;
    __m256i y183 = y16 ^ y18;
    __m256i y184 = y25 ^ y27;
    __m256i y185 = y29 ^ y184;
    __m256i y186 = h20 ^ y183;
    __m256i y187 = c[2] ^ y185;
    __m256i y188 = y59 ^ y61;
    __m256i y189 = y63 ^ y188;
    __m256i h17 = y186 ^ y189;
    __m256i y191 = y76 ^ y78;
    __m256i y192 = y80 ^ y191;
    c[5] = y187 ^ y192;
    __m256i y194 = y115 ^ y117;
    __m256i y195 = y119 ^ y194;
    __m256i y196 = y126 ^ y128;
    __m256i y197 = y130 ^ y196;
    __m256i y198 = y183 ^ y187;
    c[8] = y195 ^ y198;
    __m256i y200 = y185 ^ y186;
    __m256i h14 = y197 ^ y200;
    __m256i y202 = h17 ^ c[5];
    __m256i y203 = y195 ^ y143;
    __m256i y206 = y145 ^ y147;
    __m256i y204 = y197 ^ y206;
    __m256i y205 = y204 ^ y203;
    c[11] = y205 ^ y202;
    
    /* Modulo reduction */
    h13   ^= h22; c[10] ^= h22;
    h12   ^= h21; c[ 9] ^= h21;
    c[11] ^= h20; c[ 8] ^= h20;
    c[10] ^= h19; c[ 7] ^= h19;
    c[ 9] ^= h18; c[ 6] ^= h18;
    c[ 8] ^= h17; c[ 5] ^= h17;
    c[ 7] ^= h16; c[ 4] ^= h16;
    c[ 6] ^= h15; c[ 3] ^= h15;
    c[ 5] ^= h14; c[ 2] ^= h14;
    c[ 4] ^= h13; c[ 1] ^= h13;
    c[ 3] ^= h12; c[ 0] ^= h12;
    
    d[ 0] = c[ 0]; d[ 1] = c[ 1];
    d[ 2] = c[ 2]; d[ 3] = c[ 3];
    d[ 4] = c[ 4]; d[ 5] = c[ 5];
    d[ 6] = c[ 6]; d[ 7] = c[ 7];
    d[ 8] = c[ 8]; d[ 9] = c[ 9];
    d[10] = c[10]; d[11] = c[11];
}
