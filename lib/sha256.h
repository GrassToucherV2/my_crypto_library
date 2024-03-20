#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>
#include "common.h"

#define SHA256_DIGEST_LEN_BYTES 32
#define SHA224_DIGEST_LEN_BYTES 28
#define SHA2_MSG_BLOCK_LEN_BYTES 64

// SHA224 and SHA256 can share the same context as SHA224 is basically truncated SHA256 with different initial state
typedef struct {
    uint32_t state[8];          // message digests
    uint64_t bit_count;                     // message length in bits
    uint8_t buffer[64];              // 512-bit message blocks
} sha224_256_ctx;

crypt_status sha224_256_init(sha224_256_ctx *ctx, SHA2 sha);
crypt_status sha224_256_update(sha224_256_ctx *ctx, const unsigned char *input, unsigned int input_len);
crypt_status sha224_256_finish(sha224_256_ctx *ctx, unsigned char *digest, 
                                unsigned int digest_len, SHA2 sha);

#endif /* SHA256_H */