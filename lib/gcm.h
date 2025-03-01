#ifndef GCM_H
#define GCM_H

#include <stdint.h>
#include "common.h"

#define GCM_BLOCK_SIZE_BYTES 16
#define GCM_IV_SIZE_BYTES 12

void inc32(uint8_t *X);
void GF_mul_blocks(uint8_t *X, uint8_t *Y, uint8_t *res);
void GHASH(uint8_t *H, uint8_t *text_in,
    uint32_t text_in_len, uint8_t *output, uint32_t output_len);


#endif /* GCM_H */