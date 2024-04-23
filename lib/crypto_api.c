#include "crypto_api.h"

#include "md5.h"
#include "sha1.h"
#include "sha256.h"
#include "sha512.h"

#include "aes.h"
#include "chacha20.h"
#include "chacha20_poly1305.h"
#include "des.h"
#include "poly1305.h"


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

crypt_status crypt_poly1305(const unsigned char *input, unsigned int input_len, 
                            const unsigned char *key, unsigned int key_len, 
                            unsigned char *mac, unsigned int mac_len)
{
    if(!input || !key || !mac) return CRYPT_NULL_PTR;

    if(key_len != CHACHA20_KEY_LEN_BYTES) return CRYPT_BAD_KEY;

    if(mac_len < POLY1305_MAC_LEN_BYTES) return CRYPT_BAD_BUFFER_LEN;

    poly1305_ctx ctx = {0};
    // Note: poly1305_init takes in the generated poly1305 key, ready to be used directly
    CRYPT_CHECK_OKAY(poly1305_init(&ctx, key, key_len));
    CRYPT_CHECK_OKAY(poly1305_compute_mac(&ctx, input, input_len, mac, mac_len));

    return CRYPT_OKAY;
}

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

crypt_status crypt_DES_encrypt(uint64_t key, uint64_t iv,
                                const unsigned char *plaintext, unsigned int plaintext_len,
                                unsigned char *ciphertext, unsigned int ciphertext_len,
                                padding_scheme padding, block_cipher_mode mode)
{
    if(!plaintext || !ciphertext) return CRYPT_NULL_PTR;

    des_ctx ctx = {0};
    switch(mode){
        case ECB:
            // if no padding is selected, then we require the plaintext be full blocks only
            if(padding == NO_PAD){
                if(plaintext_len % DES_BLOCK_SIZE_BYTES != 0){
                    return CRYPT_INVALID_PADDING;
                }
            }

            if(padding != NO_PAD && padding != PKCS7){
                return CRYPT_INVALID_PADDING;
            }
            CRYPT_CHECK_OKAY(DES_init(&ctx, key));
            CRYPT_CHECK_OKAY(DES_encrypt_ECB(&ctx, plaintext, plaintext_len, ciphertext, ciphertext_len, padding));
            CRYPT_CHECK_OKAY(DES_cleanup(&ctx));
            break;
        
        case CBC:
            CRYPT_CHECK_OKAY(DES_init(&ctx, key));
            CRYPT_CHECK_OKAY(DES_encrypt_CBC(&ctx, plaintext, plaintext_len, iv, ciphertext, ciphertext_len));
            CRYPT_CHECK_OKAY(DES_cleanup(&ctx));
            break;
        
        case CTR:
            printf("not implemented\n");
            break;
        
        default:
            printf("mode not supported\n");
            break;
    }
    
    return CRYPT_OKAY;
}

crypt_status crypt_DES_decrypt(uint64_t key, uint64_t iv,
                                    const unsigned char *ciphertext, unsigned int ciphertext_len,
                                    unsigned char *plaintext, unsigned int plaintext_len,
                                    padding_scheme padding, block_cipher_mode mode)
{
    if(!plaintext || !ciphertext) return CRYPT_NULL_PTR;

    des_ctx ctx = {0};
    switch(mode){
        case ECB:
            // if no padding is selected, then we require the plaintext be full blocks only
            if(padding == NO_PAD){
                if(ciphertext_len % DES_BLOCK_SIZE_BYTES != 0){
                    return CRYPT_INVALID_PADDING;
                }
            }

            if(padding != NO_PAD && padding != PKCS7){
                return CRYPT_INVALID_PADDING;
            }
            CRYPT_CHECK_OKAY(DES_init(&ctx, key));
            CRYPT_CHECK_OKAY(DES_decrypt_ECB(&ctx, ciphertext, ciphertext_len, plaintext, plaintext_len, padding));
            CRYPT_CHECK_OKAY(DES_cleanup(&ctx));
            break;
        
        case CBC:
            CRYPT_CHECK_OKAY(DES_init(&ctx, key));
            CRYPT_CHECK_OKAY(DES_decrypt_CBC(&ctx, ciphertext, ciphertext_len, iv, plaintext, plaintext_len));
            CRYPT_CHECK_OKAY(DES_cleanup(&ctx));
            break;
        
        case CTR:
            printf("not implemented\n");
            break;
        
        default:
            printf("mode not supported\n");
            break;
    }
    
    return CRYPT_OKAY;
}

