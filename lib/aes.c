#include "aes.h"
#include "gcm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
    AES algorithm implemented in this file uses PKCS#7 padding scheme
*/

static const uint8_t sbox[16][16] = {
    {0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76},
    {0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0},
    {0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15},
    {0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75},
    {0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84},
    {0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf},
    {0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8},
    {0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2},
    {0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73},
    {0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb},
    {0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79},
    {0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08},
    {0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a},
    {0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e},
    {0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf},
    {0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16},
};

// inverse Sbox
static const uint8_t inv_sbox[16][16] = {
    {0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb},
    {0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb},
    {0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e},
    {0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25},
    {0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92},
    {0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84},
    {0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06},
    {0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b},
    {0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73},
    {0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e},
    {0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b},
    {0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4},
    {0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f},
    {0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef},
    {0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61},
    {0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d}
};


static const uint32_t Rcon[11] = {
    0x01, // Rcon[1]
    0x02, // Rcon[2]
    0x04, // Rcon[3]
    0x08, // Rcon[4]
    0x10, // Rcon[5]
    0x20, // Rcon[6]
    0x40, // Rcon[7]
    0x80, // Rcon[8]
    0x1b, // Rcon[9]
    0x36  // Rcon[10]
};


static void subBytes(aes_ctx *ctx){
    uint8_t row = 0;
    uint8_t col = 0;
    for(int i = 0; i < AES_BLOCK_SIZE_BYTES; i++) {
        row = (ctx->state[i] & 0xF0) >> 4;
        col = ctx->state[i] & 0x0F;
        ctx->state[i] = sbox[row][col];
    }
}

// row 1 remains the same 
// row 2 cyclically shifted to the left by 1 
// row 3 cyclically shifted to the left by 2
// row 4 cyclically shifted to the left by 3, which is basically 
// cyclically shifted to the right by 1
/*
    The state matrix after shiftRows
    0  4  8   12                    0  4  8  12
    1  5  9   13        ===>        5  9  13  1
    2  6  10  14                    10 14 2   6
    3  7  11  15                    15 3  7  11
*/
static void shiftRows(aes_ctx *ctx){
    uint8_t tmp1, tmp2;
    tmp1 = ctx->state[1];
    ctx->state[1] = ctx->state[5];
    ctx->state[5] = ctx->state[9];
    ctx->state[9] = ctx->state[13];
    ctx->state[13] = tmp1;

    tmp1 = ctx->state[2];
    tmp2 = ctx->state[6];
    ctx->state[2] = ctx->state[10];
    ctx->state[6] = ctx->state[14];
    ctx->state[10] = tmp1;
    ctx->state[14] = tmp2;

    tmp1 = ctx->state[15];
    ctx->state[15] = ctx->state[11];
    ctx->state[11] = ctx->state[7];
    ctx->state[7] = ctx->state[3];
    ctx->state[3] = tmp1;
    
}

/*
    This function compute 2 * x in GF(2^8) as defined in AES
    if overflow occurs then we reduce the result by the irreducible polynomial
    as defined in NIST FIP 197
    https://nvlpubs.nist.gov/nistpubs/fips/nist.fips.197.pdf
    the polynomial is m(x) = x^8 + x^4 + x^3 + x + 1
    Multiplication is specified in section 4.2.1
*/
static uint8_t xtime(uint8_t x){
    // check if overflow will occur
    uint8_t res = x << 1;
    // check if there is an overflow, if the msb of x is set, then shifting it will cause an overflow 
    // if overflow occured, then (res & 0x80 >> 7) = 1, so we subtract 1 and flip all the bits to set the mask to 0xFF
    // if no overflow, then (res & 0x80 >> 7) = 0, subtracting 1 makes if 0xFF, so we flip all bits to turn it into 0x00
    // this masks will allow us to conditionally XOR with either 0x1B or 0x00
    uint8_t overflow_mask = ~(((x & 0x80) >> 7) - 1);
    return res ^ (0x1B & overflow_mask);
}

