#ifndef CRYPTO_API_H
#define CRYPTO_API_H

#include "common.h"

crypt_status crypt_test();

/////////////////////////////////// API for hashing algorithms ///////////////////////////////////

crypt_status crypt_md5(const unsigned char *input,
                 unsigned int input_len, unsigned char *output,
                 unsigned int output_len
);

crypt_status crypt_sha1(const unsigned char *input,
                 unsigned int input_len, unsigned char *output,
                 unsigned int output_len
);

crypt_status crypt_sha256(const unsigned char *input,
                 unsigned int input_len, unsigned char *output,
                 unsigned int output_len, SHA2 sha
);

crypt_status crypt_sha512(const unsigned char *input,
                 unsigned int input_len, unsigned char *output,
                 unsigned int output_len, SHA2 sha
);

/////////////////////////////////// API for symmetric key algorithms and mac ///////////////////////////////////

crypt_status crypt_chacha20_encrypt(const unsigned char *plaintext, unsigned int plaintext_len,
                                    const unsigned char *key, unsigned int key_len,
                                    const unsigned char *nonce, unsigned int nonce_len,
                                    unsigned int counter, unsigned char *ciphertext,
                                    unsigned int ciphertext_len);

crypt_status crypt_chacha20_decrypt(const unsigned char *ciphertext, unsigned int ciphertext_len,
                                    const unsigned char *key, unsigned int key_len,
                                    const unsigned char *nonce, unsigned int nonce_len,
                                    unsigned int counter, unsigned char *plaintext,
                                    unsigned int plaintext_len);

crypt_status crypt_poly1305(const unsigned char *input, unsigned int input_len, 
                            const unsigned char *key, unsigned int key_len, 
                            const unsigned char *nonce, unsigned int nonce_len,
                            unsigned char *mac, unsigned int mac_len);

crypt_status crypt_chacha20_poly1305_encrypt(const unsigned char *iv, unsigned int iv_len,
                                            const unsigned char *aad, unsigned int aad_len,
                                            const unsigned char *key, unsigned int key_len,
                                            const unsigned char *constant, unsigned int constant_len,
                                            const unsigned char *plaintext, unsigned int plaintext_len,
                                            unsigned char *ciphertext, unsigned int ciphertext_len,
                                            unsigned char *aead_output, unsigned int aead_output_len);

crypt_status crypt_chacha20_poly1305_decrypt(const unsigned char *iv, unsigned int iv_len,
                                            const unsigned char *aad, unsigned int aad_len,
                                            const unsigned char *key, unsigned int key_len,
                                            const unsigned char *constant, unsigned int constant_len,                                            
                                            unsigned char *aead_input, unsigned int aead_input_len,
                                            unsigned char *plaintext, unsigned int plaintext_len);

                                    
#endif /* CRYPTO_API_H */