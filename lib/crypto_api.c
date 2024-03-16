#include "crypto_api.h"

#include "md5.h"


#include <stdio.h>
crypt_status md5(md5_ctx *ctx, const unsigned char *input,
                 unsigned int input_len, unsigned char *output,
                 unsigned int output_len
)
{
    if(!ctx || !input || !output) return CRYPT_NULL_PTR;

    if((output_len) != 16){
        return CRYPT_BAD_BUFFER_LEN;
    }
    CRYPT_CHECK_OKAY(md5_init(ctx));
    CRYPT_CHECK_OKAY(md5_update(ctx, input, input_len));
    CRYPT_CHECK_OKAY(md5_finish(ctx, output));

    return CRYPT_OKAY;
}