/*
    As specified in 4.2.1 of the AES standard, multiplication by any constant can be achieved by adding intermediate 
    results
*/
static uint8_t xtime_GF3(uint8_t x){
    return xtime(x) ^ x;
}

/*
    After shift row operation, each column is treated as a vector and 
    transformed by multiplying a matrix with fixed value
    c1     | 2 3 1 1 |    b1 
    c2  =  | 1 2 3 1 | x  b2
    c3     | 1 1 2 3 |    b3
    c4     | 3 1 1 2 |    b4
    All operations happen in GF(2^8)
*/
static void mixColumn(aes_ctx *ctx){
    for(int col = 0; col < 4; col++) {
        // Temp variables to store the current state of the column
        uint8_t s0 = ctx->state[0 + 4*col];
        uint8_t s1 = ctx->state[1 + 4*col];
        uint8_t s2 = ctx->state[2 + 4*col];
        uint8_t s3 = ctx->state[3 + 4*col];

        // Calculating new values
        uint8_t t0 = xtime(s0) ^ xtime_GF3(s1) ^ s2 ^ s3;
        uint8_t t1 = s0 ^ xtime(s1) ^ xtime_GF3(s2) ^ s3;
        uint8_t t2 = s0 ^ s1 ^ xtime(s2) ^ xtime_GF3(s3);
        uint8_t t3 = xtime_GF3(s0) ^ s1 ^ s2 ^ xtime(s3);

        // Assigning calculated values back to the state array
        ctx->state[0 + 4*col] = t0;
        ctx->state[1 + 4*col] = t1;
        ctx->state[2 + 4*col] = t2;
        ctx->state[3 + 4*col] = t3;
    }
}

static void addRoundKey(aes_ctx *ctx, uint8_t *subkey){
    for(int i = 0; i < AES_BLOCK_SIZE_BYTES; i++){
        ctx->state[i] ^= subkey[i];
    }
}

// cyclically shift all bytes in the word to the left by 1
static uint32_t rotWord(uint32_t word){
    word = LE32TOBE32(word);
    uint8_t *word_8 = (uint8_t *)&word;
    uint8_t tmp = word_8[0];
    word_8[0] = word_8[1];
    word_8[1] = word_8[2];
    word_8[2] = word_8[3];
    word_8[3] = tmp;
    return LE32TOBE32(word);
}

// apply subByte to the 4 byte word
static uint32_t subWord(uint32_t word){
    uint8_t row = 0;
    uint8_t col = 0;
    uint8_t *word_8 = (uint8_t *)&word;
    for(int i = 0; i < 4; i++) {
        row = (word_8[i] & 0xF0) >> 4;
        col = word_8[i] & 0x0F;
        word_8[i] = sbox[row][col];
    }

    return word;
}

/*
    if the key is 128-bit, then byte 0 to 3 form column 0,bytes 4 - 7 form column 1 and so on
*/
static void key_expansion(const uint8_t *key, uint32_t *round_keys, int num_key_blocks, int num_rounds){
    
    // the key itself is break into group of four bytes as round key 0
    int i = 0;
    uint32_t * key_32 = (uint32_t *)key;
    for(; i < num_key_blocks; i++){
        round_keys[i] = LE32TOBE32(key_32[i]);
    }
    i = num_key_blocks;
    uint32_t tmp;
    int num_iteration = AES_NUM_BLOCK * (num_rounds + 1);

    for(; i < num_iteration; i++){
        tmp = round_keys[i - 1];
        if(i % num_key_blocks == 0){
            tmp = rotWord(tmp);
            tmp = subWord(tmp); 
            tmp ^= LE32TOBE32(Rcon[i / num_key_blocks - 1]);
        }
        // this step is specific to AES-256
        else if( (num_key_blocks > 6) && (i % num_key_blocks == 4) ){
            tmp = subWord(tmp);
        }
        round_keys[i] = round_keys[i - num_key_blocks] ^ tmp;
    }

    // convert all round keys into big endian
    for(int i = 0; i < 60; i++){
        round_keys[i] = LE32TOBE32(round_keys[i]);
    }
}

