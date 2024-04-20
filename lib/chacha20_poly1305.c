#include "chacha20_poly1305.h"
#include <string.h>
#include <stdlib.h>
#include "../util/tools.h"

#define CHACHA20_INIT_COUNTER 1

crypt_status chacha20_poly1305_init(chacha20_poly1305_ctx *ctx, const unsigned char *key, unsigned int keylen)
{
    if(!ctx) return CRYPT_NULL_PTR;
    if(keylen != CHACHA20_KEY_LEN_BYTES){
        return CRYPT_BAD_KEY;
    }
    memcpy(ctx->key, key, keylen);
    CRYPT_CHECK_OKAY(chacha20_init(&ctx->chacha_ctx, key));

    return CRYPT_OKAY;

}

static void form_mac_data(const unsigned char *aad, unsigned int aad_len,
                        const unsigned char *ciphertext, unsigned int ciphertext_len,
                        unsigned char *mac_data)
{   
    int current_len = 0;
    // prepending aad to the final output
    memcpy(mac_data, aad, aad_len);
    current_len += aad_len;

    // the padding is up to 15 zero bytes, and it should bring the total length so far to
    // and integral multiple of 16  
    int padding_len = 16 - (current_len & 0x0F);
    if(padding_len == 0) padding_len = 0;
    
    memset_s(&mac_data[current_len], 0, padding_len);
    current_len += padding_len;

    // appending ciphertext
    memcpy(&mac_data[current_len], ciphertext, ciphertext_len);
    current_len += ciphertext_len;

    // appending padding 2
    padding_len = 16 - (current_len & 0x0F);
    if(padding_len == 16) padding_len = 0;
    memset_s(&mac_data[current_len], 0, padding_len);
    current_len += padding_len;

    // appending aad length and ciphertext length as 64 bit value 
    uint64_t aad_len_64 = (uint64_t)aad_len;
    uint64_t ciphertext_len_64 = (uint64_t)ciphertext_len;
    memcpy(&mac_data[current_len], &aad_len_64, sizeof(uint64_t));
    current_len += sizeof(uint64_t);
    memcpy(&mac_data[current_len], &ciphertext_len_64, sizeof(uint64_t));
}

static crypt_status compute_mac(chacha20_poly1305_ctx *ctx, const unsigned char *mac_data, unsigned int mac_data_len,
                                const unsigned char *nonce, unsigned char *mac)
{    
    // generating a poly1305 key 
    unsigned char polykey[POLY1305_KEY_LEN_BYTES];
    poly1305_keygen(ctx->key, nonce, polykey);
    CRYPT_CHECK_OKAY(poly1305_init(&ctx->poly_ctx, polykey, sizeof(polykey)));
    CRYPT_CHECK_OKAY(poly1305_compute_mac(&ctx->poly_ctx, mac_data, mac_data_len, 
                        mac, POLY1305_MAC_LEN_BYTES));
    
    // wipe sensitive data
    memset_s(polykey, 0, sizeof(polykey));
    
    return CRYPT_OKAY;
}

crypt_status chacha20_poly1305_encrypt(chacha20_poly1305_ctx *ctx, 
                                        const unsigned char *plaintext, unsigned int plaintext_len,
                                        unsigned char *ciphertext, unsigned int ciphertext_len, 
                                        const unsigned char *constant, unsigned int constant_len, 
                                        const unsigned char *iv, unsigned int iv_len,
                                        const unsigned char *aad, unsigned int aad_len, 
                                        unsigned char *aead_output, unsigned int aead_output_len)
{   
    if(!ctx || !aad || !iv || !aead_output) return CRYPT_NULL_PTR; 

    // preliminary checks
    if(iv_len + constant_len != CHACHA20_NONCE_LEN_BYTES){
        return CRYPT_BAD_NONCE;
    }

    if(aead_output_len != ciphertext_len + POLY1305_MAC_LEN_BYTES){
        return CRYPT_BAD_BUFFER_LEN;
    }
    
    // mac will be computed based on the string that is concatenated from the following
    // aad + padding1 + ciphertext + padding2 + aad_len (64-bit value) + ciphertext_len (64-bit value)
    // 16 bytes at the end for aad_len and ciphertext_len
    unsigned int mac_data_len = aad_len + (16 - (aad_len & 0x0F)) + ciphertext_len + (16 - (ciphertext_len & 0x0F)) + 16;

    unsigned char *mac_data = (unsigned char *)malloc(mac_data_len);
    crypt_status res = CRYPT_OKAY;

    // forming a nonce 
    unsigned char nonce[CHACHA20_NONCE_LEN_BYTES];
    memcpy(nonce, constant, constant_len);
    memcpy(&nonce[sizeof(uint32_t)], iv, iv_len);

    // omitting chacha20 parameter validations as chacha20_crypt will validate them
    CRYPT_CHECK_OKAY_CL(chacha20_crypt(&ctx->chacha_ctx, CHACHA20_INIT_COUNTER, nonce, sizeof(nonce),
                                    plaintext, plaintext_len, ciphertext, ciphertext_len));

    // MAC will be computed on the mac data that is the concatenation of
    // aad + padding1 + ciphertexr + padding2 + aad_len as 64 bit value + ciphertext_len as 64 bit value
    form_mac_data(aad, aad_len, ciphertext, ciphertext_len, mac_data);

    // compute mac
    unsigned char mac[POLY1305_MAC_LEN_BYTES];
    CRYPT_CHECK_OKAY_CL(compute_mac(ctx, mac_data, mac_data_len, nonce, mac));

    // concatenate ciphertext and mac to form the final output
    memcpy(aead_output, ciphertext, ciphertext_len);
    memcpy(&aead_output[ciphertext_len], mac, POLY1305_MAC_LEN_BYTES);

cleanup: 
    if(mac_data){
        free(mac_data);
    }

    return res;
}

