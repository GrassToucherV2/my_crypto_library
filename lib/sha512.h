#ifndef SHA512_H
#define SHA512_H

#include <stdint.h>
#include "common.h"

#define SHA512_DIGEST_LEN_BYTES 64
#define SHA384_DIGEST_LEN_BYTES 48
#define SHA512_MSG_BLOCK_LEN_BYTES 128

// SHA512 and SHA384 can share the same context
typedef struct {
    uint64_t state[8];                      // message digests
    uint64_t bit_count;                     // message length in bits
    uint8_t buffer[128];                    // 1024-bit message blocks
} sha384_512_ctx;

crypt_status sha384_512_init(sha384_512_ctx *ctx, SHA2 sha);
crypt_status sha384_512_update(sha384_512_ctx *ctx, const unsigned char *input, unsigned int input_len);
crypt_status sha384_512_finish(sha384_512_ctx *ctx, unsigned char *digest, 
                                unsigned int digest_len, SHA2 sha);

#endif /* SHA512_H */