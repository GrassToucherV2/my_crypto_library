#ifndef MD5_H
#define MD5_H

#include "common.h"
#include <inttypes.h>

#define MD5_BLOCK_SIZE_BYTES 64 
#define MD5_REQ_LEN_BYTES 64
#define MD5_LEN_FIELD_SIZE_BYTES 8
#define MD5_DIGESTS_LEN_BYTES 16

typedef struct {
    uint32_t state[4];
    uint64_t bit_count;
    unsigned char buffer[64];
} md5_ctx;

crypt_status md5_init(md5_ctx *ctx);
crypt_status md5_update(md5_ctx *ctx, const unsigned char *input, unsigned int input_len);
crypt_status md5_finish(md5_ctx *ctx, unsigned char *digest);

#endif /* MD5_H */