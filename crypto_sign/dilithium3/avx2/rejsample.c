#include <stdint.h>
#include <immintrin.h>
#include "params.h"

#define DBENCH_START()
#define DBENCH_STOP(t)

static const unsigned char idx[256][8] = {
  { 0,  0,  0,  0,  0,  0,  0,  0},
  { 0,  0,  0,  0,  0,  0,  0,  0},
  { 1,  0,  0,  0,  0,  0,  0,  0},
  { 0,  1,  0,  0,  0,  0,  0,  0},
  { 2,  0,  0,  0,  0,  0,  0,  0},
  { 0,  2,  0,  0,  0,  0,  0,  0},
  { 1,  2,  0,  0,  0,  0,  0,  0},
  { 0,  1,  2,  0,  0,  0,  0,  0},
  { 3,  0,  0,  0,  0,  0,  0,  0},
  { 0,  3,  0,  0,  0,  0,  0,  0},
  { 1,  3,  0,  0,  0,  0,  0,  0},
  { 0,  1,  3,  0,  0,  0,  0,  0},
  { 2,  3,  0,  0,  0,  0,  0,  0},
  { 0,  2,  3,  0,  0,  0,  0,  0},
  { 1,  2,  3,  0,  0,  0,  0,  0},
  { 0,  1,  2,  3,  0,  0,  0,  0},
  { 4,  0,  0,  0,  0,  0,  0,  0},
  { 0,  4,  0,  0,  0,  0,  0,  0},
  { 1,  4,  0,  0,  0,  0,  0,  0},
  { 0,  1,  4,  0,  0,  0,  0,  0},
  { 2,  4,  0,  0,  0,  0,  0,  0},
  { 0,  2,  4,  0,  0,  0,  0,  0},
  { 1,  2,  4,  0,  0,  0,  0,  0},
  { 0,  1,  2,  4,  0,  0,  0,  0},
  { 3,  4,  0,  0,  0,  0,  0,  0},
  { 0,  3,  4,  0,  0,  0,  0,  0},
  { 1,  3,  4,  0,  0,  0,  0,  0},
  { 0,  1,  3,  4,  0,  0,  0,  0},
  { 2,  3,  4,  0,  0,  0,  0,  0},
  { 0,  2,  3,  4,  0,  0,  0,  0},
  { 1,  2,  3,  4,  0,  0,  0,  0},
  { 0,  1,  2,  3,  4,  0,  0,  0},
  { 5,  0,  0,  0,  0,  0,  0,  0},
  { 0,  5,  0,  0,  0,  0,  0,  0},
  { 1,  5,  0,  0,  0,  0,  0,  0},
  { 0,  1,  5,  0,  0,  0,  0,  0},
  { 2,  5,  0,  0,  0,  0,  0,  0},
  { 0,  2,  5,  0,  0,  0,  0,  0},
  { 1,  2,  5,  0,  0,  0,  0,  0},
  { 0,  1,  2,  5,  0,  0,  0,  0},
  { 3,  5,  0,  0,  0,  0,  0,  0},
  { 0,  3,  5,  0,  0,  0,  0,  0},
  { 1,  3,  5,  0,  0,  0,  0,  0},
  { 0,  1,  3,  5,  0,  0,  0,  0},
  { 2,  3,  5,  0,  0,  0,  0,  0},
  { 0,  2,  3,  5,  0,  0,  0,  0},
  { 1,  2,  3,  5,  0,  0,  0,  0},
  { 0,  1,  2,  3,  5,  0,  0,  0},
  { 4,  5,  0,  0,  0,  0,  0,  0},
  { 0,  4,  5,  0,  0,  0,  0,  0},
  { 1,  4,  5,  0,  0,  0,  0,  0},
  { 0,  1,  4,  5,  0,  0,  0,  0},
  { 2,  4,  5,  0,  0,  0,  0,  0},
  { 0,  2,  4,  5,  0,  0,  0,  0},
  { 1,  2,  4,  5,  0,  0,  0,  0},
  { 0,  1,  2,  4,  5,  0,  0,  0},
  { 3,  4,  5,  0,  0,  0,  0,  0},
  { 0,  3,  4,  5,  0,  0,  0,  0},
  { 1,  3,  4,  5,  0,  0,  0,  0},
  { 0,  1,  3,  4,  5,  0,  0,  0},
  { 2,  3,  4,  5,  0,  0,  0,  0},
  { 0,  2,  3,  4,  5,  0,  0,  0},
  { 1,  2,  3,  4,  5,  0,  0,  0},
  { 0,  1,  2,  3,  4,  5,  0,  0},
  { 6,  0,  0,  0,  0,  0,  0,  0},
  { 0,  6,  0,  0,  0,  0,  0,  0},
  { 1,  6,  0,  0,  0,  0,  0,  0},
  { 0,  1,  6,  0,  0,  0,  0,  0},
  { 2,  6,  0,  0,  0,  0,  0,  0},
  { 0,  2,  6,  0,  0,  0,  0,  0},
  { 1,  2,  6,  0,  0,  0,  0,  0},
  { 0,  1,  2,  6,  0,  0,  0,  0},
  { 3,  6,  0,  0,  0,  0,  0,  0},
  { 0,  3,  6,  0,  0,  0,  0,  0},
  { 1,  3,  6,  0,  0,  0,  0,  0},
  { 0,  1,  3,  6,  0,  0,  0,  0},
  { 2,  3,  6,  0,  0,  0,  0,  0},
  { 0,  2,  3,  6,  0,  0,  0,  0},
  { 1,  2,  3,  6,  0,  0,  0,  0},
  { 0,  1,  2,  3,  6,  0,  0,  0},
  { 4,  6,  0,  0,  0,  0,  0,  0},
  { 0,  4,  6,  0,  0,  0,  0,  0},
  { 1,  4,  6,  0,  0,  0,  0,  0},
  { 0,  1,  4,  6,  0,  0,  0,  0},
  { 2,  4,  6,  0,  0,  0,  0,  0},
  { 0,  2,  4,  6,  0,  0,  0,  0},
  { 1,  2,  4,  6,  0,  0,  0,  0},
  { 0,  1,  2,  4,  6,  0,  0,  0},
  { 3,  4,  6,  0,  0,  0,  0,  0},
  { 0,  3,  4,  6,  0,  0,  0,  0},
  { 1,  3,  4,  6,  0,  0,  0,  0},
  { 0,  1,  3,  4,  6,  0,  0,  0},
  { 2,  3,  4,  6,  0,  0,  0,  0},
  { 0,  2,  3,  4,  6,  0,  0,  0},
  { 1,  2,  3,  4,  6,  0,  0,  0},
  { 0,  1,  2,  3,  4,  6,  0,  0},
  { 5,  6,  0,  0,  0,  0,  0,  0},
  { 0,  5,  6,  0,  0,  0,  0,  0},
  { 1,  5,  6,  0,  0,  0,  0,  0},
  { 0,  1,  5,  6,  0,  0,  0,  0},
  { 2,  5,  6,  0,  0,  0,  0,  0},
  { 0,  2,  5,  6,  0,  0,  0,  0},
  { 1,  2,  5,  6,  0,  0,  0,  0},
  { 0,  1,  2,  5,  6,  0,  0,  0},
  { 3,  5,  6,  0,  0,  0,  0,  0},
  { 0,  3,  5,  6,  0,  0,  0,  0},
  { 1,  3,  5,  6,  0,  0,  0,  0},
  { 0,  1,  3,  5,  6,  0,  0,  0},
  { 2,  3,  5,  6,  0,  0,  0,  0},
  { 0,  2,  3,  5,  6,  0,  0,  0},
  { 1,  2,  3,  5,  6,  0,  0,  0},
  { 0,  1,  2,  3,  5,  6,  0,  0},
  { 4,  5,  6,  0,  0,  0,  0,  0},
  { 0,  4,  5,  6,  0,  0,  0,  0},
  { 1,  4,  5,  6,  0,  0,  0,  0},
  { 0,  1,  4,  5,  6,  0,  0,  0},
  { 2,  4,  5,  6,  0,  0,  0,  0},
  { 0,  2,  4,  5,  6,  0,  0,  0},
  { 1,  2,  4,  5,  6,  0,  0,  0},
  { 0,  1,  2,  4,  5,  6,  0,  0},
  { 3,  4,  5,  6,  0,  0,  0,  0},
  { 0,  3,  4,  5,  6,  0,  0,  0},
  { 1,  3,  4,  5,  6,  0,  0,  0},
  { 0,  1,  3,  4,  5,  6,  0,  0},
  { 2,  3,  4,  5,  6,  0,  0,  0},
  { 0,  2,  3,  4,  5,  6,  0,  0},
  { 1,  2,  3,  4,  5,  6,  0,  0},
  { 0,  1,  2,  3,  4,  5,  6,  0},
  { 7,  0,  0,  0,  0,  0,  0,  0},
  { 0,  7,  0,  0,  0,  0,  0,  0},
  { 1,  7,  0,  0,  0,  0,  0,  0},
  { 0,  1,  7,  0,  0,  0,  0,  0},
  { 2,  7,  0,  0,  0,  0,  0,  0},
  { 0,  2,  7,  0,  0,  0,  0,  0},
  { 1,  2,  7,  0,  0,  0,  0,  0},
  { 0,  1,  2,  7,  0,  0,  0,  0},
  { 3,  7,  0,  0,  0,  0,  0,  0},
  { 0,  3,  7,  0,  0,  0,  0,  0},
  { 1,  3,  7,  0,  0,  0,  0,  0},
  { 0,  1,  3,  7,  0,  0,  0,  0},
  { 2,  3,  7,  0,  0,  0,  0,  0},
  { 0,  2,  3,  7,  0,  0,  0,  0},
  { 1,  2,  3,  7,  0,  0,  0,  0},
  { 0,  1,  2,  3,  7,  0,  0,  0},
  { 4,  7,  0,  0,  0,  0,  0,  0},
  { 0,  4,  7,  0,  0,  0,  0,  0},
  { 1,  4,  7,  0,  0,  0,  0,  0},
  { 0,  1,  4,  7,  0,  0,  0,  0},
  { 2,  4,  7,  0,  0,  0,  0,  0},
  { 0,  2,  4,  7,  0,  0,  0,  0},
  { 1,  2,  4,  7,  0,  0,  0,  0},
  { 0,  1,  2,  4,  7,  0,  0,  0},
  { 3,  4,  7,  0,  0,  0,  0,  0},
  { 0,  3,  4,  7,  0,  0,  0,  0},
  { 1,  3,  4,  7,  0,  0,  0,  0},
  { 0,  1,  3,  4,  7,  0,  0,  0},
  { 2,  3,  4,  7,  0,  0,  0,  0},
  { 0,  2,  3,  4,  7,  0,  0,  0},
  { 1,  2,  3,  4,  7,  0,  0,  0},
  { 0,  1,  2,  3,  4,  7,  0,  0},
  { 5,  7,  0,  0,  0,  0,  0,  0},
  { 0,  5,  7,  0,  0,  0,  0,  0},
  { 1,  5,  7,  0,  0,  0,  0,  0},
  { 0,  1,  5,  7,  0,  0,  0,  0},
  { 2,  5,  7,  0,  0,  0,  0,  0},
  { 0,  2,  5,  7,  0,  0,  0,  0},
  { 1,  2,  5,  7,  0,  0,  0,  0},
  { 0,  1,  2,  5,  7,  0,  0,  0},
  { 3,  5,  7,  0,  0,  0,  0,  0},
  { 0,  3,  5,  7,  0,  0,  0,  0},
  { 1,  3,  5,  7,  0,  0,  0,  0},
  { 0,  1,  3,  5,  7,  0,  0,  0},
  { 2,  3,  5,  7,  0,  0,  0,  0},
  { 0,  2,  3,  5,  7,  0,  0,  0},
  { 1,  2,  3,  5,  7,  0,  0,  0},
  { 0,  1,  2,  3,  5,  7,  0,  0},
  { 4,  5,  7,  0,  0,  0,  0,  0},
  { 0,  4,  5,  7,  0,  0,  0,  0},
  { 1,  4,  5,  7,  0,  0,  0,  0},
  { 0,  1,  4,  5,  7,  0,  0,  0},
  { 2,  4,  5,  7,  0,  0,  0,  0},
  { 0,  2,  4,  5,  7,  0,  0,  0},
  { 1,  2,  4,  5,  7,  0,  0,  0},
  { 0,  1,  2,  4,  5,  7,  0,  0},
  { 3,  4,  5,  7,  0,  0,  0,  0},
  { 0,  3,  4,  5,  7,  0,  0,  0},
  { 1,  3,  4,  5,  7,  0,  0,  0},
  { 0,  1,  3,  4,  5,  7,  0,  0},
  { 2,  3,  4,  5,  7,  0,  0,  0},
  { 0,  2,  3,  4,  5,  7,  0,  0},
  { 1,  2,  3,  4,  5,  7,  0,  0},
  { 0,  1,  2,  3,  4,  5,  7,  0},
  { 6,  7,  0,  0,  0,  0,  0,  0},
  { 0,  6,  7,  0,  0,  0,  0,  0},
  { 1,  6,  7,  0,  0,  0,  0,  0},
  { 0,  1,  6,  7,  0,  0,  0,  0},
  { 2,  6,  7,  0,  0,  0,  0,  0},
  { 0,  2,  6,  7,  0,  0,  0,  0},
  { 1,  2,  6,  7,  0,  0,  0,  0},
  { 0,  1,  2,  6,  7,  0,  0,  0},
  { 3,  6,  7,  0,  0,  0,  0,  0},
  { 0,  3,  6,  7,  0,  0,  0,  0},
  { 1,  3,  6,  7,  0,  0,  0,  0},
  { 0,  1,  3,  6,  7,  0,  0,  0},
  { 2,  3,  6,  7,  0,  0,  0,  0},
  { 0,  2,  3,  6,  7,  0,  0,  0},
  { 1,  2,  3,  6,  7,  0,  0,  0},
  { 0,  1,  2,  3,  6,  7,  0,  0},
  { 4,  6,  7,  0,  0,  0,  0,  0},
  { 0,  4,  6,  7,  0,  0,  0,  0},
  { 1,  4,  6,  7,  0,  0,  0,  0},
  { 0,  1,  4,  6,  7,  0,  0,  0},
  { 2,  4,  6,  7,  0,  0,  0,  0},
  { 0,  2,  4,  6,  7,  0,  0,  0},
  { 1,  2,  4,  6,  7,  0,  0,  0},
  { 0,  1,  2,  4,  6,  7,  0,  0},
  { 3,  4,  6,  7,  0,  0,  0,  0},
  { 0,  3,  4,  6,  7,  0,  0,  0},
  { 1,  3,  4,  6,  7,  0,  0,  0},
  { 0,  1,  3,  4,  6,  7,  0,  0},
  { 2,  3,  4,  6,  7,  0,  0,  0},
  { 0,  2,  3,  4,  6,  7,  0,  0},
  { 1,  2,  3,  4,  6,  7,  0,  0},
  { 0,  1,  2,  3,  4,  6,  7,  0},
  { 5,  6,  7,  0,  0,  0,  0,  0},
  { 0,  5,  6,  7,  0,  0,  0,  0},
  { 1,  5,  6,  7,  0,  0,  0,  0},
  { 0,  1,  5,  6,  7,  0,  0,  0},
  { 2,  5,  6,  7,  0,  0,  0,  0},
  { 0,  2,  5,  6,  7,  0,  0,  0},
  { 1,  2,  5,  6,  7,  0,  0,  0},
  { 0,  1,  2,  5,  6,  7,  0,  0},
  { 3,  5,  6,  7,  0,  0,  0,  0},
  { 0,  3,  5,  6,  7,  0,  0,  0},
  { 1,  3,  5,  6,  7,  0,  0,  0},
  { 0,  1,  3,  5,  6,  7,  0,  0},
  { 2,  3,  5,  6,  7,  0,  0,  0},
  { 0,  2,  3,  5,  6,  7,  0,  0},
  { 1,  2,  3,  5,  6,  7,  0,  0},
  { 0,  1,  2,  3,  5,  6,  7,  0},
  { 4,  5,  6,  7,  0,  0,  0,  0},
  { 0,  4,  5,  6,  7,  0,  0,  0},
  { 1,  4,  5,  6,  7,  0,  0,  0},
  { 0,  1,  4,  5,  6,  7,  0,  0},
  { 2,  4,  5,  6,  7,  0,  0,  0},
  { 0,  2,  4,  5,  6,  7,  0,  0},
  { 1,  2,  4,  5,  6,  7,  0,  0},
  { 0,  1,  2,  4,  5,  6,  7,  0},
  { 3,  4,  5,  6,  7,  0,  0,  0},
  { 0,  3,  4,  5,  6,  7,  0,  0},
  { 1,  3,  4,  5,  6,  7,  0,  0},
  { 0,  1,  3,  4,  5,  6,  7,  0},
  { 2,  3,  4,  5,  6,  7,  0,  0},
  { 0,  2,  3,  4,  5,  6,  7,  0},
  { 1,  2,  3,  4,  5,  6,  7,  0},
  { 0,  1,  2,  3,  4,  5,  6,  7}
};

