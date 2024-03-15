#ifndef MD5_H
#define MD5_H

#include "common.h"

#define MD5_REQ_LEN_BYTES 64
#define MD5_LEN_FIELD_SIZE_BYTES 8

typedef struct {

} md5_ctx;

crypt_status md5_pad(unsigned char *input, unsigned int input_len, 
                    unsigned char * output, unsigned int output_len);

#endif /* MD5_H */