#include "set1_challenge.h"

/*
    Single-byte XOR cipher:
    The goal is to break a ciphertext where each byte of the plaintext was XOR'ed with 
    a single character (1 byte). This is similar to Caesar cipher, and can be brute forced
    given that the key is only a single byte, so there are a total of 256 possible keys. 

    There should only be one meaningful message, and all other ones would be meaningless, the 
    strategy here is to use the frequency (probability) at which each English letter show up (statistics taken
    from books, articles etc.) then add all the probabilties up. 

    repeat this process witha all possible keys, the greater the sum is, the more likely that
    key is the real one 

    Correct plaintext: Cooking MC's like a pound of bacon
*/

void print_str(char *str, int len){
    for(int i = 0; i < len; i++){
        printf("%c", str[i]);
    }
    printf("\n");
}

void break_single_byte_XOR_cipher(char *ciphertext, char *plaintext, int len){
    double score = 0;
    uint8_t key = 0x00;
    int i;
    for(; key < 0xFF; key++){
        for(i = 0; i < len; i++){
            plaintext[i] = ciphertext[i] ^ key;
        }
        score = score_text(plaintext, len);
        if(score >= 1.2){
            printf("score: %f; ", score);
            print_str(plaintext, 34);
        }  
    }
    key = 0xFF;
    plaintext[i + 1] = ciphertext[i + 1] ^ key;
    score = score_text(plaintext, len);
    if(score >= 1.2){
        print_str(plaintext, 34);
    }
}

void set1_challenge3(){
    print_msg(YELLOW, "Set 1 challenge 3 - break_single_byte_XOR_cipher");
    char ciphertext_hex[69] = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    char ciphertext[35];
    char plaintext[35];
    hex_to_bytes_conv(ciphertext_hex, 69, ciphertext);
    break_single_byte_XOR_cipher(ciphertext, plaintext, 35);
}