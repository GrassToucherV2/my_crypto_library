#ifndef POLY1305_H
#define POLY1305_H

#include <stdint.h>
#include "common.h"

#define POLY1305_KEY_LEN_BYTES 32
#define POLY1305_S_LEN_BYTES 16
#define POLY1305_R_LEN_BYTES 16
#define POLY1305_MAC_LEN_BYTES 16
#define POLY1305_BLOCK_LEN_BYTES 16

typedef struct {
    // uint8_t buffer[16];
    uint8_t accumulator[20];
    uint8_t r[16];
    uint8_t s[16];
    uint8_t polykey[32];
    // uint32_t buffer_len;
} poly1305_ctx;

void poly1305_keygen(const unsigned char *key, const unsigned char *nonce,
                     unsigned char *polykey);

// crypt_status poly1305_init(poly1305_ctx *ctx, const unsigned char *key, unsigned int key_len,
//                             const unsigned char *nonce, unsigned int nonce_len);

crypt_status poly1305_init(poly1305_ctx *ctx, const unsigned char *key, unsigned int polykey_len);

crypt_status poly1305_compute_mac(poly1305_ctx *ctx, const unsigned char *input,
                            unsigned int input_len, unsigned char *mac, 
                            unsigned int mac_len);

#endif /* POLY1305_H */
