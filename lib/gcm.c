#include "gcm.h"
#include "bigint.h"


uint8_t R[16] = {
    0xE1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// this function assumes all inputs and output are 16 bytes long 
static void xor_arrays(uint8_t *x, uint8_t *y, uint8_t *z){
    for(int i = 0; i < 16; i++){
        z[i] = x[i] ^ y[i];
    }
}
// this function performs the multiplication of two blocks in GF(2^128)
static void GF_mul_blocks(uint8_t *X, uint8_t *Y, uint8_t *res){
    uint8_t Z0[16] = {0};
    uint8_t V0[16];
    memcpy(V0, X, sizeof(V0));

    bigint Z, V, bi_Y, bi_R, tmp, one;
    bigint_init(&Z, GCM_BLOCK_SIZE_BYTES / sizeof(digit));
    bigint_init(&bi_Y, GCM_BLOCK_SIZE_BYTES / sizeof(digit));
    bigint_init(&bi_R, GCM_BLOCK_SIZE_BYTES / sizeof(digit));
    bigint_init(&V, GCM_BLOCK_SIZE_BYTES / sizeof(digit));
    bigint_init(&tmp, GCM_BLOCK_SIZE_BYTES / sizeof(digit));
    bigint_init(&one, 1);

    bigint_from_small_int(&one, 1);
    bigint_from_bytes(&V, X, GCM_BLOCK_SIZE_BYTES);
    bigint_from_bytes(&bi_R, R, sizeof(R));

    for(int i = 0; i < 128; i++){
        if (bigint_is_bit_set(&bi_Y, i)){
            bigint_xor(&Z, &V, &Z);
        }
        if (!bigint_is_bit_set(&V, 127)) {
            bigint_right_bit_shift(&V, &V);
        } else {
            bigint_right_bit_shift(&V, &V);
            bigint_xor(&V, &bi_R, &V);
        }
    }


    bigint_free(&Z);
    bigint_free(&V);
    bigint_free(&tmp);
    bigint_free(&one);
    bigint_free(&bi_Y);

}