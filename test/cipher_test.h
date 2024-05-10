#ifndef CIPHER_TEST_H
#define CIPHER_TEST_H

typedef struct {
    char *name;
    int (*unit_test_fn)();
    int enabled;
} cipher_tests;

int chacha20_test();
int poly1305_test();
int chacha20_poly1305_test();
int des_test();
int tdes_test();
int hmac_test();
int aes_test();
int AES_CBC_test();
int AES_CTR_test();

#endif /* CIPHER_TEST_H */