#include "gcm.h"
#include "bigint.h"
#include "common.h"
#include "util/tools.h"

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

/*
    s-bit increment function described in NIST GMC, sec 6.2
    - X is a bit string
    - s is an integer, denoting the number of bits

    /////// check for endianness when testing
*/
static void increment_s(uint8_t *X, int X_size_bytes, int s, uint8_t *out){

    reverse_byte_order(X, X, X_size_bytes);

    // extract s left most bits of X
    int MSB_num_bytes = (X_size_bytes * NUM_BITS_IN_BYTE - s) >> 3;                // (len(X) - s) / 8
    int MSB_num_remaining_bits = 0;
    if(!s){
        MSB_num_remaining_bits = (X_size_bytes * NUM_BITS_IN_BYTE - s) & 0x07;     // (len(X) - s) % 8
    }
    
    uint8_t tmp_X_MSB[GCM_BLOCK_SIZE_BYTES] = {0};

    // extract full MSB bytes first
    memcpy(tmp_X_MSB, X, MSB_num_bytes);

    // extract the remaining bits
    tmp_X_MSB[MSB_num_bytes] = X[MSB_num_bytes] >> (NUM_BITS_IN_BYTE - MSB_num_remaining_bits);


    int LSB_num_bytes = 0;
    int LSB_num_bits = 0;
    if(!MSB_num_remaining_bits){
        LSB_num_bytes = X_size_bytes - MSB_num_bytes - 1;
        LSB_num_bits = NUM_BITS_IN_BYTE - MSB_num_remaining_bits;
    } else{
        LSB_num_bytes = X_size_bytes - MSB_num_bytes;
    }

    uint8_t tmp_X_LSB[16] = {0};
    tmp_X_LSB[0] = X[MSB_num_bytes] & ((1 << (NUM_BITS_IN_BYTE - MSB_num_remaining_bits)) - 1);
    memcpy(&tmp_X_LSB[1], &X[MSB_num_bytes + 1], LSB_num_bytes - 1);


    bigint lsb, lsb1, r;
    bigint_init(&lsb, GCM_BLOCK_SIZE_BYTES / sizeof(digit));
    bigint_init(&r, GCM_BLOCK_SIZE_BYTES / sizeof(digit));
    bigint_init(&lsb1, GCM_BLOCK_SIZE_BYTES / sizeof(digit));

    int lsb_num_bytes_bigint = X_size_bytes - MSB_num_bytes;
    bigint_from_bytes(&lsb, tmp_X_LSB, lsb_num_bytes_bigint);

    bigint_add_digit(&lsb, 1, &lsb1);
    bigint_mod_pow_2(&lsb1, s, &r);

    bigint_to_bytes(&r, tmp_X_LSB, GCM_BLOCK_SIZE_BYTES, 0);
    bigint_free(&r);
    bigint_free(&lsb1);
    bigint_free(&lsb);

    int max_len_lsb = s >> 3;
    // assemble the final result
    memcpy(out, tmp_X_MSB, MSB_num_bytes + 1);
    // out[MSB_num_bytes] = (out[MSB_num_bytes] << MSB_num_remaining_bits) | tmp_X_LSB[0];
    if (MSB_num_remaining_bits > 0) {
        out[MSB_num_bytes] = (out[MSB_num_bytes] << MSB_num_remaining_bits) | tmp_X_LSB[0];
    }
    memcpy(&out[MSB_num_bytes + 1], &tmp_X_LSB[1], max_len_lsb); 

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