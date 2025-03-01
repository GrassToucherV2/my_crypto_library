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
static void xor_arrays(uint8_t *x, uint8_t *y, uint8_t *z){
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

// this function performs the multiplication of two blocks in GF(2^128)
void GF_mul_blocks(uint8_t *X, uint8_t *Y, uint8_t *res) {
    uint64_t Z_high = 0, Z_low = 0;  // 128-bit accumulator (Z)
    uint64_t V_high, V_low;          // 128-bit register (V)
    uint64_t Y_high, Y_low;

    // Load X and Y into two 64-bit words (big-endian format)
    memcpy(&V_high, X, 8);
    memcpy(&V_low, X + 8, 8);
    memcpy(&Y_high, Y, 8);
    memcpy(&Y_low, Y + 8, 8);

    // Convert to host endianness (assume input is big-endian)
    V_high = LE64TOBE64(V_high);
    V_low = LE64TOBE64(V_low);
    Y_high = LE64TOBE64(Y_high);
    Y_low = LE64TOBE64(Y_low);

    bigint bi_X;
    bigint_init(&bi_X, GCM_BLOCK_SIZE_BYTES / sizeof(digit));
    bigint_from_bytes(&bi_X, X, GCM_BLOCK_SIZE_BYTES);

    for (int i = 127; i >= 0; i--) {
        // TODO: make it constant-time
        if(bigint_is_bit_set(&bi_X, i)){
            Z_high ^= V_high;
            Z_low ^= V_low;
        }
        
        // mask used to determine whether we need to reduce (V >> 1) or not
        // if V's LSB is set, then we reduce it
        uint64_t msb_mask = -(V_low & 1);

        // right shift V by 1, LSB of V_high becomes the MSB of V_low after shifting
        V_low = (V_low >> 1) | (V_high << 63);
        V_high >>= 1;

        // It's not necessary to use the entire 16-byte R array due to all lower bytes being 0
        // and XORing 0 does pretty much nothing
        V_high ^= (msb_mask & GCM_R);
    }

    bigint_free(&bi_X);

    // Convert back to big-endian - only LE conversions would work on little-endidan systems, kinda goofy
    // TODO: change endian swapping macro to be independent of system endianness, it simply needs to swap
    // byte order
    Z_high = LE64TOBE64(Z_high);
    Z_low = LE64TOBE64(Z_low);

    memcpy(res, &Z_high, 8);
    memcpy(res + 8, &Z_low, 8);
}

// this function assumes the input text (text_in) has been padded and processed
// such that the length is a multiple of 16 in bytes
// validate that output_len is 16 bytes
void GHASH(uint8_t *H, uint8_t *text_in,
            uint32_t text_in_len, uint8_t *output, uint32_t output_len)
{
    int loop_counter = text_in_len >> 4; // >> 7 = / 16
    memset(output, 0, output_len);
    uint8_t X[16] = {0};

    for(int i = 0; i < loop_counter; i++){
        memcpy(X, &text_in[i * GCM_BLOCK_SIZE_BYTES], GCM_BLOCK_SIZE_BYTES);
        xor_arrays(X, output, output);
        GF_mul_blocks(output, H, output);
    }

}