// static void print_round_keys(uint32_t *round_keys){
//     for(int i = 0; i < 60; i++){
//         printf("%08x ", round_keys[i]);
//         if((i+1) % 10 == 0){
//             printf("\n");
//         }
//     }
// }

// static void print_aes_state(aes_ctx *ctx){
//     for(int i = 0; i < 16; i++){
//         printf("%02x ", ctx->state[i]);
//     }
//     printf("\n");
// }

// Using reverse_byte_order to reverse the round keys array, as for decryption, the order of round keys are
// applied in reverse
// reverse_byte_order((uint8_t *)ctx->round_keys, (uint8_t *)ctx->round_keys, key_len_bytes);
static void revert_round_key_order(aes_ctx *ctx, int key_len_uint32_bytes){
    reverse_byte_order((uint8_t *)ctx->round_keys, (uint8_t *)ctx->round_keys, key_len_uint32_bytes * 4);
    uint32_t tmp[2] = {0};
    for(int j = 0; j < key_len_uint32_bytes; j += 4){
        tmp[0] = ctx->round_keys[0 + j];
        tmp[1] = ctx->round_keys[1 + j];
        ctx->round_keys[0 + j] = LE32TOBE32(ctx->round_keys[3 + j]);
        ctx->round_keys[1 + j] = LE32TOBE32(ctx->round_keys[2 + j]);
        ctx->round_keys[2 + j] = LE32TOBE32(tmp[1]);
        ctx->round_keys[3 + j] = LE32TOBE32(tmp[0]);
    }
}

crypt_status AES_init(aes_ctx *ctx, const uint8_t *key, AES_key_length key_len, int decrypt, block_cipher_mode mode){

    memset_s(ctx->state, 0, sizeof(ctx->state));
    memset_s(ctx->round_keys, 0, sizeof(ctx->round_keys));

    // I don't like these if statements in core crypto algorithms, perhaps I should have different init functins 
    // for different modes. But then, if we use a specific mode, CTR for example, the branching will never happen, or
    // if we use CBC, then the branching will always happen, so the timing variance is perhaps benign?
    // modern processors' branch prediction will introduce a small timing difference at the beginning, but since
    // the branching behavior is consistent, so I don't think this is a significant threat. 
    switch (key_len){
        case AES_128:
            ctx->aes_key_len = AES_128;
            key_expansion(key, ctx->round_keys, 4, AES_128_NUM_ROUNDS);
            if(decrypt && (mode == CBC || mode ==ECB)){
                revert_round_key_order(ctx, 44);
            }
            break;
        
        case AES_192:
            ctx->aes_key_len = AES_192;
            key_expansion(key, ctx->round_keys, 6, AES_192_NUM_ROUNDS);
            if(decrypt && (mode == CBC || mode == ECB)){
                revert_round_key_order(ctx, 52);
            }
            break;
        
        case AES_256:
            ctx->aes_key_len = AES_256;
            key_expansion(key, ctx->round_keys, 8, AES_256_NUM_ROUNDS);
            if(decrypt && (mode == CBC || mode ==ECB)){
                revert_round_key_order(ctx, 60);
            }
            break;
        default:
            return CRYPT_AES_BAD_KEY_LEN;
    }

    // printf("computed round keys\n");
    // print_round_keys(ctx->round_keys);
    return CRYPT_OKAY;
}

