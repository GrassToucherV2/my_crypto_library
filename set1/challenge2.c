#include "set1_challenge.h"

void fixed_length_xor(char *buffer, int len){
    // a and b are hex encoded, meaning that each char represents only one hex digit
    // so they must be converted to byte array (decode hex string) first 
    const char *a = "1c0111001f010100061a024b53535009181c";
    const char *b = "686974207468652062756c6c277320657965";
    char c[18] = {0};
    char d[18] = {0};
    hex_to_bytes_conv(a, 36, c);
    hex_to_bytes_conv(b, 36, d);
    buffers_xor(c, d, len/2, buffer);
}


void set1_challenge2(){
    print_msg(YELLOW, "Now verifying set 1 challenge 2 - fixed length XOR");
    int buffer_len = 18;
    char out_buffer[18] = {0};
    char expected_output_hex[36] = "746865206b696420646f6e277420706c6179";
    char expected_output[18] = {0};
    hex_to_bytes_conv(expected_output_hex, 36, expected_output);
    fixed_length_xor(out_buffer, 36);
    for(int i = 0; i < buffer_len; i++){
        printf("%c",out_buffer[i]);
    }
    printf("\n");
    if(cmp_str(out_buffer, expected_output, buffer_len)){
        print_msg(GREEN, "set 1 challenge 2 - fixed length XOR produced correct output");
    } else{
        print_msg(RED, "set 1 challenge 2 - fixed length XOR produced wrong output");
    }

}