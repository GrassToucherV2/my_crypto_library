#ifndef SET_CHALLENGE1_H
#define SET_CHALLENGE1_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "set1_challenge.h"
#include "../util/tools.h"
#include "../lib/bigint.h"
#include "../test/test_util.h"

/* Challenge tests */
typedef struct {
    char *name;
    void (*challenge_fn)();
    int enabled;
} challeng1_test;

void set1_challenge1();
void set1_challenge2();
void set1_challenge3();
void set1_challenge4();
void set1_challenge5();
void set1_challenge6();
void set1_challenge7();
void set1_challenge8();

int hex_to_base64(char *output);
void fixed_length_xor(char *output, int len);
void break_single_byte_XOR_cipher(char *ciphertext, char *plaintext, int len);
void find_encrypted_text(FILE *f);
void repeat_byte_XOR(char *plaintext, char *key, char* ciphertext, 
                    int plaintext_len, int key_len);

#endif /* SET_CHALLENGE1_H */