static void AES_encrypt_block(aes_ctx *ctx, const uint8_t *input, uint8_t *output){
    // copy the input into the state
    for(int i = 0; i < AES_BLOCK_SIZE_BYTES; i++){
        ctx->state[i] = input[i];
    }
    
    int current_round_key_index = 0;
    uint8_t *round_key_bytes = (uint8_t *)ctx->round_keys;

    addRoundKey(ctx, &round_key_bytes[current_round_key_index]);
    current_round_key_index += 16;
   
    int num_rounds;
    switch (ctx->aes_key_len){
        case AES_128:
            num_rounds = AES_128_NUM_ROUNDS;
            break;
        
        case AES_192:
            num_rounds = AES_192_NUM_ROUNDS;
            break;
        
        case AES_256:
            num_rounds = AES_256_NUM_ROUNDS;
            break;
        default:
            return;
    }

    // perform rounds of AES transformation - last round does not include mixColumn step, so it 
    // is performed outside of the for loop
    for(int round = 0; round < num_rounds - 1; round++){
        subBytes(ctx);
        shiftRows(ctx);
        mixColumn(ctx);
        addRoundKey(ctx, &round_key_bytes[current_round_key_index]);
        current_round_key_index += 16;
    }

    subBytes(ctx);
    shiftRows(ctx);
    addRoundKey(ctx, &round_key_bytes[current_round_key_index]);

    // copying the resultant state into output
    for(int i = 0; i < AES_BLOCK_SIZE_BYTES; i++){
        output[i] = ctx->state[i];
    }

}

crypt_status AES_encrypt_ECB(aes_ctx *ctx, 
                            const uint8_t *plaintext, unsigned int plaintext_len,
                            uint8_t *ciphertext, unsigned int ciphertext_len)
{
    memset_s(ciphertext, 0, ciphertext_len);

    int num_blocks = plaintext_len / AES_BLOCK_SIZE_BYTES;

    // encrypt the full blocks
    for(int i = 0; i < num_blocks; i++){
        AES_encrypt_block(ctx, &plaintext[i * AES_BLOCK_SIZE_BYTES], &ciphertext[i * AES_BLOCK_SIZE_BYTES]);
    }

    return CRYPT_OKAY;
}

// void print_block(uint8_t *bytes){
//     for(int i = 0; i < 16; i++){
//         printf("%02X ", bytes[i]);
//     }
//     printf("\n");
// }

crypt_status AES_encrypt_CBC(aes_ctx *ctx, const uint8_t *plaintext, unsigned int plaintext_len,
                            const uint8_t *iv, uint8_t *ciphertext, unsigned int ciphertext_len)
{
    int num_blocks = (plaintext_len / AES_BLOCK_SIZE_BYTES) + ((plaintext_len % AES_BLOCK_SIZE_BYTES) ? 1 : 0);
    unsigned int padded_len = num_blocks * AES_BLOCK_SIZE_BYTES + (AES_BLOCK_SIZE_BYTES - (plaintext_len % AES_BLOCK_SIZE_BYTES));

    memset_s(ciphertext, 0, ciphertext_len);

    uint8_t *plaintext_padded = (uint8_t *)malloc(padded_len);
    if (!plaintext_padded) {
        return CRYPT_FAILURE;
    }
    memcpy(plaintext_padded, plaintext, plaintext_len);
    uint8_t len_to_pad = AES_BLOCK_SIZE_BYTES - (plaintext_len % AES_BLOCK_SIZE_BYTES);
    memset_s(&plaintext_padded[plaintext_len], len_to_pad, len_to_pad);

    uint8_t xor_block[AES_BLOCK_SIZE_BYTES];
    memcpy(xor_block, iv, AES_BLOCK_SIZE_BYTES);
    for(int i = 0; i < num_blocks; i++){
        for(int j = 0; j < AES_BLOCK_SIZE_BYTES; j++){
            xor_block[j] ^= plaintext_padded[j + i * AES_BLOCK_SIZE_BYTES];
        }
        AES_encrypt_block(ctx, xor_block, &ciphertext[i * AES_BLOCK_SIZE_BYTES]);
        memcpy(xor_block, &ciphertext[i * AES_BLOCK_SIZE_BYTES], AES_BLOCK_SIZE_BYTES); // Copy new ciphertext to xor_block for next block
    }

    free(plaintext_padded);
    return CRYPT_OKAY;
}

