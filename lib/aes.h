#ifndef AES_H
#define AES_H

#include <stdint.h>

#include "common.h"

#define AES_NUM_BLOCK 4
#define AES_BLOCK_SIZE_BYTES 16
#define AES_128_KEY_SIZE_BYTES 16
#define AES_192_KEY_SIZE_BYTES 24
#define AES_256_KEY_SIZE_BYTES 32

#define AES_128_NUM_ROUNDS 10
#define AES_192_NUM_ROUNDS 12
#define AES_256_NUM_ROUNDS 14

/*
    The state array representation of the state matrix
    0  4  8   12
    1  5  9   13
    2  6  10  14
    3  7  11  15

    to get each index: row + (4 * col), everything zero indexed

    round_keys length for AES-128 = 44 words
                          AES-192 = 52 words
                          AES-256 = 60 words 
*/
typedef struct aes_ctx {
    uint8_t state[16];
    uint32_t round_keys[60];
    AES_key_length aes_key_len;
} aes_ctx;

crypt_status AES_init(aes_ctx *ctx, const uint8_t *key, AES_key_length key_len);


crypt_status AES_cleanup(aes_ctx *ctx);

#endif /* AES_H */