#include "gcm.h"
#include "bigint.h"
#include "common.h"
#include "../util/tools.h"

#include <stdlib.h>

// GCM standard: https://nvlpubs.nist.gov/nistpubs/legacy/sp/nistspecialpublication800-38d.pdf

#define GCM_R 0xE100000000000000ULL  // Irreducible polynomial in GF(2^128) defined in the standard
uint8_t R[16] = {
    0xE1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// this function assumes all inputs and output are 16 bytes long 
void xor_blocks(const uint8_t *x, const uint8_t *y, uint8_t *z){
    for(int i = 0; i < 16; i++){
        z[i] = x[i] ^ y[i];
    }
}

/*
    in AES-GCM, as per NIST standard for GCM, the first 96 bits is the IV, left unchanged
    thus we only increment the last 32 bits (mod 2^32)
*/
void inc32(uint8_t *X){

    // assuming little endian
    uint32_t lsb = (X[12] << 24) | (X[13] << 16) | (X[14] << 8) | X[15];

    lsb = (lsb + 1) & 0xFFFFFFFF;   // 0xFFFFFFFF = 2^32 - 1

    // back to big endian order
    X[12] = (lsb >> 24) & 0xFF;
    X[13] = (lsb >> 16) & 0xFF;
    X[14] = (lsb >> 8) & 0xFF;
    X[15] = lsb & 0xFF;
}

/*
 * Multiply X * Y in GF(2^128), producing 'res'.
 * All pointers are 16-byte blocks in big-endian form.
 */
void GF_mul_blocks(const uint8_t *X, const uint8_t *Y, uint8_t *res) {
    uint64_t Z_high = 0, Z_low = 0;
    uint64_t V_high, V_low;
    uint64_t X_high, X_low;

    memcpy(&V_high, Y, 8);
    memcpy(&V_low, Y + 8, 8);
    memcpy(&X_high, X, 8);
    memcpy(&X_low, X + 8, 8);

    V_high = LE64TOBE64(V_high);
    V_low = LE64TOBE64(V_low);
    X_high = LE64TOBE64(X_high);
    X_low = LE64TOBE64(X_low);

    for (int i = 0; i < 128; i++) {
        uint64_t is_high_half = (i <= 63);
        uint64_t mask = (is_high_half ? ((uint64_t)1 << (63 - i))
                                      : ((uint64_t)1 << (127 - i)));
        if ((is_high_half ? X_high : X_low) & mask) {
            Z_high ^= V_high;
            Z_low ^= V_low;
        }

        uint64_t lsb_mask = 0ULL - (V_low & 1ULL);
        V_low = (V_low >> 1) | ((V_high & 1ULL) << 63);
        V_high >>= 1;
        V_high ^= (lsb_mask & GCM_R);
    }

    Z_high = LE64TOBE64(Z_high);
    Z_low = LE64TOBE64(Z_low);

    memcpy(res, &Z_high, 8);
    memcpy(res + 8, &Z_low, 8);
}

// this function assumes the input text (text_in) has been padded and processed
// such that the length is a multiple of 16 in bytes
// validate that output_len is 16 bytes
void GHASH(const uint8_t *H, 
           const uint8_t *aad, uint32_t aad_len,
           const uint8_t *ciphertext, uint32_t ciphertext_len,
           uint8_t *output)
{
    // The GHASH state block, initialized to 0
    uint8_t Y[16] = {0};
    uint8_t partial_block[16];   // Buffer for handling partial blocks
    uint32_t i;

    // hash aad
    if (aad != NULL && aad_len > 0) {
        i = 0;
        while (i < aad_len) {
            uint32_t copy_len = (aad_len - i >= 16) ? 16 : (aad_len - i);
            
            // zero pad the block first
            memset(partial_block, 0, 16); 
            memcpy(partial_block, aad + i, copy_len);
            
            xor_blocks(partial_block, Y, Y);
            GF_mul_blocks(Y, H, Y);
            
            i += copy_len;
        }
    }

    // hash the Ciphertext
    if (ciphertext != NULL && ciphertext_len > 0) {
        i = 0;
        while (i < ciphertext_len) {
            uint32_t copy_len = (ciphertext_len - i >= 16) ? 16 : (ciphertext_len - i);
            
            // zero-pad the block first, then copy up to 16 bytes
            memset(partial_block, 0, 16); 
            memcpy(partial_block, ciphertext + i, copy_len);
            
            xor_blocks(partial_block, Y, Y);
            GF_mul_blocks(Y, H, Y);
            
            i += copy_len;
        }
    }

    // hash the length block
    // "64-bit aad length in bits" || "64-bit ciphertext length in bits"
    uint8_t len_block[16] = {0};
    uint64_t aad_bits = (uint64_t)aad_len * 8;
    uint64_t ciphertext_bits = (uint64_t)ciphertext_len * 8;

    for (int j = 0; j < 8; j++) {
        len_block[j]     = (uint8_t)(aad_bits >> (56 - 8 * j));
        len_block[8 + j] = (uint8_t)(ciphertext_bits >> (56 - 8 * j));
    }

    xor_blocks(len_block, Y, Y);
    GF_mul_blocks(Y, H, Y);

    memcpy(output, Y, 16);
}