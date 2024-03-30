#ifndef CHACHA20_H
#define CHACHA20_H

#include <stdint.h>
#include "common.h"

#define CHACHA20_STATE_LEN_BYTES 64
#define CHACHA20_KEYSTREAM_LEN_BYTES 64
#define CHACHA20_KEY_LEN_BYTES 32
#define CHACHA20_NONCE_LEN_BYTES 12

typedef struct{
    uint32_t state[16];
} chacha20_ctx;

crypt_status chacha20_init(chacha20_ctx *ctx, const unsigned char *key);

crypt_status chacha20_crypt(chacha20_ctx *ctx, unsigned int counter,
                            const unsigned char *nonce, unsigned int nonce_len, 
                            const unsigned char *input, unsigned int input_len, 
                            unsigned char *output, unsigned int output_len);

crypt_status chacha20_cleanup(chacha20_ctx *ctx);

void chacha20_block(chacha20_ctx *ctx, unsigned char keystream[CHACHA20_KEYSTREAM_LEN_BYTES]);
void print_chacha20_state(const uint32_t state[16]);


#endif /* CHACHA20_H */
