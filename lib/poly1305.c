#include "poly1305.h"
#include "chacha20.h"
#include "bigint.h"
#include "../util/tools.h"

void poly1305_clamp(unsigned char r[16]){
    r[3] &= 15;
    r[7] &= 15;
    r[11] &= 15;
    r[15] &= 15;
    r[4] &= 252;
    r[8] &= 252;
    r[12] &= 252;
}

static void poly1305_keygen(const unsigned char *key, const unsigned char *nonce,
                            unsigned char *polykey)
{   
    unsigned char keystream[64];                    
    chacha20_ctx ctx = {0};
    chacha20_init(&ctx, key, nonce, 0);
    chacha20_block(&ctx, keystream);
    memcpy(polykey, keystream, POLY1305_KEY_LEN_BYTES);
}

crypt_status poly1305_init(poly1305_ctx *ctx, const unsigned char *key, unsigned int key_len,
                            const unsigned char *nonce, unsigned int nonce_len)
{
    if(!ctx || !key || !nonce) return CRYPT_NULL_PTR;

    if(key_len != CHACHA20_KEY_LEN_BYTES){
        return CRYPT_BAD_KEY;
    }

    if(nonce_len != CHACHA20_NONCE_LEN_BYTES){
        return CRYPT_BAD_NONCE;
    }

    unsigned char polykey[32];
    poly1305_keygen(key, nonce, polykey);

    uint64_t *r_64 = (uint64_t *)ctx->r;
    uint64_t *s_64 = (uint64_t *)ctx->s;
    uint64_t *polykey_64 = (uint64_t *)polykey;

    r_64[0] = BE64TOLE64(polykey_64[0]);
    r_64[1] = BE64TOLE64(polykey_64[1]);
    s_64[0] = BE64TOLE64(polykey_64[2]);
    s_64[1] = BE64TOLE64(polykey_64[3]);

    poly1305_clamp(ctx->r);

    memset(ctx->accumulator, 0, sizeof(ctx->accumulator));

    return CRYPT_OKAY;
}

crypt_status poly1305_init_test(poly1305_ctx *ctx, const unsigned char *key){

    memcpy(ctx->polykey, key, 32);
    uint64_t *r_64 = (uint64_t *)ctx->r;
    uint64_t *s_64 = (uint64_t *)ctx->s;
    uint64_t *polykey_64 = (uint64_t *)ctx->polykey;

    //  poly1305_clamp(ctx->polykey);
    r_64[0] = BE64TOLE64(polykey_64[0]);
    r_64[1] = BE64TOLE64(polykey_64[1]);
    s_64[0] = BE64TOLE64(polykey_64[2]);
    s_64[1] = BE64TOLE64(polykey_64[3]);

    poly1305_clamp(ctx->r);
   
    memset(ctx->accumulator, 0, sizeof(ctx->accumulator));
    // memset(ctx->buffer, 0, sizeof(ctx->buffer));
    // ctx->buffer_len = 0;

    return CRYPT_OKAY;
}

static const unsigned char prime[] = {
    0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xfb
};

// using this to change the endianness
void reverse_array(unsigned char *arr_in, unsigned char *arr_out, int length) {
    if (arr_in != arr_out) {
        // If input and output buffers are different, copy while reversing
        for (int i = 0; i < length; ++i) {
            arr_out[i] = arr_in[length - 1 - i];
        }
    } else {
        // If input and output buffers are the same, swap in place
        for (int i = 0; i < length / 2; ++i) {
            unsigned char temp = arr_in[i];
            arr_in[i] = arr_in[length - 1 - i];
            arr_in[length - 1 - i] = temp;
        }
    }
}

