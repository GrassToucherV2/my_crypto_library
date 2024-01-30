#include "set1_challenge.h"

/*
    Detect single-character XOR
    One of the 60-character strings in this file has been encrypted by single-character XOR.

    Find it.

    Using the same method as challenge 3 to brute-force through all the messages, and printing
    only those with high enough score to be potential decrypted message

    Correct plaintext: Now that the party is jumping
*/
void find_encrypted_text(FILE *f){
    char line[60];
    char ciphertext[30];
    char plaintext[30];
    int line_counter = 1;
    while (fgets(line, sizeof(line), f)){
        // printf("line %d: ", line_counter);
        hex_to_bytes_conv(line, 60, ciphertext);
        break_single_byte_XOR_cipher(ciphertext, plaintext, 30);
        // line_counter++;
    }
}

void set1_challenge4(){
    print_msg(YELLOW, "Set 1 challenge 4 - Detect single-byte XOR");
    FILE *f = fopen("/home/nuohan/Desktop/school/side-project/crypto/set1/encrypted.txt", "r");
    if(f == NULL){
        perror("f is null\n");
    }
    find_encrypted_text(f);
}

