/********************************************************************************************
* FrodoKEM: Learning with Errors Key Encapsulation
*
* Abstract: matrix arithmetic functions used by the KEM
*********************************************************************************************/

#include "aes.h"
#include <immintrin.h>

int frodo_mul_add_as_plus_e(uint16_t *out, const uint16_t *s, const uint16_t *e, const uint8_t *seed_A) 
{ // Generate-and-multiply: generate matrix A (N x N) row-wise, multiply by s on the right.
  // Inputs: s, e (N x N_BAR)
  // Output: out = A*s + e (N x N_BAR)
    int i, j, k;
    ALIGN_HEADER(32) int16_t a_row[4*PARAMS_N] ALIGN_FOOTER(32) = {0};

    for (i = 0; i < (PARAMS_N*PARAMS_NBAR); i += 2) {    
        *((uint32_t*)&out[i]) = *((uint32_t*)&e[i]);
    }    
    
    int16_t a_row_temp[4*PARAMS_N] = {0};                       // Take four lines of A at once       
    EVP_CIPHER_CTX *aes_key_schedule;    
    int len;
    if (!(aes_key_schedule = EVP_CIPHER_CTX_new())) handleErrors();    
    if (1 != EVP_EncryptInit_ex(aes_key_schedule, EVP_aes_128_ecb(), NULL, seed_A, NULL)) handleErrors();    
                                     
    for (j = 0; j < PARAMS_N; j += PARAMS_STRIPE_STEP) {
        a_row_temp[j + 1 + 0*PARAMS_N] = j;                     // Loading values in the little-endian order
        a_row_temp[j + 1 + 1*PARAMS_N] = j;
        a_row_temp[j + 1 + 2*PARAMS_N] = j;
        a_row_temp[j + 1 + 3*PARAMS_N] = j;
    }

    for (i = 0; i < PARAMS_N; i += 4) {
        for (j = 0; j < PARAMS_N; j += PARAMS_STRIPE_STEP) {    // Go through A, four rows at a time
            a_row_temp[j + 0*PARAMS_N] = i+0;                   // Loading values in the little-endian order                                
            a_row_temp[j + 1*PARAMS_N] = i+1;
            a_row_temp[j + 2*PARAMS_N] = i+2;
            a_row_temp[j + 3*PARAMS_N] = i+3;
        }

        if (1 != EVP_EncryptUpdate(aes_key_schedule, (uint8_t*)a_row, &len, (uint8_t*)a_row_temp, 4*PARAMS_N*sizeof(int16_t))) handleErrors();

        for (k = 0; k < PARAMS_NBAR; k++) {
            uint16_t sum[4] = {0};
            for (j = 0; j < PARAMS_N; j++) {                    // Matrix-vector multiplication            
                uint16_t sp = s[k*PARAMS_N + j];
                sum[0] += a_row[0*PARAMS_N + j] * sp;           // Go through four lines with same s
                sum[1] += a_row[1*PARAMS_N + j] * sp;
                sum[2] += a_row[2*PARAMS_N + j] * sp;
                sum[3] += a_row[3*PARAMS_N + j] * sp;
            }
            out[(i+0)*PARAMS_NBAR + k] += sum[0];
            out[(i+2)*PARAMS_NBAR + k] += sum[2];
            out[(i+1)*PARAMS_NBAR + k] += sum[1];
            out[(i+3)*PARAMS_NBAR + k] += sum[3];
        }
    }
    
    AES128_free_schedule(aes_key_schedule);
    return 1;
}


