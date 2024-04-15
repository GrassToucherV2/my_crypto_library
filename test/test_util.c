#include "test_util.h"

bool assert_eq(int a, int b){
    if(!ASSERT_EQ(a, b)){
        printf("\033[1;31m"); // printing in different color and style 1 for bold 31 for red 
        printf("Failed, expected %d, got %d\n", a, b);
        printf("\033[0m");
        return false;
    } else{
        printf("\033[1;32m"); 
        printf("Passed\n");
        printf("\033[0m");
    }
    return true;
}

bool cmp_str(char *a, char *b, int len){
    if(!memcmp(a, b, len)){
        return true;
    }
    return false;
}

void print_msg_n(int color, char *msg){
    printf("\033[%dm", color);  
    printf("%s", msg);
    printf("\033[0m");
}

void print_msg(int color, char *msg){
    printf("\033[%dm", color);  
    printf("%s\n", msg);
    printf("\033[0m");
}

void print_passed(char *msg){
    print_msg_n(GREEN, "  [   OK] ");
    print_msg(GREEN, msg);
}

void print_failed(char *msg){
    print_msg_n(RED, "  [:(   ] ");
    print_msg(RED, msg);
}

void print_bytes(const unsigned char* bytes, size_t size, const char* str){
    printf("%s", str);
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", bytes[i]);
    }
    printf("\n");
}

//////////////////////////////////////////////////////////////

void hex_to_bytes_conv_test(){
    print_msg(YELLOW, "Now running hex_to_bytes_conv_test");  
    const char* input1 = "123456789ABCDEF";
    char output1[8]; // Adjust the size according to the expected output
    int output_size1 = hex_to_bytes_conv(input1, strlen(input1), output1);
    printf("Output 1: ");
    for (int i = 0; i < output_size1; ++i) {
        printf("\\x%02X", (unsigned char)output1[i]);
    }
    printf(" ");
    if(!assert_eq(output_size1, 8)){
        print_msg(RED, "hex_to_bytes_conv_test failed");
        return;
    }

    const char* input2 = "A1B2C3D4E5F6";
    char output2[6]; // Adjust the size according to the expected output
    int output_size2 = hex_to_bytes_conv(input2, strlen(input2), output2);
    printf("Output 2: ");
    for (int i = 0; i < output_size2; ++i) {
        printf("\\x%02X", (unsigned char)output2[i]);
    }
    printf(" ");
    if(!assert_eq(output_size2, 6)){
        print_msg(RED, "hex_to_bytes_conv_test failed");
        return;
    };
    print_msg(GREEN, "hex_to_bytes_conv_test passed");
    printf("\n");
}

void is_odd_test(){
    print_msg(YELLOW, "Now running hex_to_bytes_conv_test");
    int a = 1;
    if(!assert_eq(IS_ODD(a), 1)){
        print_msg(RED, "is_odd_test failed");
        return;
    }
    a = 2;
    if(!assert_eq(IS_ODD(a), 0)){
        print_msg(RED, "is_odd_test failed");
        return;
    }
    print_msg(GREEN, "is_odd_test passed");
    printf("\n");
}

void three_bytes_to_base64_test(){
    print_msg(YELLOW, "Now running three_bytes_to_base64");
    int24 num = {0};
    uint8_t nums[3] = {0b00110001, 0b01110001, 0b00000101}; 
    uint8_t arr[4] = {0};
    uint8_t expected_output[4] = {49, 4, 23, 1};

    init_int24(nums, &num);
    printf("(in binary, higher bits to lower bits) nums = "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN "\n", 
        BYTE_TO_BINARY(num.data >> 16), BYTE_TO_BINARY(num.data >> 8),  BYTE_TO_BINARY(num.data) );
    
    three_bytes_to_base64(&num, (char *)arr);
    for(int i = 0; i < 4; i++){
        printf("arr[%d] = %d\n", i, arr[i]);
    }
    if(memcmp(arr, expected_output, 4)){
        print_msg(RED, "three_bytes_to_base64 test failed");
    } else{
        print_msg(GREEN, "three_bytes_to_base64 test Passed");
    }
    printf("\n");
}

void bytes_to_base64_test(){
    print_msg(YELLOW, "Now running bytes_to_base64_test");
    uint8_t a = 0b00001111;
    uint8_t b = 0b11110000;
    uint8_t c = 0;
    c = a & 0b11111100;
    c >>= 2;
    printf("c = "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(c));
    c = a << 6;
    c |= (b >> 2) & 0b00111100;
    c >>= 2;
    printf("c = "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(c));
    printf("\n");
}

/*
    text1 = result 
    text2 = expected text
*/
int assert_eq_texts(unsigned char* text1, unsigned char *text2, unsigned int size, char *str){
    printf("\033[%dm", YELLOW);  
    printf("Test - %s\n", str);
    printf("\033[0m");
    if(memcmp(text1, text2, size)){
        print_failed("The texts are different - assertion failed");
        print_bytes_array(text1, size, "text 1");
        print_bytes_array(text2, size, "expected text");
        return 1;
    } else {
        print_passed("The texts are identical - assertion passed");
        print_bytes_array(text1, size, "text 1");
    }
    return 0;
}