#ifndef CRYPTO_API_H
#define CRYPTO_API_H

#include "md5.h"
#include "common.h"

crypt_status md5(md5_ctx *ctx, const unsigned char *input,
                 unsigned int input_len, unsigned char *output,
                 unsigned int output_len
);

#endif /* CRYPTO_API_H */