#ifndef SHA1_H
#define SHA1_H

#include <stdint.h>
#include "common.h"

#define SHA1_DIGEST_LEN_BYTES 20
#define SHA1_MEG_BLOCK_LEN_BYTES 64

typedef struct {
    uint32_t state[5];          // message digests
    uint64_t bit_count;                     // message length in bits
    uint8_t buffer[64];              // 512-bit message blocks
} sha1_ctx;

crypt_status sha1_init(sha1_ctx *ctx);
crypt_status sha1_update(sha1_ctx *ctx, const unsigned char *input, unsigned int input_len);
crypt_status sha1_finish(sha1_ctx *ctx, unsigned char digest[20], unsigned int digest_len);

#endif /* SHA1_H */