unsigned int rej_uniform(uint32_t *r,
                         unsigned int len,
                         const unsigned char *buf,
                         unsigned int buflen)
{
  unsigned int i, ctr, pos;
  uint32_t vec[8];
  __m256i d, tmp;
  uint32_t good;
  const __m256i bound = _mm256_set1_epi32(Q);
  DBENCH_START();

  ctr = pos = 0;
  while(ctr + 8 <= len && pos + 24 <= buflen) {
    for(i = 0; i < 8; i++) {
      vec[i]  = buf[pos++];
      vec[i] |= (uint32_t)buf[pos++] << 8;
      vec[i] |= (uint32_t)buf[pos++] << 16;
      vec[i] &= 0x7FFFFF;
    }

    d = _mm256_loadu_si256((__m256i *)vec);
    tmp = _mm256_cmpgt_epi32(bound, d);
    good = _mm256_movemask_ps((__m256)tmp);

    __m128i rid = _mm_loadl_epi64((__m128i *)&idx[good]);
    tmp = _mm256_cvtepu8_epi32(rid);
    d = _mm256_permutevar8x32_epi32(d, tmp);
    _mm256_storeu_si256((__m256i *)&r[ctr], d);
    ctr += __builtin_popcount(good);
  }

  while(ctr < len && pos + 3 <= buflen) {
    vec[0]  = buf[pos++];
    vec[0] |= (uint32_t)buf[pos++] << 8;
    vec[0] |= (uint32_t)buf[pos++] << 16;
    vec[0] &= 0x7FFFFF;

    if(vec[0] < Q)
      r[ctr++] = vec[0];
  }

  DBENCH_STOP(*tsample);
  return ctr;
}

