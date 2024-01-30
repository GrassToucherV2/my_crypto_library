#ifndef SET_CHALLENGE1_H
#define SET_CHALLENGE1_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "set1_challenge.h"
#include "../util/tools.h"
#include "../util/bigint.h"
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

int hex_to_base64(char *output);
void fixed_length_xor(char *output, int len);
void break_single_byte_XOR_cipher(char *ciphertext, char *plaintext, int len);
void find_encrypted_text();

#endif /* SET_CHALLENGE1_H */