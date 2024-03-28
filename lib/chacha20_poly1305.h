#include "common.h"
#include "chacha20.h"
#include "poly1305.h"
#include <stdint.h>

typedef struct {
    uint8_t key[32];
    chacha20_ctx *chacha_ctx;
    poly1305_ctx *poly_ctx;
    
} chacha20_poly1305_ctx;

crypt_status chacha20_poly1305_encrypt(chacha20_poly1305_ctx *ctx, unsigned char *plaintext, unsigned int plaintext_len,
                                        unsigned char *ciphertext, unsigned int ciphertext_len, uint32_t constant, 
                                        const unsigned char *iv, unsigned int iv_len,
                                        const unsigned char *aad, unsigned int aad_len, 
                                        unsigned char *aead_output, unsigned int aead_output_len);

