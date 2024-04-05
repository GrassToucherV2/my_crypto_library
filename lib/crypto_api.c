#include "crypto_api.h"

#include "md5.h"
#include "sha1.h"
#include "sha256.h"
#include "sha512.h"

#include "chacha20.h"
#include "poly1305.h"
#include "chacha20_poly1305.h"

#include <stdio.h>

crypt_status crypt_test(){
    printf("test function\n");
    return CRYPT_OKAY;
}

/////////////////////////////////// API for hashing algorithms ///////////////////////////////////

crypt_status crypt_md5(const unsigned char *input,
                 unsigned int input_len, unsigned char *output,
                 unsigned int output_len
)
{
    if(!input || !output) return CRYPT_NULL_PTR;

    if((output_len) < MD5_DIGESTS_LEN_BYTES){
        return CRYPT_BAD_BUFFER_LEN;
    }

    md5_ctx ctx = {0};

    CRYPT_CHECK_OKAY(md5_init(&ctx));
    CRYPT_CHECK_OKAY(md5_update(&ctx, input, input_len));
    CRYPT_CHECK_OKAY(md5_finish(&ctx, output));
    return CRYPT_OKAY;
}

crypt_status crypt_sha1(const unsigned char *input,
                 unsigned int input_len, unsigned char *output,
                 unsigned int output_len
)
{
    if(!input || !output) return CRYPT_NULL_PTR;

    if(output_len < SHA1_DIGEST_LEN_BYTES){
        return CRYPT_BAD_BUFFER_LEN;
    }    

    sha1_ctx ctx = {0};

    CRYPT_CHECK_OKAY(sha1_init(&ctx));
    CRYPT_CHECK_OKAY(sha1_update(&ctx, input, input_len));
    CRYPT_CHECK_OKAY(sha1_finish(&ctx, output, output_len));

    return CRYPT_OKAY;
}

crypt_status crypt_sha256(const unsigned char *input,
                 unsigned int input_len, unsigned char *output,
                 unsigned int output_len, SHA2 sha
){
    if(!input || !output) return CRYPT_NULL_PTR;

    switch (sha){
        case SHA224:
            if(output_len < SHA224_DIGEST_LEN_BYTES){
                return CRYPT_BAD_BUFFER_LEN;
            } 
            sha224_256_ctx ctx_224 = {0};
            CRYPT_CHECK_OKAY(sha224_256_init(&ctx_224, SHA224));
            CRYPT_CHECK_OKAY(sha224_256_update(&ctx_224, input, input_len));
            CRYPT_CHECK_OKAY(sha224_256_finish(&ctx_224, output, output_len, SHA224));
            break;
        case SHA256:
            if(output_len < SHA256_DIGEST_LEN_BYTES){
                return CRYPT_BAD_BUFFER_LEN;
            } 
            sha224_256_ctx ctx_256 = {0};
            CRYPT_CHECK_OKAY(sha224_256_init(&ctx_256, SHA256));
            CRYPT_CHECK_OKAY(sha224_256_update(&ctx_256, input, input_len));
            CRYPT_CHECK_OKAY(sha224_256_finish(&ctx_256, output, output_len, SHA256));
            break;
        default:
            return CRYPT_UNSUPPORTED_ALGO;
    }

    return CRYPT_OKAY;

}