crypt_status AES_encrypt_CTR(aes_ctx *ctx, const uint8_t *plaintext, unsigned int plaintext_len,
                            const uint8_t *iv, uint8_t *ciphertext)
{
    uint32_t iv_32[4];
    memcpy(iv_32, iv, AES_BLOCK_SIZE_BYTES);
    
    // the first block of the ciphertext is the IV in CTR mode
    memcpy(ciphertext, iv, AES_BLOCK_SIZE_BYTES);

    int num_complete_blocks = plaintext_len / AES_BLOCK_SIZE_BYTES;
    int partial_block_len = plaintext_len % AES_BLOCK_SIZE_BYTES;
    int current_index = AES_BLOCK_SIZE_BYTES;
    uint8_t xor_block[AES_BLOCK_SIZE_BYTES];
   
    for(int i = 0; i < num_complete_blocks; i++){
        AES_encrypt_block(ctx, (uint8_t *)iv_32, xor_block);
        for(int j = 0; j < AES_BLOCK_SIZE_BYTES; j++){
            ciphertext[j + current_index] = plaintext[j + current_index - AES_BLOCK_SIZE_BYTES] ^ xor_block[j];
        }
        current_index += AES_BLOCK_SIZE_BYTES;
        // flip the endianness to make sure counter is incremented on the correct byte, then flip it back
        // there must be a better way to do it, maybe use iv as a byte array?
        iv_32[3] = LE32TOBE32(iv_32[3]); 
        iv_32[3]++;
        iv_32[3] = LE32TOBE32(iv_32[3]); 
    }

    if (partial_block_len > 0) {
        AES_encrypt_block(ctx, (uint8_t *)iv_32, xor_block);
        for(int j = 0; j < partial_block_len; j++) {
            ciphertext[j + current_index] = plaintext[j + current_index - AES_BLOCK_SIZE_BYTES] ^ xor_block[j];
        }
    }

    return CRYPT_OKAY;
}

crypt_status AES_encrypt_GCTR(aes_ctx *ctx, const uint8_t *plaintext, unsigned int plaintext_len,
    const uint8_t *iv, uint8_t *ciphertext)
{
    
    return CRYPT_OKAY;
}

///////////////////////////////////////////////////////////// Decryption functions /////////////////////////////////////////////////////////////
static void invSubBytes(aes_ctx *ctx){
    uint8_t row = 0;
    uint8_t col = 0;
    for(int i = 0; i < AES_BLOCK_SIZE_BYTES; i++) {
        row = (ctx->state[i] & 0xF0) >> 4;
        col = ctx->state[i] & 0x0F;
        ctx->state[i] = inv_sbox[row][col];
    }
}

/*
    The state matrix after shiftRows
    0  4  8   12                    0  4  8  12
    1  5  9   13        ===>        13 1  5   9
    2  6  10  14                    10 14 2   6
    3  7  11  15                    7  11 15  3
*/
static void invShiftRows(aes_ctx *ctx){
    uint8_t tmp1, tmp2;
    tmp1 = ctx->state[13];
    ctx->state[13] = ctx->state[9];
    ctx->state[9] = ctx->state[5];
    ctx->state[5] = ctx->state[1];
    ctx->state[1] = tmp1;

    tmp1 = ctx->state[10];
    tmp2 = ctx->state[14];
    ctx->state[14] = ctx->state[6];
    ctx->state[10] = ctx->state[2];
    ctx->state[6] = tmp2;
    ctx->state[2] = tmp1;

    tmp1 = ctx->state[3];
    ctx->state[3] = ctx->state[7];
    ctx->state[7] = ctx->state[11];
    ctx->state[11] = ctx->state[15];
    ctx->state[15] = tmp1;
    
}

