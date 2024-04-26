#include "set1_challenge.h"

/* 
    Implement repeating-key XOR
    Here is the opening stanza of an important work of the English language:

    Burning 'em, if you ain't quick and nimble
    I go crazy when I hear a cymbal

    Encrypt it, under the key "ICE", using repeating-key XOR.

    In repeating-key XOR, you'll sequentially apply each byte of the key; the first byte of 
    plaintext will be XOR'd against I, the next C, the next E, then I again for the 4th byte, 
    and so on.

    It should come out to:

    0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272
    a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f

*/
void repeat_byte_XOR(char *plaintext, char *key, char* ciphertext, 
                    int plaintext_len, int key_len){
    int key_byte_index = 0;
    for(int i = 0; i < plaintext_len; i++){
        ciphertext[i] = plaintext[i] ^ key[key_byte_index];
        printf("ptext[%d] = %c; key[%d] = %c, cipher[%d] = %02x\n", 
        i, plaintext[i], key_byte_index, key[key_byte_index], i, ciphertext[i]);
        if(key_byte_index < key_len - 1){
            key_byte_index++;
        } else {
            key_byte_index = 0;
        }
        
    }
}

void set1_challenge5(){
    print_msg(YELLOW, "Set 1 Challenge 5 - Repeating-byte XOR");
    char plaintext[] = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
    char ciphertext[74];
    char expected_ciphertext[] = "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";

    char key[3] = "ICE";
    
    repeat_byte_XOR(plaintext, key, ciphertext, 74, 3);
    printf("%d\n", memcmp(ciphertext, expected_ciphertext, 74));
    print_hex(ciphertext, 74);
}