unsigned int rej_eta(uint32_t *r,
                     unsigned int len,
                     const unsigned char *buf,
                     unsigned int buflen)
{
  unsigned int i, ctr, pos;
  uint8_t vec[32];
  __m256i tmp0, tmp1;
  __m128i d0, d1, rid;
  uint32_t good;
  const __m256i bound = _mm256_set1_epi8(2*ETA + 1);
  const __m256i off = _mm256_set1_epi32(Q + ETA);
  DBENCH_START();

  ctr = pos = 0;
  while(ctr + 32 <= len && pos + 16 <= buflen) {
    for(i = 0; i < 16; i++) {
#if ETA <= 3
      vec[2*i+0] = buf[pos] & 0x07;
      vec[2*i+1] = buf[pos++] >> 5;
#else
      vec[2*i+0] = buf[pos] & 0x0F;
      vec[2*i+1] = buf[pos++] >> 4;
#endif
    }

    tmp0 = _mm256_loadu_si256((__m256i *)vec);
    tmp1 = _mm256_cmpgt_epi8(bound, tmp0);
    good = _mm256_movemask_epi8(tmp1);

    d0 = _mm256_castsi256_si128(tmp0);
    rid = _mm_loadl_epi64((__m128i *)&idx[good & 0xFF]);
    d1 = _mm_shuffle_epi8(d0, rid);
    tmp1 = _mm256_cvtepu8_epi32(d1);
    tmp1 = _mm256_sub_epi32(off, tmp1);
    _mm256_storeu_si256((__m256i *)&r[ctr], tmp1);
    ctr += __builtin_popcount(good & 0xFF);

    d0 = _mm_bsrli_si128(d0, 8);
    rid = _mm_loadl_epi64((__m128i *)&idx[(good >> 8) & 0xFF]);
    d1 = _mm_shuffle_epi8(d0, rid);
    tmp1 = _mm256_cvtepu8_epi32(d1);
    tmp1 = _mm256_sub_epi32(off, tmp1);
    _mm256_storeu_si256((__m256i *)&r[ctr], tmp1);
    ctr += __builtin_popcount((good >> 8) & 0xFF);

    d0 = _mm256_extracti128_si256(tmp0, 1);
    rid = _mm_loadl_epi64((__m128i *)&idx[(good >> 16) & 0xFF]);
    d1 = _mm_shuffle_epi8(d0, rid);
    tmp1 = _mm256_cvtepu8_epi32(d1);
    tmp1 = _mm256_sub_epi32(off, tmp1);
    _mm256_storeu_si256((__m256i *)&r[ctr], tmp1);
    ctr += __builtin_popcount((good >> 16) & 0xFF);

    d0 = _mm_bsrli_si128(d0, 8);
    rid = _mm_loadl_epi64((__m128i *)&idx[(good >> 24) & 0xFF]);
    d1 = _mm_shuffle_epi8(d0, rid);
    tmp1 = _mm256_cvtepu8_epi32(d1);
    tmp1 = _mm256_sub_epi32(off, tmp1);
    _mm256_storeu_si256((__m256i *)&r[ctr], tmp1);
    ctr += __builtin_popcount((good >> 24) & 0xFF);
  }

  while(ctr < len && pos < buflen) {
#if ETA <= 3
    vec[0] = buf[pos] & 0x07;
    vec[1] = buf[pos++] >> 5;
#else
    vec[0] = buf[pos] & 0x0F;
    vec[1] = buf[pos++] >> 4;
#endif

    if(vec[0] <= 2*ETA)
      r[ctr++] = Q + ETA - vec[0];
    if(vec[1] <= 2*ETA && ctr < len)
      r[ctr++] = Q + ETA - vec[1];
  }

  DBENCH_STOP(*tsample);
  return ctr;
}

