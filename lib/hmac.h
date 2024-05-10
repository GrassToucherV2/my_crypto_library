#ifndef HMAC_H
#define HMAC_H

#include "common.h"


/*
    different hashing functions work with blocks with different sizes 
        - SHA256 uses 64 bytes 
        - SHA384 uses 128 bytes
    allocating 128 bytes to padded key to fit all possible sizes
*/
#define HMAC_PADDED_KEY_LEN 128
#define HMAC_IPAD_BYTE 0x36
#define HMAC_OPAD_BYTE 0x5c

typedef struct hmac_ctx{
    uint8_t padded_key[HMAC_PADDED_KEY_LEN];
    uint8_t inner_pad[HMAC_PADDED_KEY_LEN];
    uint8_t outer_pad[HMAC_PADDED_KEY_LEN];
} hmac_ctx;

void hmac_init( hmac_ctx *ctx,
                        const uint8_t *key, unsigned int key_len, 
                        unsigned int block_size);

void hmac_sha256_compute_mac(hmac_ctx *ctx, 
                                const uint8_t *text, unsigned int text_len,
                                uint8_t *mac, unsigned int mac_len);


void hmac_sha384_compute_mac(hmac_ctx *ctx, 
                                const uint8_t *text, unsigned int text_len,
                                uint8_t *mac, unsigned int mac_len);

void hmac_sha512_compute_mac(hmac_ctx *ctx, 
                                const uint8_t *text, unsigned int text_len,
                                uint8_t *mac, unsigned int mac_len);

void hmac_cleanup(hmac_ctx *ctx);

#endif /* HMAC_H */