crypt_status crypt_TDES_encrypt(uint64_t key1, uint64_t key2, uint64_t key3, uint64_t iv,
                                    const unsigned char *plaintext, unsigned int plaintext_len,
                                    unsigned char *ciphertext, unsigned int ciphertext_len,
                                    padding_scheme padding, block_cipher_mode mode)
{
    if(!plaintext || !ciphertext) return CRYPT_NULL_PTR;

    tdes_ctx ctx = {0};
    switch(mode){
        case ECB:
            // if no padding is selected, then we require the plaintext be full blocks only
            if(padding == NO_PAD){
                if(ciphertext_len % DES_BLOCK_SIZE_BYTES != 0){
                    return CRYPT_INVALID_PADDING;
                }
            }

            if(padding != NO_PAD && padding != PKCS7){
                return CRYPT_INVALID_PADDING;
            }
            CRYPT_CHECK_OKAY(TDES_init(&ctx, key1, key2, key3));
            CRYPT_CHECK_OKAY(TDES_encrypt_ECB(&ctx, plaintext, plaintext_len, ciphertext, ciphertext_len, padding));
            CRYPT_CHECK_OKAY(TDES_cleanup(&ctx));
            break;
        
        case CBC:
            CRYPT_CHECK_OKAY(TDES_init(&ctx, key1, key2, key3));
            CRYPT_CHECK_OKAY(TDES_encrypt_CBC(&ctx, plaintext, plaintext_len, iv, ciphertext, ciphertext_len));
            CRYPT_CHECK_OKAY(TDES_cleanup(&ctx));
            break;
        
        case CTR:
            printf("not implemented\n");
            break;
        
        default:
            printf("mode not supported\n");
            break;
    }
    
    return CRYPT_OKAY;
}

crypt_status crypt_TDES_decrypt(uint64_t key1, uint64_t key2, uint64_t key3,  uint64_t iv,
                                    const unsigned char *ciphertext, unsigned int ciphertext_len,
                                    unsigned char *plaintext, unsigned int plaintext_len,
                                    padding_scheme padding, block_cipher_mode mode)
{
    if(!plaintext || !ciphertext) return CRYPT_NULL_PTR;

    tdes_ctx ctx = {0};
    switch(mode){
        case ECB:
            // if no padding is selected, then we require the plaintext be full blocks only
            if(padding == NO_PAD){
                if(ciphertext_len % DES_BLOCK_SIZE_BYTES != 0){
                    return CRYPT_INVALID_PADDING;
                }
            }

            if(padding != NO_PAD && padding != PKCS7){
                return CRYPT_INVALID_PADDING;
            }
            CRYPT_CHECK_OKAY(TDES_init(&ctx, key1, key2, key3));
            CRYPT_CHECK_OKAY(TDES_decrypt_ECB(&ctx, ciphertext, ciphertext_len, plaintext, plaintext_len, padding));
            CRYPT_CHECK_OKAY(TDES_cleanup(&ctx));
            break;
        
        case CBC:
            CRYPT_CHECK_OKAY(TDES_init(&ctx, key1, key2, key3));
            CRYPT_CHECK_OKAY(TDES_decrypt_CBC(&ctx, ciphertext, ciphertext_len, iv, plaintext, plaintext_len));
            CRYPT_CHECK_OKAY(TDES_cleanup(&ctx));
            break;
        
        case CTR:
            printf("not implemented\n");
            break;
        
        default:
            printf("mode not supported\n");
            break;
    }
    
    return CRYPT_OKAY;
}