/* 
    This function multiplies x by y in GF(2^8), where y is a constant 0x09, 0x0b, 0x0d, or 0x0e
    we 4 bits (0b1000, 0b0100, 0b0010, 0b0001) to figure out how to combine the products of 2^n to produce the final result 
    This combination is specified in section 4.2.1 of NIST FIPS 197
*/
static uint8_t multiply_GF(uint8_t x, uint8_t y) {

    uint8_t xtimes_1 = x;
    uint8_t xtimes_2 = xtime(x);
    uint8_t xtimes_4 = xtime(xtimes_2);
    uint8_t xtimes_8 = xtime(xtimes_4);
    uint8_t result = 0;
    uint8_t mask;

    mask = -((y & 1) >> 0);
    result ^= xtimes_1 & mask;

    mask = -((y & 2) >> 1);
    result ^= xtimes_2 & mask;

    mask = -((y & 4) >> 2);
    result ^= xtimes_4 & mask;

    mask = -((y & 8) >> 3);
    result ^= xtimes_8 & mask;

    return result;
}

static void invMixColumn(aes_ctx *ctx){
    for(int col = 0; col < 4; col++) {
        uint8_t s0 = ctx->state[0 + 4*col];
        uint8_t s1 = ctx->state[1 + 4*col];
        uint8_t s2 = ctx->state[2 + 4*col];
        uint8_t s3 = ctx->state[3 + 4*col];

        ctx->state[0 + 4*col] = multiply_GF(s0, 0x0e) ^ multiply_GF(s1, 0x0b) ^ multiply_GF(s2, 0x0d) ^ multiply_GF(s3, 0x09);
        ctx->state[1 + 4*col] = multiply_GF(s0, 0x09) ^ multiply_GF(s1, 0x0e) ^ multiply_GF(s2, 0x0b) ^ multiply_GF(s3, 0x0d);
        ctx->state[2 + 4*col] = multiply_GF(s0, 0x0d) ^ multiply_GF(s1, 0x09) ^ multiply_GF(s2, 0x0e) ^ multiply_GF(s3, 0x0b);
        ctx->state[3 + 4*col] = multiply_GF(s0, 0x0b) ^ multiply_GF(s1, 0x0d) ^ multiply_GF(s2, 0x09) ^ multiply_GF(s3, 0x0e);
    }
}

static void AES_decrypt_block(aes_ctx *ctx, const uint8_t *input, uint8_t *output){
    // copy the input into the state
    for(int i =0; i < AES_BLOCK_SIZE_BYTES; i++){
        ctx->state[i] = input[i];
    }
    
    int num_rounds;
    int current_round_key_index = 0;
    switch(ctx->aes_key_len){
        case AES_128:
            num_rounds = AES_128_NUM_ROUNDS;
            break; 
        case AES_192:
            num_rounds = AES_192_NUM_ROUNDS;
            break; 
        case AES_256:
            num_rounds = AES_256_NUM_ROUNDS;
            break; 
        default:
            break;
    } 
    
    uint8_t *round_key_bytes = (uint8_t *)ctx->round_keys;

    addRoundKey(ctx, &round_key_bytes[current_round_key_index]);
    current_round_key_index += 16;

    // perform rounds of AES transformation - last round does not include mixColumn step, so it 
    // is performed outside of the for loop
    for(int round = num_rounds; round >= 2; round--){
        invShiftRows(ctx);
        invSubBytes(ctx);
        addRoundKey(ctx, &round_key_bytes[current_round_key_index]);
        invMixColumn(ctx);
        current_round_key_index += 16;
    }

    invShiftRows(ctx);
    invSubBytes(ctx);
    addRoundKey(ctx, &round_key_bytes[current_round_key_index]);

    for(int i = 0; i < AES_BLOCK_SIZE_BYTES; i++){
        output[i] = ctx->state[i];
    }

}

