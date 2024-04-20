#ifndef AES_H
#define AES_H

#include <stdint.h>

#include "common.h"

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
*/
typedef struct aes_ctx {
    uint8_t state[16];
} aes_ctx;

#endif /* AES_H */