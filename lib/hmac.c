#include "hmac.h"
#include "sha256.h"
#include "sha512.h"
#include <stdlib.h>

// static void pad_key_CT(uint8_t *key, size_t key_len, size_t block_size) {
//     int8_t mask;
//     uint8_t pad_byte = 0x00;  // Pad byte for HMAC key padding

//     for (size_t i = 0; i < block_size; i++) {
//         // Create a mask that is 0xFF if i is greater than or equal to key_len, otherwise 0x00
//         mask = ~((i >= key_len) - 1);
//         key[i] = (key[i] & ~mask) | (pad_byte & mask);
//     }
// }

void hmac_init( hmac_ctx *ctx,
                        const uint8_t *key, unsigned int key_len, 
                        unsigned int block_size)
{   
    memset(ctx->padded_key, 0, HMAC_PADDED_KEY_LEN);
    memset(ctx->inner_pad, 0, HMAC_PADDED_KEY_LEN);
    memset(ctx->outer_pad, 0, HMAC_PADDED_KEY_LEN);
    
    memcpy(ctx->padded_key, key, key_len);
    // memset and memcpy will work fine, but does memcpy compiles to branchless instr??
    // pad_key_CT(ctx->padded_key, key_len, block_size);

    memset(ctx->inner_pad, HMAC_IPAD_BYTE, block_size);
    memset(ctx->outer_pad, HMAC_OPAD_BYTE, block_size);
}

// hmac is computed according to the following formula
// mac = H((K0 XOR outer_pad) || H((K0 XOR inner_pad) || text ))
void hmac_sha256_compute_mac(hmac_ctx *ctx, 
                                const uint8_t *text, unsigned int text_len,
                                uint8_t *mac, unsigned int mac_len)

{
    uint8_t inner_hash[SHA256_DIGEST_LEN_BYTES];
    uint8_t xor_res[SHA2_MSG_BLOCK_LEN_BYTES];

    uint8_t *inner_hash_data = (uint8_t *)malloc(SHA2_MSG_BLOCK_LEN_BYTES + text_len);

    // computing K0 XOR outer_pad and K0 XOR inner_pad within a single loop
    for(size_t i = 0; i < SHA2_MSG_BLOCK_LEN_BYTES; i++){
        xor_res[i] = ctx->padded_key[i] ^ ctx->outer_pad[i]; 
        inner_hash_data[i] = ctx->padded_key[i] ^ ctx->inner_pad[i];
    }

    // compute the inner hash first 
    memcpy(inner_hash_data + SHA2_MSG_BLOCK_LEN_BYTES, text, text_len);
    sha224_256_ctx sha256_ctx;
    sha224_256_init(&sha256_ctx, SHA256);
    sha224_256_update(&sha256_ctx, inner_hash_data, SHA2_MSG_BLOCK_LEN_BYTES + text_len);
    sha224_256_finish(&sha256_ctx, inner_hash, SHA256_DIGEST_LEN_BYTES, SHA256);

    free(inner_hash_data);

    // concantenate (k0 XOR outer_pad) and inner hash, and compute mac
    uint8_t hmac_data[SHA2_MSG_BLOCK_LEN_BYTES + SHA256_DIGEST_LEN_BYTES];

    memcpy(hmac_data, xor_res, SHA2_MSG_BLOCK_LEN_BYTES);
    memcpy(hmac_data + SHA2_MSG_BLOCK_LEN_BYTES, inner_hash, SHA256_DIGEST_LEN_BYTES);

    sha224_256_init(&sha256_ctx, SHA256);
    sha224_256_update(&sha256_ctx, hmac_data, SHA2_MSG_BLOCK_LEN_BYTES + SHA256_DIGEST_LEN_BYTES);
    sha224_256_finish(&sha256_ctx, mac, mac_len, SHA256);
}

void hmac_sha384_compute_mac(hmac_ctx *ctx, 
                                const uint8_t *text, unsigned int text_len,
                                uint8_t *mac, unsigned int mac_len)