unsigned int rej_gamma1m1(uint32_t *r,
                          unsigned int len,
                          const unsigned char *buf,
                          unsigned int buflen)
{
  unsigned int i, ctr, pos;
  uint32_t vec[8];
  __m256i d, tmp;
  uint32_t good;
  const __m256i bound = _mm256_set1_epi32(2*GAMMA1 - 1);
  const __m256i off = _mm256_set1_epi32(Q + GAMMA1 - 1);
  DBENCH_START();

  ctr = pos = 0;
  while(ctr + 8 <= len && pos + 20 <= buflen) {
    for(i = 0; i < 4; i++) {
      vec[2*i+0]  = buf[pos + 0];
      vec[2*i+0] |= (uint32_t)buf[pos + 1] << 8;
      vec[2*i+0] |= (uint32_t)buf[pos + 2] << 16;
      vec[2*i+0] &= 0xFFFFF;

      vec[2*i+1]  = buf[pos + 2] >> 4;
      vec[2*i+1] |= (uint32_t)buf[pos + 3] << 4;
      vec[2*i+1] |= (uint32_t)buf[pos + 4] << 12;

      pos += 5;
    }

    d = _mm256_loadu_si256((__m256i *)vec);
    tmp = _mm256_cmpgt_epi32(bound, d);
    good = _mm256_movemask_ps((__m256)tmp);
    d = _mm256_sub_epi32(off, d);

    __m128i rid = _mm_loadl_epi64((__m128i *)&idx[good]);
    tmp = _mm256_cvtepu8_epi32(rid);
    d = _mm256_permutevar8x32_epi32(d, tmp);
    _mm256_storeu_si256((__m256i *)&r[ctr], d);
    ctr += __builtin_popcount(good);
  }

  while(ctr < len && pos + 5 <= buflen) {
    vec[0]  = buf[pos + 0];
    vec[0] |= (uint32_t)buf[pos + 1] << 8;
    vec[0] |= (uint32_t)buf[pos + 2] << 16;
    vec[0] &= 0xFFFFF;

    vec[1]  = buf[pos + 2] >> 4;
    vec[1] |= (uint32_t)buf[pos + 3] << 4;
    vec[1] |= (uint32_t)buf[pos + 4] << 12;

    pos += 5;

    if(vec[0] <= 2*GAMMA1 - 2)
      r[ctr++] = Q + GAMMA1 - 1 - vec[0];
    if(vec[1] <= 2*GAMMA1 - 2 && ctr < len)
      r[ctr++] = Q + GAMMA1 - 1 - vec[1];
  }

  DBENCH_STOP(*tsample);
  return ctr;
}
