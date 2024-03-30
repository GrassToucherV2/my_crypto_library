#include "common.h"
#include "chacha20.h"
#include "poly1305.h"
#include <stdint.h>

#define CHACHA20_POLY1305_IV_LEN_BYTES 8
#define CHACHA20_POLY1305_CONSTANT_LEN_BYTES 4

typedef struct {
    uint8_t key[32];
    chacha20_ctx chacha_ctx;
    poly1305_ctx poly_ctx;
    
} chacha20_poly1305_ctx;

crypt_status chacha20_poly1305_init(chacha20_poly1305_ctx *ctx, const unsigned char *key, unsigned int keylen);

crypt_status chacha20_poly1305_encrypt(chacha20_poly1305_ctx *ctx, const unsigned char *plaintext, unsigned int plaintext_len,
                                        unsigned char *ciphertext, unsigned int ciphertext_len, 
                                        const unsigned char *constant, unsigned int constant_len,
                                        const unsigned char *iv, unsigned int iv_len,
                                        const unsigned char *aad, unsigned int aad_len, 
                                        unsigned char *aead_output, unsigned int aead_output_len);

crypt_status chacha20_poly1305_decrypt(chacha20_poly1305_ctx *ctx, const unsigned char *encrypted_text, unsigned int encrypted_text_len,
                                        const unsigned char *constant, unsigned int constant_len, 
                                        const unsigned char *iv, unsigned int iv_len,
                                        const unsigned char *aad, unsigned int aad_len, 
                                        unsigned char *plaintext, unsigned int plaintext_len);

crypt_status chacha20_poly1305_cleanup(chacha20_poly1305_ctx *ctx);

