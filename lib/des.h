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

typedef struct tdes_ctx{
    uint64_t subkeys1[16];
    uint64_t subkeys2[16];
    uint64_t subkeys3[16];
} tdes_ctx;

crypt_status DES_init(des_ctx *ctx, uint64_t key);

crypt_status DES_encrypt_ECB(des_ctx *ctx, const unsigned char *plaintext, unsigned int plaintext_len,
                        unsigned char *ciphertext, unsigned int ciphertext_len, padding_scheme padding);

crypt_status DES_decrypt_ECB(des_ctx *ctx, const unsigned char *ciphertext, unsigned int ciphertext_len,
                             unsigned char *plaintext, unsigned int plaintext_len, padding_scheme padding);

crypt_status DES_encrypt_CBC(des_ctx *ctx, const unsigned char *plaintext, unsigned int plaintext_len,
                            uint64_t iv, unsigned char *ciphertext, unsigned int ciphertext_len);

crypt_status DES_decrypt_CBC(des_ctx *ctx, const unsigned char *ciphertext, unsigned int ciphertext_len,
                            uint64_t iv, unsigned char *plaintext, unsigned int plaintext_len);

crypt_status DES_cleanup(des_ctx *ctx);

//////////////////////////////////////////////////////// 3DES functions ////////////////////////////////////////////////////////

crypt_status TDES_init(tdes_ctx *ctx, uint64_t key1, uint64_t key2, uint64_t key3);

crypt_status TDES_encrypt_ECB(tdes_ctx *ctx, const unsigned char *plaintext, unsigned int plaintext_len,
                        unsigned char *ciphertext, unsigned int ciphertext_len, padding_scheme padding);

crypt_status TDES_decrypt_ECB(tdes_ctx *ctx, const unsigned char *ciphertext, unsigned int ciphertext_len,
                             unsigned char *plaintext, unsigned int plaintext_len, padding_scheme padding);

crypt_status TDES_encrypt_CBC(tdes_ctx *ctx, const unsigned char *plaintext, unsigned int plaintext_len,
                            uint64_t iv, unsigned char *ciphertext, unsigned int ciphertext_len);

crypt_status TDES_decrypt_CBC(tdes_ctx *ctx, const unsigned char *ciphertext, unsigned int ciphertext_len,
                            uint64_t iv, unsigned char *plaintext, unsigned int plaintext_len);

crypt_status TDES_cleanup(tdes_ctx *ctx);