crypt_status crypt_sha512(const unsigned char *input,
                 unsigned int input_len, unsigned char *output,
                 unsigned int output_len, SHA2 sha
){
    if(!input || !output) return CRYPT_NULL_PTR;

    switch (sha){
        case SHA512:
            if(output_len < SHA512_DIGEST_LEN_BYTES){
                return CRYPT_BAD_BUFFER_LEN;
            } 
            sha384_512_ctx ctx_512 = {0};
            CRYPT_CHECK_OKAY(sha384_512_init(&ctx_512, SHA512));
            CRYPT_CHECK_OKAY(sha384_512_update(&ctx_512, input, input_len));
            CRYPT_CHECK_OKAY(sha384_512_finish(&ctx_512, output, output_len, SHA512));
            break;
        case SHA384:
            if(output_len < SHA384_DIGEST_LEN_BYTES){
                return CRYPT_BAD_BUFFER_LEN;
            } 
            sha384_512_ctx ctx_384 = {0};
            CRYPT_CHECK_OKAY(sha384_512_init(&ctx_384, SHA384));
            CRYPT_CHECK_OKAY(sha384_512_update(&ctx_384, input, input_len));
            CRYPT_CHECK_OKAY(sha384_512_finish(&ctx_384, output, output_len, SHA384));
            break;
        case SHA512_256:
            if(output_len < SHA256_DIGEST_LEN_BYTES){
                return CRYPT_BAD_BUFFER_LEN;
            } 
            sha384_512_ctx ctx_256 = {0};
            CRYPT_CHECK_OKAY(sha384_512_init(&ctx_256, SHA512_256));
            CRYPT_CHECK_OKAY(sha384_512_update(&ctx_256, input, input_len));
            CRYPT_CHECK_OKAY(sha384_512_finish(&ctx_256, output, output_len, SHA512_256));
            break;
        case SHA512_224:
            if(output_len < SHA224_DIGEST_LEN_BYTES){
                return CRYPT_BAD_BUFFER_LEN;
            } 
            sha384_512_ctx ctx_224 = {0};
            CRYPT_CHECK_OKAY(sha384_512_init(&ctx_224, SHA512_224));
            CRYPT_CHECK_OKAY(sha384_512_update(&ctx_224, input, input_len));
            CRYPT_CHECK_OKAY(sha384_512_finish(&ctx_224, output, output_len, SHA512_224));
            break;
        default:
            return CRYPT_UNSUPPORTED_ALGO;
    }

    return CRYPT_OKAY;
}

/////////////////////////////////// API for symmetric key algorithms ///////////////////////////////////

crypt_status crypt_chacha20_encrypt(const unsigned char *plaintext, unsigned int plaintext_len,
                                    const unsigned char *key, unsigned int key_len,
                                    const unsigned char *nonce, unsigned int nonce_len,
                                    unsigned int counter, unsigned char *ciphertext,
                                    unsigned int ciphertext_len)
{

    if(!plaintext || !key || !nonce || !ciphertext) return CRYPT_NULL_PTR;

    if(key_len != CHACHA20_KEY_LEN_BYTES) return CRYPT_BAD_KEY;

    if(nonce_len != CHACHA20_NONCE_LEN_BYTES) return CRYPT_BAD_NONCE;

    if(ciphertext_len < plaintext_len) return CRYPT_BAD_BUFFER_LEN;

    chacha20_ctx ctx = {0};
    CRYPT_CHECK_OKAY(chacha20_init(&ctx, key));
    CRYPT_CHECK_OKAY(chacha20_crypt(&ctx, counter, nonce, nonce_len, plaintext, plaintext_len, ciphertext, ciphertext_len));
    CRYPT_CHECK_OKAY(chacha20_cleanup(&ctx));

    return CRYPT_OKAY;
}

crypt_status crypt_chacha20_decrypt(const unsigned char *ciphertext, unsigned int ciphertext_len,
                                    const unsigned char *key, unsigned int key_len,
                                    const unsigned char *nonce, unsigned int nonce_len,
                                    unsigned int counter, unsigned char *plaintext,
                                    unsigned int plaintext_len)
{
    if(!plaintext || !key || !nonce || !ciphertext) return CRYPT_NULL_PTR;

    if(key_len != CHACHA20_KEY_LEN_BYTES) return CRYPT_BAD_KEY;

    if(nonce_len != CHACHA20_NONCE_LEN_BYTES) return CRYPT_BAD_NONCE;

    if(plaintext_len < ciphertext_len) return CRYPT_BAD_BUFFER_LEN;

    chacha20_ctx ctx = {0};
    CRYPT_CHECK_OKAY(chacha20_init(&ctx, key));
    CRYPT_CHECK_OKAY(chacha20_crypt(&ctx, counter, nonce, nonce_len, ciphertext, ciphertext_len, plaintext, plaintext_len));
    CRYPT_CHECK_OKAY(chacha20_cleanup(&ctx));

    return CRYPT_OKAY;
}

