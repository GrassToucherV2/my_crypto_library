#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "des.h"
#include "../util/tools.h"

static const uint8_t S_BOX[8][4][16] = {
    {   // S1
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    },
    {   // S2
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
    },
    {   // S3
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
    },
    {   // S4
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
    },
    {   // S5
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
    },
    {   // S6
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
    },
    {   // S7
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
    },
    {   // S8
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
    }
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

static const uint8_t PC2[48] = {
    14, 17, 11, 24,  1,  5,
     3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8,
    16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

static const uint8_t E[48] = {
    32,  1,  2,  3,  4,  5,
     4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32,  1
};

static const uint8_t P[32] = {
    16,  7, 20, 21,
    29, 12, 28, 17,
     1, 15, 23, 26,
     5, 18, 31, 10,
     2,  8, 24, 14,
    32, 27,  3,  9,
    19, 13, 30,  6,
    22, 11,  4, 25
};



// this permutation is specified in the standard prior to encryption/decryption, there is no security related purposes
static void permute(uint64_t input, uint64_t *output, const uint8_t *table, 
                    unsigned int table_size, unsigned int num_bits_input){
    *output = 0;

    for(unsigned int i = 0; i < table_size; i++){
        // getting the bit to permute from the input and the position for that bit in output
        // for example, the first entry in PC1, 57 means the 57th bit will be the first bit after 
        // permutation. Since the first bit is the msb of the input, to get 57th bit, we shift left
        // by 64 - 57 and AND with 1 to extract the bit
        uint8_t bit_to_permute = (input >> (num_bits_input - table[i])) & 1;
        uint8_t new_pos = table_size - 1 - i;
        
        // bitwise OR the bits to their position as specified by one of the tables
        *output |= (uint64_t)bit_to_permute << new_pos;
    }
}

static void key_schedule(uint32_t left, uint32_t right, uint32_t *left_keys, uint32_t *right_keys){
    int shift_amounts[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};
    uint32_t mask = 0x0FFFFFFF;
    for (int i = 0; i < 16; i++) {
        // Circular left shift
        left = ((left << shift_amounts[i]) | (left >> (28 - shift_amounts[i]))) & mask;
        right = ((right << shift_amounts[i]) | (right >> (28 - shift_amounts[i]))) & mask;

        left_keys[i] = left;
        right_keys[i] = right;
    }
}

static void key_expansion(uint64_t key, uint64_t *keys){
    // dropping parity bits (lsb of each byte) - key permutation
    uint64_t key_56 = 0;

    permute(key, &key_56, PC1, DES_KEY_SIZE_BITS, 64);
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
        permute(keys_56[i], &keys[i], PC2, DES_SUBKEY_SIZE_BITS, 56);
    }
    
}

// static void print_subkeys(des_ctx *ctx){
//     for(int i = 0; i < 16; i++){
//         printf("%lX ", ctx->subkeys[i]);
//         // print_as_bits_BE((uint8_t *)&ctx->subkeys[i], 8, "subkey");
//     }
//     printf("\n");
// }

crypt_status DES_init(des_ctx *ctx, uint64_t key){
    if(!ctx) return CRYPT_NULL_PTR;

    memset(ctx->subkeys, 0, sizeof(ctx->subkeys));

    key_expansion(key, ctx->subkeys);

    return CRYPT_OKAY;
}

static void s_box_lookup(uint8_t block, uint8_t *block_4 , const uint8_t S[4][16]){
    uint8_t row = 0; 
    row |= (block & 0x20) >> 4;     // 0b00100000 = 0x20
    row |= block & 1;

    uint8_t col = (block & 0x1E) >> 1;  // 0b00011110 = 0x1E
    *block_4 = S[row][col];
}

// Feistel function, the internal PRF applied to right half of the block during each round
static void feistel(uint32_t right, uint32_t *res, uint64_t key){
    *res = 0;
    // expand the 32 bit right half of the block
    uint64_t right_expanded = 0;
    permute(right, &right_expanded, E, DES_RIGHT_EXPANDED_SIZE_BITS, 32);

    // XOR the expanded block with the subkey
    right_expanded ^= key;
    // break the expanded block into 8 6-bit blocks and perform S-box lookup
    uint32_t res_before_perm = 0;
    for (int i = 0; i < 8; i++) {
        // shifting 42 bits to get the most significant 6 bits
        uint8_t six_bit_block = (right_expanded >> (42 - 6 * i)) & 0x3F;
        s_box_lookup(six_bit_block, &six_bit_block, S_BOX[i]);
        
        // Assemble the result from the S-box outputs, same as above, shifting 28 bits to get the 4 msb
        res_before_perm |= ((uint32_t)six_bit_block << (28 - 4 * i));
    }
    permute(res_before_perm, (uint64_t *)res, P, sizeof(P)/sizeof(P[0]), 32);
}

// one round of transformation in DES Feistel network
static void DES_transform(uint32_t right_in, uint32_t left_in, uint64_t key,
                        uint32_t *right_out, uint32_t *left_out)
{
    // L_(i+1) = R_i
    *left_out = right_in;
    // R_(i+1) = L_i ^ f(k, R_i)
    feistel(right_in, right_out, key);
    *right_out ^= left_in;
}

static void DES_encrypt_block(des_ctx *ctx, uint64_t block_in, uint32_t *left_half, 
                            uint32_t *right_half, uint64_t *ciphertext_block)
{   
    uint64_t block_out = 0;
    uint64_t temp = LE64TOBE64(block_in);
    permute(temp, &block_out, IP, sizeof(IP)/sizeof(IP[0]), 64);
    left_half[0] = (block_out & 0xFFFFFFFF00000000) >> 32;
    right_half[0] = block_out & 0xFFFFFFFF;
    for(int i = 0; i < DES_NUM_ROUNDS; i++){
        DES_transform(right_half[i], left_half[i], ctx->subkeys[i], &right_half[i+1], &left_half[i+1]);

    }
    *ciphertext_block |= (((uint64_t)right_half[16]) << 32);
    *ciphertext_block |= (uint64_t)(left_half[16]);
    permute(*ciphertext_block, ciphertext_block, IP_1, sizeof(IP_1)/sizeof(IP_1[0]), 64);

    reverse_byte_order((uint8_t *)ciphertext_block, (uint8_t *)ciphertext_block, DES_BLOCK_SIZE_BYTES);
}

// By default a block cipher operates in ECB mode
crypt_status DES_encrypt_ECB(des_ctx *ctx, const unsigned char *plaintext, unsigned int plaintext_len,
                        unsigned char *ciphertext, unsigned int ciphertext_len, DES_padding padding)
{
    if(!ctx || !plaintext || !ciphertext) return CRYPT_NULL_PTR;

    if(ciphertext_len < plaintext_len){
        return CRYPT_BAD_BUFFER_LEN;
    }
    memset(ciphertext, 0, ciphertext_len);

    int num_blocks = plaintext_len / DES_BLOCK_SIZE_BYTES;

    uint64_t *blocks = (uint64_t *)plaintext;
    uint64_t *ciphertext_blocks = (uint64_t *)ciphertext;
    uint32_t right_half[17] = {0};
    uint32_t left_half[17] = {0};

    // encrypt the full blocks
    for(int i = 0; i < num_blocks; i++){
        DES_encrypt_block(ctx, blocks[i], left_half, right_half, &ciphertext_blocks[i]);
    }

    return CRYPT_OKAY;
}

static void DES_decrypt_block(des_ctx *ctx, uint64_t block_in, uint32_t *left_half, 
                            uint32_t *right_half, uint64_t *plaintext_block)
{   
    uint64_t block_out = 0;
    uint64_t temp = LE64TOBE64(block_in);
    permute(temp, &block_out, IP_1, sizeof(IP_1)/sizeof(IP_1[0]), 64);
    left_half[0] = (block_out & 0xFFFFFFFF00000000) >> 32;
    right_half[0] = block_out & 0xFFFFFFFF;
    for(int i = 0; i < DES_NUM_ROUNDS; i++){
        DES_transform(left_half[i], right_half[i], ctx->subkeys[15 - i], &left_half[i+1], &right_half[i+1]);

    }
    *plaintext_block |= (((uint64_t)right_half[16]) << 32);
    *plaintext_block |= (uint64_t)(left_half[16]);
    permute(*plaintext_block, plaintext_block, IP, sizeof(IP)/sizeof(IP[0]), 64);
    reverse_byte_order((uint8_t *)plaintext_block, (uint8_t *)plaintext_block, DES_BLOCK_SIZE_BYTES);
}

crypt_status DES_decrypt_ECB(des_ctx *ctx, const unsigned char *ciphertext, unsigned int ciphertext_len,
                        unsigned char *plaintext, unsigned int plaintext_len, DES_padding padding)
{
    if(!ctx || !plaintext || !ciphertext) return CRYPT_NULL_PTR;

    if(plaintext_len < ciphertext_len){
        return CRYPT_BAD_BUFFER_LEN;
    }
    memset(plaintext, 0, plaintext_len);

    int num_blocks = ciphertext_len / DES_BLOCK_SIZE_BYTES;

    uint64_t *blocks = (uint64_t *)ciphertext;
    uint64_t *ciphertext_blocks = (uint64_t *)plaintext;
    uint32_t right_half[17] = {0};
    uint32_t left_half[17] = {0};

    // encrypt the full blocks
    for(int i = 0; i < num_blocks; i++){
        DES_decrypt_block(ctx, blocks[i], left_half, right_half, &ciphertext_blocks[i]);
    }

    return CRYPT_OKAY;
}

crypt_status DES_encrypt_CBC(des_ctx *ctx, const unsigned char *plaintext, unsigned int plaintext_len,
                            uint64_t iv, unsigned char *ciphertext, unsigned int ciphertext_len)
{
    if(!ctx || !plaintext || !iv || !ciphertext) return CRYPT_NULL_PTR;

    if(ciphertext_len < plaintext_len){
        return CRYPT_BAD_BUFFER_LEN;
    }
    memset(ciphertext, 0, ciphertext_len);

    int partial_block_len = plaintext_len % DES_BLOCK_SIZE_BYTES;

    // padding the plaintext
    uint8_t len_to_pad = DES_BLOCK_SIZE_BYTES - partial_block_len;
    unsigned char *plaintext_padded = (unsigned char*)malloc(plaintext_len + len_to_pad);
    memcpy(plaintext_padded, plaintext, plaintext_len);
    memset(&plaintext_padded[plaintext_len], len_to_pad, len_to_pad);

    uint64_t *blocks = (uint64_t *)plaintext_padded;
    uint64_t *ciphertext_blocks = (uint64_t *)ciphertext;
    uint32_t right_half[17] = {0};
    uint32_t left_half[17] = {0};

    int num_blocks = plaintext_len / DES_BLOCK_SIZE_BYTES + 1;
    uint64_t xor_block = iv ^ blocks[0];
    for(int i = 0; i < num_blocks; i++){
        DES_encrypt_block(ctx, xor_block, left_half, right_half, &ciphertext_blocks[i]);
        xor_block = blocks[i+1] ^ ciphertext_blocks[i];
    }

    free(plaintext_padded);
    return CRYPT_OKAY;

}

crypt_status DES_cleanup(des_ctx *ctx){
    if(!ctx) return CRYPT_NULL_PTR;

    memset(ctx->subkeys, 0, sizeof(ctx->subkeys));
    return CRYPT_OKAY;
}
