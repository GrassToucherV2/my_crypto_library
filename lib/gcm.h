#ifndef GCM_H
#define GCM_H

#include <stdint.h>
#include "common.h"

#define GCM_BLOCK_SIZE_BYTES 16
#define GCM_IV_SIZE_BYTES 12

void inc32(uint8_t *X);
void xor_blocks(const uint8_t *x, const uint8_t *y, uint8_t *z);
void GF_mul_blocks(const uint8_t *X, const uint8_t *Y, uint8_t *res);
void GHASH(const uint8_t *H, 
           const uint8_t *aad, uint32_t aad_len,
           const uint8_t *ciphertext, uint32_t ciphertext_len,
           uint8_t *output);


#endif /* GCM_H */