crypt_status crypt_AES_encrypt(const uint8_t *key, unsigned int key_size, AES_key_length key_len,
                                const uint8_t *iv, unsigned int iv_len,
                                const uint8_t *plaintext, unsigned int plaintext_len,
                                uint8_t *ciphertext, unsigned int ciphertext_len,
                                padding_scheme padding, block_cipher_mode mode)
{
    if(!key || !plaintext || !ciphertext) return CRYPT_NULL_PTR;

    switch (key_len){
        case AES_128:
            if(key_size != AES_128_KEY_SIZE_BYTES)
                return CRYPT_BAD_KEY;
            break;
        case AES_192:
            if(key_size != AES_192_KEY_SIZE_BYTES)
                return CRYPT_BAD_KEY;
            break;
        case AES_256:
            if(key_size != AES_256_KEY_SIZE_BYTES)
                return CRYPT_BAD_KEY;
            break;
        default:
            return CRYPT_AES_BAD_KEY_LEN;
    }

    if (mode == CBC || mode == CTR || mode == GCM) {
        if (!iv || iv_len != AES_BLOCK_SIZE_BYTES) return CRYPT_BAD_IV;
    }

    // if no padding is selected, then we require the plaintext be full blocks only
    if (mode == ECB) {
        if (padding == NO_PAD){
            if(plaintext_len % AES_BLOCK_SIZE_BYTES != 0)
                return CRYPT_BAD_BUFFER_LEN;
            if (ciphertext_len < plaintext_len)
                return CRYPT_BAD_BUFFER_LEN;
        }        
    } else{
        if(ciphertext_len < plaintext_len + (plaintext_len - (plaintext_len % AES_BLOCK_SIZE_BYTES))){
            return CRYPT_BAD_BUFFER_LEN;
        }
    }

    if (padding != NO_PAD && padding != PKCS7) 
        return CRYPT_INVALID_PADDING;

    aes_ctx ctx = {0};

    switch(mode){
        case ECB:
            CRYPT_CHECK_OKAY(AES_init(&ctx, key, key_len));
            CRYPT_CHECK_OKAY(AES_encrypt_ECB(&ctx, plaintext, plaintext_len, ciphertext, ciphertext_len));
            CRYPT_CHECK_OKAY(AES_cleanup(&ctx));
            break;
        
        case CBC:
            printf("CBC mode not implemented yet\n");
            break;
        
        case CTR:
            printf("CTR mode not implemented yet\n");
            break;
        
        case GCM:
            printf("GCM mode not implemented yet\n");
            break;
        
        default:
            printf("mode not supported\n");
            break;
    }
    
    return CRYPT_OKAY;

}

crypt_status crypt_AES_decrypt(const uint8_t *key, unsigned int key_size, AES_key_length key_len,
                                const uint8_t *iv, unsigned int iv_len,
                                const uint8_t *ciphertext, unsigned int ciphertext_len,
                                uint8_t *plaintext, unsigned int plaintext_len,
                                padding_scheme padding, block_cipher_mode mode)
{
    if(!key || !plaintext || !ciphertext) return CRYPT_NULL_PTR;

    switch (key_len){
        case AES_128:
            if(key_size != AES_128_KEY_SIZE_BYTES)
                return CRYPT_BAD_KEY;
            break;
        case AES_192:
            if(key_size != AES_192_KEY_SIZE_BYTES)
                return CRYPT_BAD_KEY;
            break;
        case AES_256:
            if(key_size != AES_256_KEY_SIZE_BYTES)
                return CRYPT_BAD_KEY;
            break;
        default:
            return CRYPT_AES_BAD_KEY_LEN;
    }

    if (mode == CBC || mode == CTR || mode == GCM) {
        if (!iv || iv_len != AES_BLOCK_SIZE_BYTES) return CRYPT_BAD_IV;
    }

    // if no padding is selected, then we require the plaintext be full blocks only
    if (mode == ECB) {
        if (padding == NO_PAD){
            if(ciphertext_len % AES_BLOCK_SIZE_BYTES != 0)
                return CRYPT_BAD_BUFFER_LEN;   
        }
    } 
    if (plaintext_len < ciphertext_len)
        return CRYPT_BAD_BUFFER_LEN;

    if (padding != NO_PAD && padding != PKCS7) 
        return CRYPT_INVALID_PADDING;

    aes_ctx ctx = {0};

    switch(mode){
        case ECB:
            CRYPT_CHECK_OKAY(AES_init(&ctx, key, key_len));
            CRYPT_CHECK_OKAY(AES_decrypt_ECB(&ctx, ciphertext, ciphertext_len, plaintext, plaintext_len));
            CRYPT_CHECK_OKAY(AES_cleanup(&ctx));
            break;
        
        case CBC:
            printf("CBC mode not implemented yet\n");
            break;
        
        case CTR:
            printf("CTR mode not implemented yet\n");
            break;
        
        case GCM:
            printf("GCM mode not implemented yet\n");
            break;
        
        default:
            printf("mode not supported\n");
            break;
    }
    
    return CRYPT_OKAY;

}