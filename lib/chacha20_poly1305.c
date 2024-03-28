#include "chacha20_poly1305.h"
#include <string.h>
#include <stdlib.h>

#define CHACHA20_INIT_COUNTER 1

crypt_status chacha20_poly1305_init(chacha20_poly1305_ctx *ctx, unsigned char *key, unsigned int keylen)
{
    if(!ctx) return CRYPT_NULL_PTR;
    if(keylen != CHACHA20_KEY_LEN_BYTES){
        return CRYPT_BAD_KEY;
    }
    memcpy(ctx->key, key, keylen);
    CRYPT_CHECK_OKAY(chacha20_init(ctx->chacha_ctx, key));

    return CRYPT_OKAY;

}

crypt_status chacha20_poly1305_encrypt(chacha20_poly1305_ctx *ctx, unsigned char *plaintext, unsigned int plaintext_len,
                                        unsigned char *ciphertext, unsigned int ciphertext_len, uint32_t constant, 
                                        const unsigned char *iv, unsigned int iv_len,
                                        const unsigned char *aad, unsigned int aad_len, 
                                        unsigned char *aead_output, unsigned int aead_output_len)
{   
    if(!aad || !aead_output) return CRYPT_NULL_PTR; 

    // preliminary checks
    if(iv_len + sizeof(uint32_t) != CHACHA20_NONCE_LEN_BYTES){
        return CRYPT_BAD_NONCE;
    }

    if(aead_output_len != ciphertext_len + POLY1305_MAC_LEN_BYTES){
        return CRYPT_BAD_BUFFER_LEN;
    }
    
    // mac will be computed based on the string that is concatenated from the following
    // aad + padding1 + ciphertext + padding2 + aad_len (64-bit value) + ciphertext_len (64-bit value)
    // 16 bytes at the end for aad_len and ciphertext_len
    unsigned int msg_len = aad_len + (aad_len & 0x0F) + ciphertext_len + (ciphertext_len & 0x0F) + 16;
    unsigned char *message = (unsigned char *)malloc(msg_len);
    crypt_status res = CRYPT_OKAY;

    // MAC will be computed on the message that is the concatenation of
    // aad + padding1 + ciphertexr + padding2 + aad_len as 64 bit value + ciphertext_len as 64 bit value
    unsigned char mac[POLY1305_MAC_LEN_BYTES];
    unsigned char polykey[POLY1305_KEY_LEN_BYTES];

    // forming a nonce 
    unsigned char nonce[CHACHA20_NONCE_LEN_BYTES];
    memcpy(nonce, &constant, sizeof(uint32_t));
    memcpy(&nonce[sizeof(uint32_t)], iv, iv_len);

    // generating a poly1305 key 
    poly1305_keygen(ctx->key, nonce, polykey);
    CRYPT_CHECK_OKAY_CL(poly1305_init(ctx->poly_ctx, polykey, sizeof(polykey)));

    int current_len = 0;

    // omitting chacha20 parameter validations as chacha20_crypt will validate them
    CRYPT_CHECK_OKAY_CL(chacha20_crypt(ctx->chacha_ctx, CHACHA20_INIT_COUNTER, nonce, sizeof(nonce),
                                    plaintext, plaintext_len, ciphertext, ciphertext_len));

    // prepending aad to the final output
    memcpy(message, aad, aad_len);
    current_len += aad_len;

    // the padding is up to 15 zero bytes, and it should bring the total length so far to
    // and integral multiple of 16  
    int padding_len = current_len & 0x0F;
    memset(&message[current_len], 0, padding_len);
    current_len += padding_len;

    // appending ciphertext
    memcpy(&message[current_len], ciphertext, ciphertext_len);
    current_len += ciphertext_len;

    // appending padding 2
    padding_len = current_len & 0x0F;
    memset(&message[current_len], 0, padding_len);
    current_len += padding_len;

    // appending aad length and ciphertext length as 64 bit value 
    uint64_t aad_len_64 = (uint64_t)aad_len;
    uint64_t ciphertext_len_64 = (uint64_t)ciphertext_len;
    memcpy(&message[current_len], &aad_len_64, sizeof(uint64_t));
    current_len += sizeof(uint64_t);
    memcpy(&message[current_len], &ciphertext_len_64, sizeof(uint64_t));

    // compute mac
    CRYPT_CHECK_OKAY_CL(poly1305_compute_mac(ctx->poly_ctx, message, msg_len, mac, POLY1305_MAC_LEN_BYTES));

    // concatenate ciphertext and mac to form the final output
    memcpy(aead_output, ciphertext, ciphertext_len);
    memcpy(&aead_output[ciphertext_len], mac, POLY1305_MAC_LEN_BYTES);

cleanup: 
    if(message){
        free(message);
    }

    return res;
}

