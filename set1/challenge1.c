#include "set1_challenge.h"

/*
    To convert hex to base64:
    We convert the hex encoded string into raw byte array, then take 6 bits at a time and take
    that 6 bit value as index to get base64 letter from a base64 char table copied from internet
*/
int hex_to_base64(char *b64_buffer){
    char bytes_buffer[48];
    const char *test_str = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    hex_to_bytes_conv(test_str, strlen(test_str), bytes_buffer);
    return read_b64(bytes_buffer, 48, b64_buffer);
}

void set1_challenge1(){
    print_msg(YELLOW, "Now verifying set 1 challenge 1 - hex_to_base64");
    char output[64] = {0};
    char *expected_output = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";
    int output_len = 0;
    output_len = hex_to_base64(output);
    
    printf("output : ");
    for(int i = 0; i < output_len; i++){
        printf("%c",output[i]);
    }
    printf("\n");
    if(!memcmp(expected_output, output, 64)){
        print_msg(GREEN, "Set 1 Challenge 1 - hex_to_base64 produced correct b64 output");
    } else {
        print_msg(RED, "Set 1 Challenge 1 - hex_to_base64 test failed, wrong b64 string");
        printf("correct output: ");
        for(size_t i = 0; i < strlen(expected_output); i++){
            printf("%c",expected_output[i]);
        }
        printf("\n");
    }
    if(assert_eq(64, output_len)){
        print_msg(GREEN, "Set 1 Challenge 1 - hex_to_base64 produced correct length");
    } else {
        print_msg(RED, "Set 1 Challenge 1 - hex_to_base64 produced wrong length");
    }
}
