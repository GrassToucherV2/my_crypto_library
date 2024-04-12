#include "common.h"
#include <stdint.h>

#define DES_BLOCK_SIZE_BITS 64
#define DES_BLOCK_SIZE_BYTES 8
#define DES_KEY_SIZE_BITS 56
#define DES_SUBKEY_SIZE_BITS 48
#define DES_RIGHT_EXPANDED_SIZE_BITS 48
#define DES_NUM_ROUNDS 16

typedef struct des_ctx{
    uint64_t subkeys[16];
} des_ctx;

crypt_status des_init(des_ctx *ctx, uint64_t key);

crypt_status DES_encrypt_ECB(des_ctx *ctx, unsigned char *plaintext, unsigned int plaintext_len,
                        unsigned char *ciphertext, unsigned int ciphertext_len);

crypt_status DES_encrypt_CBC(des_ctx *ctx, unsigned char *plaintext, unsigned int plaintext_len,
                            uint64_t iv, unsigned char *ciphertext, unsigned int ciphertext_len);

void des_key_test(uint64_t key, uint64_t *key_56);