int frodo_mul_add_sa_plus_e(uint16_t *out, const uint16_t *s, const uint16_t *e, const uint8_t *seed_A)
{ // Generate-and-multiply: generate matrix A (N x N) column-wise, multiply by s' on the left.
  // Inputs: s', e' (N_BAR x N)
  // Output: out = s'*A + e' (N_BAR x N)
    int i, j, kk;

    for (i = 0; i < (PARAMS_N*PARAMS_NBAR); i += 2) {
        *((uint32_t*)&out[i]) = *((uint32_t*)&e[i]);
    }

    int k;
    uint16_t a_cols[PARAMS_N*PARAMS_STRIPE_STEP] = {0};
    ALIGN_HEADER(32) uint16_t a_cols_t[PARAMS_N*PARAMS_STRIPE_STEP] ALIGN_FOOTER(32) = {0};
    uint16_t a_cols_temp[PARAMS_N*PARAMS_STRIPE_STEP] = {0};       
    EVP_CIPHER_CTX *aes_key_schedule;    
    int len;
    if (!(aes_key_schedule = EVP_CIPHER_CTX_new())) handleErrors();    
    if (1 != EVP_EncryptInit_ex(aes_key_schedule, EVP_aes_128_ecb(), NULL, seed_A, NULL)) handleErrors();    

    for (i = 0, j = 0; i < PARAMS_N; i++, j += PARAMS_STRIPE_STEP) {
        a_cols_temp[j] = i;                                     // Loading values in the little-endian order
    }

    for (kk = 0; kk < PARAMS_N; kk += PARAMS_STRIPE_STEP) {     // Go through A's columns, 8 (== PARAMS_STRIPE_STEP) columns at a time.       
        for (i = 0; i < (PARAMS_N*PARAMS_STRIPE_STEP); i += PARAMS_STRIPE_STEP) {
            a_cols_temp[i + 1] = kk;                            // Loading values in the little-endian order
        }
        
        if (1 != EVP_EncryptUpdate(aes_key_schedule, (uint8_t*)a_cols, &len, (uint8_t*)a_cols_temp, PARAMS_N*PARAMS_STRIPE_STEP*sizeof(int16_t))) handleErrors();

        for (i = 0; i < PARAMS_N; i++) {                        // Transpose a_cols to have access to it in the column-major order.
            for (k = 0; k < PARAMS_STRIPE_STEP; k++) {
                a_cols_t[k*PARAMS_N + i] = a_cols[i*PARAMS_STRIPE_STEP + k];
            }
        } 

        for (i = 0; i < PARAMS_NBAR; i++) {
            for (k = 0; k < PARAMS_STRIPE_STEP; k += PARAMS_PARALLEL) {
                ALIGN_HEADER(32) uint32_t sum[8 * PARAMS_PARALLEL] ALIGN_FOOTER(32);
                __m256i a[PARAMS_PARALLEL], b, acc[PARAMS_PARALLEL];
                acc[0] = _mm256_setzero_si256();
                acc[1] = _mm256_setzero_si256();
                acc[2] = _mm256_setzero_si256();
                acc[3] = _mm256_setzero_si256();
                for (j = 0; j < PARAMS_N; j += 16) {            // Matrix-vector multiplication
                    b = _mm256_load_si256((__m256i*)&s[i*PARAMS_N + j]);
                    a[0] = _mm256_load_si256((__m256i*)&a_cols_t[(k+0)*PARAMS_N + j]);
                    a[0] = _mm256_madd_epi16(a[0], b);
                    acc[0] = _mm256_add_epi16(a[0], acc[0]);
                    a[1] = _mm256_load_si256((__m256i*)&a_cols_t[(k+1)*PARAMS_N + j]);
                    a[1] = _mm256_madd_epi16(a[1], b);
                    acc[1] = _mm256_add_epi16(a[1], acc[1]);
                    a[2] = _mm256_load_si256((__m256i*)&a_cols_t[(k+2)*PARAMS_N + j]);
                    a[2] = _mm256_madd_epi16(a[2], b);
                    acc[2] = _mm256_add_epi16(a[2], acc[2]);
                    a[3] = _mm256_load_si256((__m256i*)&a_cols_t[(k+3)*PARAMS_N + j]);
                    a[3] = _mm256_madd_epi16(a[3], b);
                    acc[3] = _mm256_add_epi16(a[3], acc[3]);
                }
                _mm256_store_si256((__m256i*)(sum + (8*0)), acc[0]);
                out[i*PARAMS_N + kk + k + 0] += sum[8*0 + 0] + sum[8*0 + 1] + sum[8*0 + 2] + sum[8*0 + 3] + sum[8*0 + 4] + sum[8*0 + 5] + sum[8*0 + 6] + sum[8*0 + 7];
                _mm256_store_si256((__m256i*)(sum + (8*1)), acc[1]);
                out[i*PARAMS_N + kk + k + 1] += sum[8*1 + 0] + sum[8*1 + 1] + sum[8*1 + 2] + sum[8*1 + 3] + sum[8*1 + 4] + sum[8*1 + 5] + sum[8*1 + 6] + sum[8*1 + 7];
                _mm256_store_si256((__m256i*)(sum + (8*2)), acc[2]);
                out[i*PARAMS_N + kk + k + 2] += sum[8*2 + 0] + sum[8*2 + 1] + sum[8*2 + 2] + sum[8*2 + 3] + sum[8*2 + 4] + sum[8*2 + 5] + sum[8*2 + 6] + sum[8*2 + 7];
                _mm256_store_si256((__m256i*)(sum + (8*3)), acc[3]);
                out[i*PARAMS_N + kk + k + 3] += sum[8*3 + 0] + sum[8*3 + 1] + sum[8*3 + 2] + sum[8*3 + 3] + sum[8*3 + 4] + sum[8*3 + 5] + sum[8*3 + 6] + sum[8*3 + 7];
            }
        }
    }
    AES128_free_schedule(aes_key_schedule);

    return 1;
}