// crypt_status crypt_poly1305(const unsigned char *input, unsigned int input_len, 
//                             const unsigned char *key, unsigned int key_len, 
//                             const unsigned char *nonce, unsigned int nonce_len,
//                             unsigned char *mac, unsigned int mac_len)
// {
//     if(!input || !key || !nonce || !mac) return CRYPT_NULL_PTR;

//     if(key_len != CHACHA20_KEY_LEN_BYTES) return CRYPT_BAD_KEY;

//     if(nonce_len != CHACHA20_NONCE_LEN_BYTES) return CRYPT_BAD_NONCE;

//     if(mac_len < POLY1305_MAC_LEN_BYTES) return CRYPT_BAD_BUFFER_LEN;

//     poly1305_ctx ctx = {0};
//     CRYPT_CHECK_OKAY(poly1305_init(&ctx, key, key_len, nonce, nonce_len));
//     CRYPT_CHECK_OKAY(poly1305_compute_mac(&ctx, input, input_len, mac, mac_len));

//     return CRYPT_OKAY;
// }

crypt_status crypt_chacha20_poly1305_encrypt(const unsigned char *iv, unsigned int iv_len,
                                            const unsigned char *aad, unsigned int aad_len,
                                            const unsigned char *key, unsigned int key_len,
                                            const unsigned char *constant, unsigned int constant_len,
                                            const unsigned char *plaintext, unsigned int plaintext_len,
                                            unsigned char *ciphertext, unsigned int ciphertext_len,
                                            unsigned char *aead_output, unsigned int aead_output_len
                                            )
{
    if(!iv || !aad || !key || !plaintext || !ciphertext || !aead_output) return CRYPT_NULL_PTR;

    if(iv_len != CHACHA20_POLY1305_IV_LEN_BYTES){
        return CRYPT_BAD_IV;
    }

    if(key_len != CHACHA20_KEY_LEN_BYTES){
        return CRYPT_BAD_KEY;
    }

    if(ciphertext_len < plaintext_len){
        return CRYPT_BAD_BUFFER_LEN;
    }

    if(aead_output_len < plaintext_len + POLY1305_MAC_LEN_BYTES){
        return CRYPT_BAD_BUFFER_LEN;
    }
    chacha20_poly1305_ctx ctx;
    CRYPT_CHECK_OKAY(chacha20_poly1305_init(&ctx, key, key_len));
    CRYPT_CHECK_OKAY(chacha20_poly1305_encrypt(&ctx, plaintext, plaintext_len, ciphertext,
                                                ciphertext_len, constant, constant_len, iv, iv_len, aad, 
                                                aad_len, aead_output, aead_output_len));
    CRYPT_CHECK_OKAY(chacha20_poly1305_cleanup(&ctx));

    return CRYPT_OKAY;
}

crypt_status crypt_chacha20_poly1305_decrypt(const unsigned char *iv, unsigned int iv_len,
                                            const unsigned char *aad, unsigned int aad_len,
                                            const unsigned char *key, unsigned int key_len,
                                            const unsigned char *constant, unsigned int constant_len,                                            
                                            unsigned char *aead_input, unsigned int aead_input_len,
                                            unsigned char *plaintext, unsigned int plaintext_len)
{
    if(!iv || !aad || !key || !plaintext || !aead_input) return CRYPT_NULL_PTR;

    if(iv_len != CHACHA20_POLY1305_IV_LEN_BYTES){
        return CRYPT_BAD_IV;
    }

    if(key_len != CHACHA20_KEY_LEN_BYTES){
        return CRYPT_BAD_KEY;
    }

    if(plaintext_len < aead_input_len - POLY1305_MAC_LEN_BYTES){
        return CRYPT_BAD_BUFFER_LEN;
    }
    chacha20_poly1305_ctx ctx;
    CRYPT_CHECK_OKAY(chacha20_poly1305_init(&ctx, key, key_len));
    CRYPT_CHECK_OKAY(chacha20_poly1305_decrypt(&ctx, aead_input, aead_input_len, constant, constant_len,
                                                iv, iv_len, aad, aad_len, plaintext, plaintext_len));
    CRYPT_CHECK_OKAY(chacha20_poly1305_cleanup(&ctx));

    return CRYPT_OKAY;
}