{
    uint8_t inner_hash[SHA384_DIGEST_LEN_BYTES];
    uint8_t xor_res[SHA512_MSG_BLOCK_LEN_BYTES];

    uint8_t *inner_hash_data = (uint8_t *)malloc(SHA512_MSG_BLOCK_LEN_BYTES + text_len);
    for(size_t i = 0; i < SHA512_MSG_BLOCK_LEN_BYTES; i++){
        xor_res[i] = ctx->padded_key[i] ^ ctx->outer_pad[i]; 
        inner_hash_data[i] = ctx->padded_key[i] ^ ctx->inner_pad[i];
    }

    memcpy(inner_hash_data + SHA512_MSG_BLOCK_LEN_BYTES, text, text_len);
    sha384_512_ctx sha384_ctx;
    sha384_512_init(&sha384_ctx, SHA384);
    sha384_512_update(&sha384_ctx, inner_hash_data, SHA512_MSG_BLOCK_LEN_BYTES + text_len);
    sha384_512_finish(&sha384_ctx, inner_hash, SHA384_DIGEST_LEN_BYTES, SHA384);
    free(inner_hash_data);

    uint8_t hmac_data[SHA512_MSG_BLOCK_LEN_BYTES + SHA384_DIGEST_LEN_BYTES];
    memcpy(hmac_data, xor_res, SHA512_MSG_BLOCK_LEN_BYTES);
    memcpy(hmac_data + SHA512_MSG_BLOCK_LEN_BYTES, inner_hash, SHA384_DIGEST_LEN_BYTES);

    sha384_512_init(&sha384_ctx, SHA384);
    sha384_512_update(&sha384_ctx, hmac_data, SHA512_MSG_BLOCK_LEN_BYTES + SHA384_DIGEST_LEN_BYTES);
    sha384_512_finish(&sha384_ctx, mac, mac_len, SHA384);

}

void hmac_sha512_compute_mac(hmac_ctx *ctx, 
                                const uint8_t *text, unsigned int text_len,
                                uint8_t *mac, unsigned int mac_len)

{
    uint8_t inner_hash[SHA512_DIGEST_LEN_BYTES];
    uint8_t xor_res[SHA512_MSG_BLOCK_LEN_BYTES];

    uint8_t *inner_hash_data = (uint8_t *)malloc(SHA512_MSG_BLOCK_LEN_BYTES + text_len);
    for(size_t i = 0; i < SHA512_MSG_BLOCK_LEN_BYTES; i++){
        xor_res[i] = ctx->padded_key[i] ^ ctx->outer_pad[i]; 
        inner_hash_data[i] = ctx->padded_key[i] ^ ctx->inner_pad[i];
    }

    memcpy(inner_hash_data + SHA512_MSG_BLOCK_LEN_BYTES, text, text_len);
    sha384_512_ctx sha512_ctx;
    sha384_512_init(&sha512_ctx, SHA512);
    sha384_512_update(&sha512_ctx, inner_hash_data, SHA512_MSG_BLOCK_LEN_BYTES + text_len);
    sha384_512_finish(&sha512_ctx, inner_hash, SHA512_DIGEST_LEN_BYTES, SHA512);
    free(inner_hash_data);

    uint8_t hmac_data[SHA512_MSG_BLOCK_LEN_BYTES + SHA512_DIGEST_LEN_BYTES];
    memcpy(hmac_data, xor_res, SHA512_MSG_BLOCK_LEN_BYTES);
    memcpy(hmac_data + SHA512_MSG_BLOCK_LEN_BYTES, inner_hash, SHA512_DIGEST_LEN_BYTES);

    sha384_512_init(&sha512_ctx, SHA512);
    sha384_512_update(&sha512_ctx, hmac_data, sizeof(hmac_data));
    sha384_512_finish(&sha512_ctx, mac, mac_len, SHA512);

}

void hmac_cleanup(hmac_ctx *ctx){
    memset_s(ctx->inner_pad, 0, sizeof(ctx->inner_pad));
    memset_s(ctx->outer_pad, 0, sizeof(ctx->outer_pad));
    memset_s(ctx->padded_key, 0, sizeof(ctx->padded_key));
}