crypt_status poly1305_compute_mac(poly1305_ctx *ctx, const unsigned char *input,
                            unsigned int input_len, unsigned char *mac, 
                            unsigned int mac_len)
{
    if(!ctx || !input || !mac) return CRYPT_NULL_PTR;

    if(mac_len != POLY1305_MAC_LEC_BYTES){
        return CRYPT_BAD_BUFFER_LEN;
    }
    
    bigint r, s, a, p, n;

    CHECK_BIGINT_OKAY(bigint_init(&r, 4));
    CHECK_BIGINT_OKAY(bigint_init(&s, 4));
    CHECK_BIGINT_OKAY(bigint_init(&a, 5));
    CHECK_BIGINT_OKAY(bigint_init(&p, 5));
    CHECK_BIGINT_OKAY(bigint_init(&n, 5));

    uint8_t r_le[16];
    uint8_t s_le[16];
    reverse_array(ctx->r, r_le, 16);
    reverse_array(ctx->s, s_le, 16);

    CHECK_BIGINT_OKAY(bigint_from_bytes(&r, r_le, sizeof(ctx->r)));
    CHECK_BIGINT_OKAY(bigint_from_bytes(&s, s_le, sizeof(s_le)));
    CHECK_BIGINT_OKAY(bigint_from_bytes(&a, ctx->accumulator, sizeof(ctx->accumulator)));
    CHECK_BIGINT_OKAY(bigint_from_bytes(&p, prime, sizeof(prime)));

    int blk_counter = 0;
    unsigned char n_arr[17] = {
        0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
    };

    uint8_t final_block[17] = {
        0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    uint8_t n_8[17] = {0}; 
    uint32_t remaining_len = input_len;
    
    // process full blocks first
    for(unsigned int i = 0;  i < input_len / POLY1305_BLOCK_LEN_BYTES; i++){   
        memcpy(&n_arr[0], &input[i * 16], POLY1305_BLOCK_LEN_BYTES); 
        reverse_array(n_arr, n_8, sizeof(n_arr));
        CHECK_BIGINT_OKAY(bigint_from_bytes(&n, (unsigned char *)n_8, sizeof(n_8)));
        CHECK_BIGINT_OKAY(bigint_add(&a, &n, &a));
        CHECK_BIGINT_OKAY(bigint_mul(&a, &r, &a));
        CHECK_BIGINT_OKAY(bigint_mod(&a, &p, &a));        
        remaining_len -= POLY1305_BLOCK_LEN_BYTES;
        blk_counter += POLY1305_BLOCK_LEN_BYTES;
    }
    // if there are remaining blocks, we pad it and then process it. Padding is done by 
    // copy the remaining bytes into final_block, which is all 0x00 initially, then 
    // change the endianness to fit the requirement, the same way as how it is done for
    // full blocks
    if(remaining_len < 16){
        memcpy(final_block, &input[blk_counter], input_len - blk_counter);
        final_block[input_len - blk_counter] = 0x01;
        reverse_array(final_block, n_8, sizeof(final_block));
        CHECK_BIGINT_OKAY(bigint_from_bytes(&n, (unsigned char *)n_8, sizeof(n_8)));
        CHECK_BIGINT_OKAY(bigint_add(&a, &n, &a));
        CHECK_BIGINT_OKAY(bigint_mul(&a, &r, &a));
        CHECK_BIGINT_OKAY(bigint_mod(&a, &p, &a));
    }

    CHECK_BIGINT_OKAY(bigint_add(&a, &s, &a));

    // keeping on the lower 128 bits of the accumulator, and copy into mac in little endian format
    if(a.MSD > 3){
        a.MSD = 3;
    }
    uint8_t mac_be[16];
    CHECK_BIGINT_OKAY(bigint_to_bytes(&a, mac_be, POLY1305_MAC_LEC_BYTES, 0));
    reverse_array(mac_be, mac, POLY1305_MAC_LEC_BYTES);
    
    bigint_free(&r);
    bigint_free(&s);
    bigint_free(&a);
    bigint_free(&p);
    bigint_free(&n);

    return CRYPT_OKAY;
}