// if a = 0, then both macs are equal
static int compare_mac(unsigned char *mac1, unsigned char *mac2){
    int a = 0;
    for(int i = 0; i < POLY1305_MAC_LEN_BYTES; i++){
        a |= mac1[i] ^ mac2[i];
    }

    return a;
}

crypt_status chacha20_poly1305_decrypt(chacha20_poly1305_ctx *ctx, 
                                        const unsigned char *encrypted_text, unsigned int encrypted_text_len,
                                        const unsigned char *constant, unsigned int constant_len, 
                                        const unsigned char *iv, unsigned int iv_len,
                                        const unsigned char *aad, unsigned int aad_len, 
                                        unsigned char *plaintext, unsigned int plaintext_len)
{
    if(!ctx || !aad || !iv) return CRYPT_NULL_PTR;

    // preliminary checks
    if(iv_len + constant_len != CHACHA20_NONCE_LEN_BYTES){
        return CRYPT_BAD_NONCE;
    }

    if(plaintext_len < encrypted_text_len - POLY1305_MAC_LEN_BYTES){
        return CRYPT_BAD_BUFFER_LEN;
    }

    crypt_status res = CRYPT_OKAY;

    // extracting mac and ciphertext from the encrypted_text
    int ciphertext_len = encrypted_text_len - POLY1305_MAC_LEN_BYTES;
    unsigned int mac_data_len = aad_len + (16 - (aad_len % 16)) + 
                             ciphertext_len + (16 - (ciphertext_len % 16)) + 
                             16;  // For two 64-bit length fields

    unsigned char extracted_mac[POLY1305_MAC_LEN_BYTES];
    unsigned char computed_mac[POLY1305_MAC_LEN_BYTES];
    
    memcpy(extracted_mac, &encrypted_text[ciphertext_len], POLY1305_MAC_LEN_BYTES);

    unsigned char *ciphertext = (unsigned char *)malloc(ciphertext_len);
    memcpy(ciphertext, encrypted_text, ciphertext_len);
    unsigned char *mac_data = (unsigned char *)malloc(mac_data_len);
    form_mac_data(aad, aad_len, ciphertext, ciphertext_len, mac_data);

    // compute mac and verify encrypted_text integrity
    unsigned char nonce[CHACHA20_NONCE_LEN_BYTES];
    memcpy(nonce, constant, sizeof(uint32_t));
    memcpy(&nonce[sizeof(uint32_t)], iv, iv_len);

    CRYPT_CHECK_OKAY_CL(compute_mac(ctx, mac_data, mac_data_len, nonce, computed_mac));
    
    if(compare_mac(extracted_mac, computed_mac)){
        res = CRYPT_INVALID_TEXT;
        goto cleanup;
    }

    // if mac is correct, decrypt the text and return 
    CRYPT_CHECK_OKAY_CL(chacha20_crypt(&ctx->chacha_ctx, CHACHA20_INIT_COUNTER, nonce, sizeof(nonce), 
                        ciphertext, ciphertext_len, plaintext, plaintext_len));

cleanup:
    if(ciphertext){
        free(ciphertext);
    }

    if(mac_data){
        free(mac_data);
    }

   return res; 

}

crypt_status chacha20_poly1305_cleanup(chacha20_poly1305_ctx *ctx){
    if(!ctx) return CRYPT_NULL_PTR;

    memset_s(ctx, 0, sizeof(*ctx));

    return CRYPT_OKAY;
}

