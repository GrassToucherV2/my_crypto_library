#include "common.h"
#include <stdint.h>

#define DES_BLOCK_SIZE_BITS 64
#define DES_KEY_SIZE_BITS 56
#define DES_SUBKEY_SIZE_BITS 48
#define DES_NUM_ROUNDS 16

void des_key_test(uint64_t key, uint64_t *key_56);

typedef struct des_ctx{
    uint64_t subkeys[16];
} des_ctx;

crypt_status des_init(des_ctx *ctx, uint64_t key);