crypt_status AES_decrypt_ECB(aes_ctx *ctx, 
                                const uint8_t *ciphertext, unsigned int ciphertext_len,
                                uint8_t *plaintext, unsigned int plaintext_len)
{
    memset_s(plaintext, 0, plaintext_len);

    int num_blocks = ciphertext_len / AES_BLOCK_SIZE_BYTES;

    // decrypt the full blocks
    for(int i = 0; i < num_blocks; i++){
        AES_decrypt_block(ctx, &ciphertext[i * AES_BLOCK_SIZE_BYTES], &plaintext[i * AES_BLOCK_SIZE_BYTES]);
    }

    return CRYPT_OKAY;

}

crypt_status AES_decrypt_CBC(aes_ctx *ctx, const unsigned char *ciphertext, unsigned int ciphertext_len,
                            const uint8_t *iv, unsigned char *plaintext, unsigned int plaintext_len)
{

    memset_s(plaintext, 0, plaintext_len);
    
    uint8_t xor_block[AES_BLOCK_SIZE_BYTES];
    memcpy(xor_block, iv, AES_BLOCK_SIZE_BYTES);
    
    // for the first block we XOR the decrypted block with IV, after that the previous ciphertext 
    // blocks becomes the IV in CBC mode
    for(unsigned int i = 0; i < ciphertext_len / AES_BLOCK_SIZE_BYTES; i++) {
        AES_decrypt_block(ctx, &ciphertext[i * AES_BLOCK_SIZE_BYTES], &plaintext[i * AES_BLOCK_SIZE_BYTES]);
        for(int j = 0; j < AES_BLOCK_SIZE_BYTES; j++){
            plaintext[j + i * AES_BLOCK_SIZE_BYTES] ^= xor_block[j];
        }
        
        memcpy(xor_block, &ciphertext[i * AES_BLOCK_SIZE_BYTES], AES_BLOCK_SIZE_BYTES);
    }

    // Remove padding
    int pad_len = plaintext[plaintext_len - 1];
    if (pad_len > 0 && pad_len <= AES_BLOCK_SIZE_BYTES) {
        plaintext_len -= pad_len;
    }

    return CRYPT_OKAY;
}

crypt_status AES_decrypt_CTR(aes_ctx *ctx, const uint8_t *ciphertext,
                            uint8_t *plaintext, unsigned int plaintext_len)
{
    uint32_t iv[4];
    // The first block of the ciphertext is the IV used to encrypt the message in CTR mode
    memcpy(iv, ciphertext, AES_BLOCK_SIZE_BYTES);

    int num_complete_blocks = plaintext_len / AES_BLOCK_SIZE_BYTES;
    int partial_block_len = plaintext_len % AES_BLOCK_SIZE_BYTES;
    int current_index = 0;
    uint8_t xor_block[AES_BLOCK_SIZE_BYTES];

    for(int i = 0; i < num_complete_blocks; i++){
        AES_encrypt_block(ctx, (uint8_t *)iv, xor_block);
        for(int j = 0; j < AES_BLOCK_SIZE_BYTES; j++){
            plaintext[j + current_index] = ciphertext[j + current_index + AES_BLOCK_SIZE_BYTES] ^ xor_block[j];
        }
        current_index += AES_BLOCK_SIZE_BYTES;
        iv[3] = LE32TOBE32(iv[3]); 
        iv[3]++;
        iv[3] = LE32TOBE32(iv[3]); 
    }

    if (partial_block_len > 0) {
        AES_encrypt_block(ctx, (uint8_t *)iv, xor_block);
        for(int j = 0; j < partial_block_len; j++) {
            plaintext[j + current_index] = ciphertext[j + current_index + AES_BLOCK_SIZE_BYTES] ^ xor_block[j];
        }
    }

    return CRYPT_OKAY;
}


crypt_status AES_cleanup(aes_ctx *ctx){

    memset_s(ctx->state, 0, sizeof(ctx->state));
    memset_s(ctx->round_keys, 0, sizeof(ctx->round_keys));

    return CRYPT_OKAY;
}