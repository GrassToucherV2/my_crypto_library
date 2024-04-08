#include <stdint.h>
#include <string.h>
#include "des.h"

static const uint8_t S1[64] = {
    14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
    0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
    4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
    15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13
};

static const uint8_t S2[64] = {
    15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
    3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
    0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
    13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9
};

static const uint8_t S3[64] = {
    10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
    13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
    13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
    1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12
};

static const uint8_t S4[64] = {
    7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
    13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
    10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
    3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14
};

static const uint8_t S5[64] = {
    2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
    14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
    4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
    11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3
};

static const uint8_t S6[64] = {
    12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
    10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
    9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
    4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13
};

static const uint8_t S7[64] = {
    4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
    13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
    1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
    6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12
};

static const uint8_t S8[64] = {
    13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
    1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
    7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
    2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
};

// Initial Permutation (IP) table
static const uint8_t IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9,  1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

// Final Permutation (Inverse of Initial Permutation, IP-1) table
static const uint8_t IP_1[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9,  49, 17, 57, 25
};

// this table is used for permutation of the key 
static const uint8_t PC1[56] = {
    57, 49, 41, 33, 25, 17,  9,  1, 
    58, 50, 42, 34, 26, 18, 10,  2, 
    59, 51, 43, 35, 27, 19, 11,  3, 
    60, 52, 44, 36, 63, 55, 47, 39, 
    31, 23, 15,  7, 62, 54, 46, 38, 
    30, 22, 14,  6, 61, 53, 45, 37, 
    29, 21, 13,  5, 28, 20, 12,  4
};

const uint8_t PC2[48] = {
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};


// this permutation is specified in the standard prior to encryption/decryption, there is no security related purposes
static void permute(uint64_t input, uint64_t *output, const uint8_t *table, unsigned int size){
    *output = 0;

    for(int i = 0; i < size; i++){
        // getting the bit to permute from the input and the position for that bit in output
        // for example, the first entry in PC1, 57 means the 57th bit will be the first bit after 
        // permutation. Since the first bit is the msb of the input, to get 57th bit, we shift left
        // by 64 - 57 and AND with 1 to extract the bit
        uint8_t bit_to_permute = (input >> (64 - table[i])) & 1;
        uint8_t new_pos = size - 1 - i;
        
        // bitwise OR the bits to their position as specified by one of the tables
        *output |= (uint64_t)bit_to_permute << new_pos;
    }
}

static void key_schedule(uint32_t left, uint32_t right, uint32_t *left_keys, uint32_t *right_keys){
    int shift_amounts[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

    for (int i = 0; i < 16; i++) {
        // Circular left shift
        left = (left << shift_amounts[i]) | (left >> (28 - shift_amounts[i]));
        right = (right << shift_amounts[i]) | (right >> (28 - shift_amounts[i]));

        left_keys[i] = left;
        right_keys[i] = right;
    }
}

static void key_expansion(uint64_t key, uint64_t *keys){
    // dropping parity bits (lsb of each byte) - key permutation
    uint64_t key_56 = 0;
    permute(key, &key_56, PC1, DES_KEY_SIZE_BITS);

    // separate the 56-bit key into left half and right half
    uint32_t right = key_56 & 0x000000000FFFFFFF;
    uint32_t left = (key_56 >> 28) & 0x000000000FFFFFFF;

    // generate 16 subkeys
    uint32_t left_keys[16] = {0};
    uint32_t right_keys[16] = {0};
    key_schedule(left, right, left_keys, right_keys);

    uint64_t keys_56[16] = {0};
    for(int i = 0; i < DES_NUM_ROUNDS; i++){
        keys_56[i] |= (uint64_t)right_keys[i];
        keys_56[i] |= ((uint64_t)left_keys[i] << 28);
    }

    permute(keys_56, keys, PC2, DES_SUBKEY_SIZE_BITS);
}

crypt_status des_init(des_ctx *ctx, uint64_t key){
    if(!ctx) return CRYPT_NULL_PTR;

    memset(ctx->subkeys, 0, sizeof(ctx->subkeys));

    key_expansion(key, ctx->subkeys);

    return CRYPT_OKAY;
}



void des_key_test(uint64_t key, uint64_t *key_56){
    key_expansion(key, key_56);
}