void frodo_mul_bs(uint16_t *out, const uint16_t *b, const uint16_t *s) 
{ // Multiply by s on the right
  // Inputs: b (N_BAR x N), s (N x N_BAR)
  // Output: out = b*s (N_BAR x N_BAR)
    int i, j, k;

    for (i = 0; i < PARAMS_NBAR; i++) {
        for (j = 0; j < PARAMS_NBAR; j++) {
            out[i*PARAMS_NBAR + j] = 0;
            for (k = 0; k < PARAMS_N; k++) {
                out[i*PARAMS_NBAR + j] += b[i*PARAMS_N + k] * s[j*PARAMS_N + k];
            }
            out[i*PARAMS_NBAR + j] = (uint32_t)(out[i*PARAMS_NBAR + j]) & ((1<<PARAMS_LOGQ)-1);
        }
    }
}


void frodo_mul_add_sb_plus_e(uint16_t *out, const uint16_t *b, const uint16_t *s, const uint16_t *e) 
{ // Multiply by s on the left
  // Inputs: b (N x N_BAR), s (N_BAR x N), e (N_BAR x N_BAR)
  // Output: out = s*b + e (N_BAR x N_BAR)
    int i, j, k;

    for (k = 0; k < PARAMS_NBAR; k++) {
        for (i = 0; i < PARAMS_NBAR; i++) {
            out[k*PARAMS_NBAR + i] = e[k*PARAMS_NBAR + i];
            for (j = 0; j < PARAMS_N; j++) {
                out[k*PARAMS_NBAR + i] += s[k*PARAMS_N + j] * b[j*PARAMS_NBAR + i];
            }
            out[k*PARAMS_NBAR + i] = (uint32_t)(out[k*PARAMS_NBAR + i]) & ((1<<PARAMS_LOGQ)-1);
        }
    }
}


void frodo_add(uint16_t *out, const uint16_t *a, const uint16_t *b) 
{ // Add a and b
  // Inputs: a, b (N_BAR x N_BAR)
  // Output: c = a + b

    for (int i = 0; i < (PARAMS_NBAR*PARAMS_NBAR); i++) {
        out[i] = (a[i] + b[i]) & ((1<<PARAMS_LOGQ)-1);
    }
}


void frodo_sub(uint16_t *out, const uint16_t *a, const uint16_t *b) 
{ // Subtract a and b
  // Inputs: a, b (N_BAR x N_BAR)
  // Output: c = a - b

    for (int i = 0; i < (PARAMS_NBAR*PARAMS_NBAR); i++) {
        out[i] = (a[i] - b[i]) & ((1<<PARAMS_LOGQ)-1);
    }
}


void frodo_key_encode(uint16_t *out, const uint16_t *in) 
{ // Encoding
    unsigned int i, j, npieces_word = 8;
    unsigned int nwords = (PARAMS_NBAR*PARAMS_NBAR)/8;
    uint64_t temp, mask = ((uint64_t)1 << PARAMS_EXTRACTED_BITS) - 1;
    uint16_t* pos = out;

    for (i = 0; i < nwords; i++) {
        temp = 0;
        for(j = 0; j < PARAMS_EXTRACTED_BITS; j++) 
            temp |= ((uint64_t)((uint8_t*)in)[i*PARAMS_EXTRACTED_BITS + j]) << (8*j);
        for (j = 0; j < npieces_word; j++) { 
            *pos = (uint16_t)((temp & mask) << (PARAMS_LOGQ - PARAMS_EXTRACTED_BITS));  
            temp >>= PARAMS_EXTRACTED_BITS;
            pos++;
        }
    }
}


void frodo_key_decode(uint16_t *out, const uint16_t *in)
{ // Decoding
    unsigned int i, j, index = 0, npieces_word = 8;
    unsigned int nwords = (PARAMS_NBAR * PARAMS_NBAR) / 8;
    uint16_t temp, maskex=((uint16_t)1 << PARAMS_EXTRACTED_BITS) -1, maskq =((uint16_t)1 << PARAMS_LOGQ) -1;
    uint8_t  *pos = (uint8_t*)out;
    uint64_t templong;

    for (i = 0; i < nwords; i++) {
        templong = 0;
        for (j = 0; j < npieces_word; j++) {  // temp = floor(in*2^{-11}+0.5)
            temp = ((in[index] & maskq) + (1 << (PARAMS_LOGQ - PARAMS_EXTRACTED_BITS - 1))) >> (PARAMS_LOGQ - PARAMS_EXTRACTED_BITS);
            templong |= ((uint64_t)(temp & maskex)) << (PARAMS_EXTRACTED_BITS * j);
            index++;
        }
	for(j = 0; j < PARAMS_EXTRACTED_BITS; j++) 
	    pos[i*PARAMS_EXTRACTED_BITS + j] = (templong >> (8*j)) & 0xFF;
    }
}
