#include "bigint_test.h"
#include "../util/bigint.h"
#include "test_util.h"
#include <valgrind/memcheck.h>
#include <assert.h>
#include <stdlib.h>

int bigint_expand_test() {
    int failed = 0;
    bigint a;
    bigint_init(&a, 5); // Initialize bigint with 5 digits

    // Set some values
    for (int i = 0; i < 5; ++i) {
        a.digits[i] = i;
    }

    // Expand the bigint
    bigint_err result = bigint_expand(&a, 10);

    // Test the results
    assert(result == BIGINT_OKAY); // Check if expansion was successful
    assert(a.num_of_digit == 10);  // Check if size is updated correctly

    // Check if the original values are still intact
    for (int i = 0; i < 5; ++i) {
        assert(a.digits[i] == (uint64_t)i); 
    }

    // Check if the new digits are initialized to zero (or as per your implementation)
    for (int i = 5; i < 10; ++i) {
        assert(a.digits[i] == 0);
    }

    // Cleanup
    bigint_free(&a);
    print_passed("test_bigint_expand passed");
    return failed;
}

int bigint_clamp_test() {
    int failed = 0;
    bigint a;
    // Test with a non-zero bigint
    bigint_init(&a, 10); // Initialize bigint with 10 digits, MSD at 5
    for(int i = 0; i < 6; i++){
        a.digits[i] = 1;
    }
    a.MSD = 5;
    bigint_clamp(&a);
    assert(a.MSD == 5); // MSD should remain 5 after clamping
    bigint_free(&a);

    // Test with a bigint that should be clamped
    bigint_init(&a, 10); // Initialize bigint with unnecessary trailing zeros
    a.digits[7] = 0; a.digits[8] = 0; a.digits[9] = 0; // Zeroes after 7th digit
    for(int i = 0; i < 7; i++){
        a.digits[i] = 1;
    }
    a.MSD = 9;
    bigint_clamp(&a);
    assert(a.MSD == 6); // MSD should be updated to 6
    bigint_free(&a);

    // Test with a bigint that is all zeroes
    bigint_init(&a, 10);
    for (unsigned int i = 0; i < 10; i++) {
        a.digits[i] = 0; // All digits are zero
    }
    a.MSD = 9;
    bigint_clamp(&a);
    assert(a.MSD == 0); // MSD should be updated to 0
    bigint_free(&a);
    print_passed("All bigint_clamp tests passed");
    return failed;
}

int bigint_set_zero_test(){
    int failed = 0;

    bigint b;

    // Initialize bigint with a specific value, e.g., 123
    bigint_init(&b, 3); 
    b.digits[0] = 3;
    b.digits[1] = 2;
    b.digits[2] = 1;
    b.MSD = 2; // Since we manually set the number to 123

    // Perform the left shift operation
    assert(bigint_set_zero(&b) == BIGINT_OKAY);
    assert(b.digits[0] == 0); 
    assert(b.MSD == 0);

    // Clean up
    bigint_free(&b);
    print_passed("bigint_set_zero test passed");

    return failed;
}

int bigint_left_shift_test(){
    int failed = 0;
    bigint b;
    bigint_err err;

    // Initialize bigint with a specific value, e.g., 123
    bigint_init(&b, 3); 
    b.digits[0] = 3;
    b.digits[1] = 2;
    b.digits[2] = 1;
    b.MSD = 2; // Since we manually set the number to 123

    // Perform the left shift operation
    err = bigint_left_shift(&b);
    assert(err == BIGINT_OKAY);

    // Verify the result is as expected, i.e., 1230 in this example
    assert(b.digits[0] == 0); // Least significant digit should be 0 after shift
    assert(b.digits[1] == 3);
    assert(b.digits[2] == 2);
    assert(b.digits[3] == 1);
    assert(b.MSD == 3); // MSD should have incremented by 1

    // Print the result (optional, for verification)
    printf("Result of left shift: ");
    bigint_print(&b, "b = ");

    // Clean up
    bigint_free(&b);
    print_passed("bigint_left_shift_test passed");
    return failed;
}

int bigint_right_shift_test(){
    int failed = 0;

    bigint b, r;
    bigint_init(&r, 4);
    bigint_init(&b, 4); 
    unsigned char b1[] = {
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02,
        0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04
    };
    unsigned char exp_res[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03
    };

    bigint_from_bytes(&b, b1, 16);
    bigint_from_bytes(&r, exp_res, 16);

    bigint_err err = bigint_right_shift(&b);
    assert(err == BIGINT_OKAY);

    int res = bigint_cmp(&b, &r);
    if(res == 0){
        print_passed("bigint_right_shift_test passed");
    } else {
        failed = 1;
        bigint_print(&b, "b = ");
        print_failed("bigint_right_shift_test failed");
    }

    // Clean up
    bigint_free(&b);
    bigint_free(&r);
    return failed;
}

int bigint_from_to_bytes_test() {
    int failed = 0;
    unsigned char original_bytes[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00, 0x01, 0x23};
    size_t original_size = sizeof(original_bytes) / sizeof(original_bytes[0]);

    bigint b;
    bigint_init(&b, 10); // Initialize bigint with more digits than necessary for test

    // Convert byte array to bigint
    bigint_err from_result = bigint_from_bytes(&b, original_bytes, original_size);
    if (from_result != BIGINT_OKAY) {
        print_msg_n(RED, "[:(   ] ");
        printf("bigint_from_bytes failed with error code: %d\n", from_result);
        failed = 1;
        return failed;
    }

    // Allocate buffer for conversion back to bytes
    unsigned char* converted_bytes = malloc(original_size);
    if (!converted_bytes) {
        print_msg_n(RED, "[:(   ] ");
        printf("Memory allocation failed for converted_bytes\n");
        failed = 1;
        return failed;
    }

    // Convert bigint back to byte array
    bigint_err to_result = bigint_to_bytes(&b, converted_bytes, original_size, 1); // Assuming ignore_leading_zero is a flag
    if (to_result != BIGINT_OKAY) {
        print_msg_n(RED, "[:(   ] ");
        printf("bigint_to_bytes failed with error code: %d\n", to_result);
        free(converted_bytes);
        failed = 1;
        return failed;
    }

    // Compare original and converted byte arrays
    if (memcmp(original_bytes, converted_bytes, original_size) == 0) {
        print_bytes(converted_bytes, original_size, "converted_bytes: ");
        print_passed("Test PASSED: Conversion from and to bytes is consistent");
    } else {
        print_bytes(original_bytes, original_size, "original_bytes: ");
        print_bytes(converted_bytes, original_size, "converted_bytes: ");
        print_failed("Test FAILED: Original and converted bytes do not match");
        failed = 1;
    }
    free(converted_bytes);

    unsigned char data2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00, 0x01, 0x23};
    original_size = sizeof(data2) / sizeof(data2[0]);
    from_result = bigint_from_bytes(&b, data2, original_size);
    if (from_result != BIGINT_OKAY) {
        print_msg_n(RED, "[:(   ] ");
        printf("bigint_from_bytes failed with error code: %d\n", from_result);
        failed = 1;
        return failed;
    }
    // Allocate buffer for conversion back to bytes
    unsigned char* converted_bytes1 = malloc(original_size);
    if (!converted_bytes1) {
        print_msg_n(RED, "[:(   ] ");
        printf("Memory allocation failed for converted_bytes\n");
        failed = 1;
        return failed;
    }
    to_result = bigint_to_bytes(&b, converted_bytes1, original_size, 0); // Assuming ignore_leading_zero is a flag
    if (to_result != BIGINT_OKAY) {
        print_msg_n(RED, "[:(   ] ");
        printf("bigint_to_bytes failed with error code: %d\n", to_result);
        free(converted_bytes1);
        failed = 1;
        return failed;
    }
    // Compare original and converted byte arrays
    if (memcmp(data2, converted_bytes1, original_size) == 0) {
        print_bytes(converted_bytes1, original_size, "converted_bytes: ");
        print_passed("Test PASSED: Conversion from and to bytes is consistent");
        // print_msg_n(GREEN, "[   OK] ");
        // print_msg(GREEN, "Test PASSED: Conversion from and to bytes is consistent.\n");
    } else {
        print_bytes(data2, original_size , "original_bytes: ");
        print_bytes(converted_bytes1, original_size, "converted_bytes: ");
        print_failed("Test FAILED: Original and converted bytes do not match");
        // print_msg_n(RED, "[:(   ] ");
        // print_msg(RED, "Test FAILED: Original and converted bytes do not match.\n");
        failed = 1;
    }
    free(converted_bytes1);

    unsigned char* converted_bytes2 = malloc(original_size);
    if (!converted_bytes2) {
        printf("Memory allocation failed for converted_bytes\n");
        failed = 1;
        return failed;
    }
    to_result = bigint_to_bytes(&b, converted_bytes2, original_size, 1); // Assuming ignore_leading_zero is a flag
    if (to_result != BIGINT_OKAY) {
        printf("bigint_to_bytes failed with error code: %d\n", to_result);
        free(converted_bytes2);
        failed = 1;
        return failed;
    }
    // Compare original and converted byte arrays
    if (memcmp(data2, converted_bytes2, original_size) == 0) {
        print_bytes(converted_bytes2, original_size, "converted_bytes: ");
        print_passed("Test PASSED: Conversion from and to bytes is consistent");
        // print_msg(GREEN, "Test PASSED: Conversion from and to bytes is consistent.\n");
    } else {
        print_bytes(data2, original_size, "original_bytes: ");
        print_bytes(converted_bytes2, original_size, "converted_bytes: ");
        print_failed("Test FAILED: Original and converted bytes do not match");
        // print_msg(RED, "Test FAILED: Original and converted bytes do not match.\n");
        failed = 1;
    }
    free(converted_bytes2);
    bigint_free(&b);
    return failed;
}

int bigint_pad_zero_test(){
    int failed = 0;

    unsigned char a1[] = {
        0x10, 0x20, 0x30, 0x40
    };

    bigint a;
    bigint_init(&a, 1);
    bigint_from_bytes(&a, a1, 4);
    bigint_pad_zero(&a, 4);
    bigint_print(&a, "a = ");

    if(a.MSD != 4){
        print_failed("a.MSD should be 4");
        failed = 1;
    }

    int check = 0;
    for(int i = 1; i < 5; i++){
        if(a.digits[i] != 0){
            check = 1;
        }
    }
    if(!check){
        print_passed("bigint_pad_zero_test passed");
    } else {
        print_failed("bigint_pad_zero_test failed");
        failed = 1;
    }

    bigint_free(&a);
    return failed;
}

int bigint_cmp_zero_test(){
    int failed = 0;
    int res = 0;
    bigint a;
    bigint_init(&a, 1);
    bigint_from_small_int(&a, 2);
    res = bigint_cmp_zero(&a);
    if(res){
        print_passed("bigint_cmp_zero_test - small bigint passed");
    } else{
        print_failed("bigint_cmp_zero_test - small bigint failed");
        failed = 1;
    }

    res = 0;
    unsigned char data1[] = {0xFF, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                            0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    bigint_from_bytes(&a, data1, 16);
    res = bigint_cmp_zero(&a);
    if(res){
        print_passed("bigint_cmp_zero_test - normal bigint passed");
    } else{
        print_failed("bigint_cmp_zero_test - normal bigint failed");
        failed = 1;
    }

    res = 0;
    unsigned char data2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    bigint_from_bytes(&a, data2, 16);
    res = bigint_cmp_zero(&a);
    if(res){
        print_passed("bigint_cmp_zero_test - leading zero bigint passed");
    } else{
        print_failed("bigint_cmp_zero_test - leading zero bigint failed");
        failed = 1;
    }

    res = 0;
    unsigned char data3[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bigint_from_bytes(&a, data3, 8);
    res = bigint_cmp_zero(&a);
    if(!res){
        print_passed("bigint_cmp_zero_test - zero passed");
    } else{
        print_failed("bigint_cmp_zero_test - zero failed");
        failed = 1;
    }
    bigint_free(&a);
    return failed;
}

int bigint_cmp_test(){
    int failed = 0;
    int res = 0;
    bigint a;
    bigint b;
    bigint_init(&a, 1);
    bigint_init(&b, 1);
    bigint_from_small_int(&a, 2);
    bigint_from_small_int(&b, 2);
    res = bigint_cmp(&a, &b);
    if(!res){
        print_passed("bigint_cmp_test - same small bigint passed");
    } else{
        print_failed("bigint_cmp_test - same small bigint failed");
        failed = 1;
    }

    bigint_from_small_int(&a, 2);
    bigint_from_small_int(&b, 3);
    res = bigint_cmp(&a, &b);
    if(res == -1){
        print_passed("bigint_cmp_test - different small bigint passed");
    } else{
        print_failed("bigint_cmp_test - different small bigint failed");
        failed = 1;
    }

    res = 0;
    unsigned char data1[] = {0xFF, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                            0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    unsigned char datab[] = {0xFF, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                            0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    bigint_from_bytes(&a, data1, 16);
    bigint_from_bytes(&b, datab, 16);
    res = bigint_cmp(&a, &b);
    if(res == 0){
        print_passed("bigint_cmp_test -  same normal bigint passed");
    } else{
        print_failed("bigint_cmp_test -  same normal bigint failed");
        failed = 1;
    }

    res = 0;
    unsigned char data2[] = {0xFE, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                            0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    unsigned char datab2[] = {0xFF, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 
                            0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    bigint_from_bytes(&a, data2, 16);
    bigint_from_bytes(&b, datab2, 15);
    res = bigint_cmp(&a, &b);
    if(res){
        print_passed("bigint_cmp_test -  same normal bigint passed");
    } else{
        print_failed("bigint_cmp_test -  same normal bigint failed");
        failed = 1;
    }

    res = 0;
    unsigned char data3[] = {0xC3, 0x1F, 0x98, 0xAC, 0x19, 0xAB, 0xB1, 0xC5, 
                            0xFB, 0xA1, 0x00, 0x9D, 0xEF, 0xEE, 0x12, 0x96};
    bigint_from_bytes(&a, data3, 16);
    bigint_from_small_int(&b, 0);
    res = bigint_cmp(&a, &b);
    if(res == 1){
        print_passed("bigint_cmp_test -  python generated random bigint passed");
    } else{
        print_failed("bigint_cmp_test -  python generated random bigint failed");
        failed = 1;
    }

    res = 0;
    bigint_free(&a);
    bigint_free(&b);
    bigint_init(&a, 1);
    bigint_init(&b, 1);
    unsigned char data4[] = {0xC3, 0x1F, 0x98, 0xAC, 0x19, 0xAB, 0xB1, 0xC5, 
                            0xFB, 0xA1, 0x00, 0x9D, 0xEF, 0xEE, 0x12, 0x96};
    unsigned char datab4[] = {0xFF, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00,
                            0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    bigint_from_bytes(&a, data4, 16);
    bigint_from_bytes(&b, datab4, 16);
    res = bigint_cmp(&a, &b);
    if(res == -1){
        print_passed("bigint_cmp_test -  python generated random bigint 2 passed");
    } else{
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        print_failed("bigint_cmp_test -  python generated random bigint 2 failed");
        printf("res = %d\n", res);
        failed = 1;
    }

    unsigned char data5[] = {
        0xC3, 0x1F, 0x98, 0xAC, 0x19, 0xAB, 0xB1, 0xC5, 
        0xFB, 0xA1, 0x00, 0x9D, 0xEF, 0xEE, 0x12, 0x96
    };

    unsigned char datab5[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    
    bigint_from_bytes(&a, data5, 16);

    bigint_from_bytes(&b, datab5, 16);
    res = bigint_cmp(&a, &b);
    if(res == 1){
        print_passed("bigint_cmp_test -  leading zeros passed");
    } else{
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        print_failed("bigint_cmp_test -  leading zeros failed");
        failed = 1;
    }

    unsigned char dataa1[] = {0x79, 0xEE, 0x36, 0xD7, 0xBC, 0x26, 0xB3, 0xBC, 
                            0x9A, 0x60, 0x4E, 0x98, 0xEA, 0xEB, 0x7C, 0xBB};
    unsigned char datab1[] = {0x59, 0x25, 0xA9, 0x03, 0x60, 0x40, 0xED, 0x3D};
    bigint_from_bytes(&a, dataa1, 16);
    bigint_from_bytes(&b, datab1, 8);
    res = bigint_cmp(&a, &b);
    if(res == 1){
        print_passed("bigint_cmp_test -  diff length passed");
    } else{
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        print_failed("bigint_cmp_test -  diff length failed");
        failed = 1;
    }

    unsigned char a6[] = {
        0xBC, 0x6A, 0x9F, 0x83, 0x18, 0xA2, 0x8F, 0x23, 
        0x7A, 0x68, 0x28, 0xFA, 0x12, 0x39, 0x55, 0x1C
    };

    unsigned char b6[] = {
        0x30, 0xB7, 0x6C, 0x86, 0x0C, 0xF0, 0x6C, 0x1E, 
        0x17, 0x69, 0x3D, 0xAE, 0x63, 0xD7, 0x11, 0x1C
    };
    bigint_from_bytes(&a, a6, 16);
    bigint_from_bytes(&b, b6, 16);
    res = bigint_cmp(&a, &b);
    if(res == 1){
        print_passed("bigint_cmp_test -  random int passed");
    } else{
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        printf("res = %d\n", res);
        print_failed("bigint_cmp_test -  random int failed");
        failed = 1;
    }
    bigint_free(&a);
    bigint_free(&b);

    return failed;
}

int bigint_add_test(){
    int failed = 0;
    // print_msg(YELLOW, "bigint_add_test");
    bigint a, b, c, res;
    bigint_err rv = 0;
    bigint_init(&a, 4);
    bigint_init(&b, 4);
    bigint_init(&c, 4);
    bigint_init(&res, 4);

    unsigned char dataa[] = {0x79, 0xEE, 0x36, 0xD7, 0xBC, 0x26, 0xB3, 0xBC, 
                            0x9A, 0x60, 0x4E, 0x98, 0xEA, 0xEB, 0x7C, 0xBB};
    unsigned char datab[] = {0x59, 0x25, 0xA9, 0x03, 0x60, 0x40, 0xED, 0x3D, 
                            0xCE, 0x91, 0xD2, 0xBB, 0x8B, 0x95, 0x1C, 0xE7};
    const unsigned char exp_res[] = {0xD3, 0x13, 0xDF, 0xDB, 0x1C, 0x67, 0xA0, 0xFA, 
                            0x68, 0xF2, 0x21, 0x54, 0x76, 0x80, 0x99, 0xA2};

    bigint_from_bytes(&a, dataa, 16);
    bigint_from_bytes(&b, datab, 16);
    bigint_from_bytes(&res, exp_res, 16);

    rv = bigint_add(&a, &b, &c);
    if(rv != BIGINT_OKAY){
        print_failed("bigint_add failed - encountered an error");
        bigint_free(&a);
        bigint_free(&b);
        bigint_free(&c);
        bigint_free(&res);
        failed = 1;
        return failed;
    }
    unsigned char c_bytes[16];
    int r = bigint_cmp(&c, &res);
    if(!r){
        bigint_print(&c, "c = ");
        print_passed("bigint_add_test - python generated random bigint passed");
    }
    else {
        bigint_print(&c, "c = ");
        bigint_print(&res, "r = ");
        print_failed("bigint_add_test - python generated random bigint failed");
        failed = 1;
    }

    bigint_set_zero(&c);
    bigint_from_small_int(&b, 1);
    bigint_add(&a, &b, &c);

    const unsigned char exp_res1[] = {0x79, 0xEE, 0x36, 0xD7, 0xBC, 0x26, 0xB3, 0xBC, 
                                    0x9A, 0x60, 0x4E, 0x98, 0xEA, 0xEB, 0x7C, 0xBC};
    bigint_from_bytes(&res, exp_res1, 16);
    r = bigint_cmp(&c, &res);
    if(!r){
        bigint_print(&c, "c = ");
        print_passed("bigint_add_test - python generated random bigint 2 passed");
    }
    else {
        bigint_to_bytes(&c, c_bytes, 16, 1);
        print_bytes(c_bytes, 16, "c = ");
        print_bytes(exp_res1, 16, "expected output: ");
        print_failed("bigint_add_test - python generated random bigint 2 failed");
        failed = 1;
    }

    unsigned char a1[] = {
        0x39, 0xA8, 0x98, 0x83, 0xBD, 0x1F, 0x15, 0xD7,
        0xD9, 0x93, 0xCE, 0xF9, 0x19, 0xEA, 0xE8, 0xD1
    };
    unsigned char b1[] = {
        0xBB, 0x98, 0xAC, 0xAF, 0xE6, 0xB2, 0x99, 0xA3,
        0x7F, 0xFC, 0xF5, 0x51, 0x6F, 0xB9, 0x5B, 0xE5,
        0x75, 0x63, 0x3D, 0x09, 0xC6, 0x75, 0x99, 0x7B,
        0x58, 0x9E, 0xA6, 0x42, 0xE5, 0xD6, 0x54, 0x15
    };

    unsigned char exp_res2[] = {
        0xBB, 0x98, 0xAC, 0xAF, 0xE6, 0xB2, 0x99, 0xA3,
        0x7F, 0xFC, 0xF5, 0x51, 0x6F, 0xB9, 0x5B, 0xE5,
        0xAF, 0x0B, 0xD5, 0x8D, 0x83, 0x94, 0xAF, 0x53,
        0x32, 0x32, 0x75, 0x3B, 0xFF, 0xC1, 0x3C, 0xE6
    };

    bigint_from_bytes(&a, a1, 16);
    bigint_from_bytes(&b, b1, 32);
    bigint_from_bytes(&res, exp_res2, 32);

    bigint_set_zero(&c);
    bigint_add(&a, &b, &c);
    r = bigint_cmp(&c, &res);
    if(!r){
        bigint_print(&c, "c = ");
        print_passed("bigint_add_test - python generated random bigint 3 passed");
    }
    else {
        bigint_print(&c, "c = ");
        bigint_print(&res, "r = ");
        print_failed("bigint_add_test - python generated random bigint 3 failed");
        failed = 1;
    }

    unsigned char a2[] = {
        0xFB, 0x37, 0xBD, 0x55, 0x08, 0x9A, 0x80, 0xED, 
        0x0F, 0xCF, 0x0F, 0xFD, 0x44, 0x84, 0x19, 0xA3
    };

    unsigned char b2[] = {
        0x40, 0xC2, 0x0B, 0x0A, 0xC2, 0xD3, 0xEA, 0x8D, 
        0xFA, 0x31, 0xD1, 0xD9, 0xEE, 0xDA, 0x11, 0x4C
    };

    unsigned char c2[] = {
        0x01, 0x3B, 0xF9, 0xC8, 0x5F, 0xCB, 0x6E, 0x6B, 
        0x7B, 0x0A, 0x00, 0xE1, 0xD7, 0x33, 0x5E, 0x2A, 
        0xEF
    };

    bigint_free(&c);
    bigint_init(&c, 1);
    bigint_from_bytes(&a, a2, 16);
    bigint_from_bytes(&b, b2, 16);
    bigint_from_bytes(&res, c2, 17);
    bigint_set_zero(&c);
    bigint_add(&a, &b, &c);
    r = bigint_cmp(&c, &res);
    if(!r){
        bigint_print(&c, "c = ");
        print_passed("bigint_add_test - python generated random bigint 4 passed");
    }
    else {
        bigint_print(&res, "r = ");
        bigint_print(&c, "c = ");
        print_failed("bigint_add_test - python generated random bigint 4 failed");
        failed = 1;
    }

    bigint_free(&c);

    bigint_from_bytes(&a, a1, 16);
    bigint_set_zero(&b);
    bigint_init(&c, 1);
    
    bigint_add(&a, &b, &c);
    r = bigint_cmp(&a, &c);
    if(!r){
        bigint_print(&c, "c = ");
        print_passed("bigint_add_test - python generated random bigint add 0 passed");
    }
    else {
        bigint_print(&c, "c = ");
        bigint_print(&a, "r = ");
        print_failed("bigint_add_test - python generated random bigint add 0 failed");
        failed = 1;
    }

    bigint_free(&a);
    bigint_free(&b);
    bigint_free(&c);
    bigint_free(&res);
    
    return failed;
}

int bigint_add_digit_test(){
    int failed = 0;
    // print_msg(YELLOW, "bigint_inc_test");
    bigint b, c, r;
    int res = 0;

    bigint_init(&b, 1); 
    bigint_init(&c, 1);
    bigint_init(&r, 1);
    unsigned char b1[] = {
        0x39, 0xA8, 0x98, 0x83, 0xBD, 0x1F, 0x15, 0xD7,
        0xD9, 0x93, 0xCE, 0xF9, 0x19, 0xEA, 0xE8, 0xD1
    };

    unsigned char r1[] = {
        0x39, 0xA8, 0x98, 0x83, 0xBD, 0x1F, 0x15, 0xD7,
        0xD9, 0x93, 0xCE, 0xF9, 0x19, 0xEA, 0xE8, 0xD2
    };
    bigint_from_bytes(&b, b1, 16);
    bigint_from_bytes(&r, r1, 16);
    bigint_add_digit(&b, 1, &c);
    res = bigint_cmp(&c, &r);
    if(!res){
        print_passed("bigint_inc_test 1 passed");
    } else {
        print_failed("bigint_inc_test 1 failed");
        failed = 1;
    }

    bigint_from_small_int(&b, 9999);
    bigint_add_digit(&b, 1, &c);
    bigint_from_small_int(&r, 10000);
    res = bigint_cmp(&c, &r);
    if(!res){
        print_passed("bigint_inc_test 2 passed");
    } else {
        print_failed("bigint_inc_test 2 failed");
        failed = 1;
    }

    bigint_free(&b); 
    bigint_free(&c);
    bigint_free(&r);

    return failed;
}

int bigint_sub_test(){
    int failed = 0;
    bigint a, b, c, res;
    int rv = 0;
    bigint_init(&a, 1);
    bigint_init(&b, 1);
    bigint_init(&c, 1);
    bigint_init(&res, 1);
    unsigned char a1[] = {
        0xBC, 0x6A, 0x9F, 0x83, 0x18, 0xA2, 0x8F, 0x23, 
        0x7A, 0x68, 0x28, 0xFA, 0x12, 0x39, 0x55, 0x1C
    };

    unsigned char b1[] = {
        0x30, 0xB7, 0x6C, 0x86, 0x0C, 0xF0, 0x6C, 0x1E, 
        0x17, 0x69, 0x3D, 0xAE, 0x63, 0xD7, 0x11, 0x1C
    };

    unsigned char exp_res[] = {
        0x8B, 0xB3, 0x32, 0xFD, 0x0B, 0xB2, 0x23, 0x05, 
        0x62, 0xFE, 0xEB, 0x4B, 0xAE, 0x62, 0x44, 0x00
    };

    bigint_from_bytes(&a, a1, 16);
    bigint_from_bytes(&b, b1, 16);
    bigint_from_bytes(&res, exp_res, 16);
    bigint_sub(&a, &b, &c);
    rv = bigint_cmp(&c, &res);
    if(!rv){
        bigint_print(&c, "c = ");
        print_passed("bigint_sub_test - python generated random bigint passed");
    } else {
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&res, "r = ");
        print_failed("bigint_sub_test - python generated random bigint failed");
    }

    unsigned char a2[] = {
        0x10, 0xC9, 0xF8, 0x2C, 0x81, 0x64, 0x3E, 0xEB,
        0xBE, 0x7E, 0x74, 0x86, 0xDA, 0x35, 0x64, 0x4D,
        0x95, 0x12, 0xA1, 0xA8, 0xF8, 0xA6, 0xFA, 0xE6,
        0xC5, 0x52, 0xC7, 0x48, 0x6B, 0x9C, 0x70, 0xAD
    };

    unsigned char b2[] = {
        0x79, 0x21, 0x8E, 0x52, 0x39, 0x80, 0xC3, 0xC2,
        0xB9, 0x7F, 0x37, 0xE8, 0xC1, 0xDE, 0x36, 0x0A
    };

    unsigned char exp_res2[] = {
        0x10, 0xC9, 0xF8, 0x2C, 0x81, 0x64, 0x3E, 0xEB,
        0xBE, 0x7E, 0x74, 0x86, 0xDA, 0x35, 0x64, 0x4D,
        0x1B, 0xF1, 0x13, 0x56, 0xBF, 0x26, 0x37, 0x24,
        0x0B, 0xD3, 0x8F, 0x5F, 0xA9, 0xBE, 0x3A, 0xA3
    };
    bigint_free(&c);

    bigint_init(&c, 1);
    bigint_from_bytes(&a, a2, 32);
    bigint_from_bytes(&b, b2, 16);
    bigint_from_bytes(&res, exp_res2, 32);
    bigint_sub(&a, &b, &c);

    rv = bigint_cmp(&c, &res);
    if(!rv){
        bigint_print(&c, "c = ");
        print_passed("bigint_sub_test - random bigint with different size passed");
    } else {
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&res, "r = ");
        print_failed("bigint_sub_test - random bigint with different size failed");
    }

    unsigned char a3[] = {
        0x01, 0x7A, 0x2F, 0x55, 0x2F, 0xA9, 0x31, 0xB3,
        0x74, 0xCE, 0xFB, 0x10, 0x63, 0x80, 0xCC, 0x2E,
        0xCD, 0xD2, 0x05, 0xE9, 0x17, 0xF5, 0x98, 0x6D,
        0xB4, 0x5C, 0x80, 0xF6, 0xF3, 0x15, 0x7C, 0x41,
        0xC8, 0xBA, 0x02, 0x5E, 0x62
    };

    unsigned char b3[] = {
        0x04, 0xFB, 0xE6, 0xBA, 0x2D, 0xAB, 0x46, 0x6C,
        0xF0, 0x90, 0xCF, 0xFD, 0x9B, 0xF0, 0x42, 0xAA
    };

    unsigned char exp_res3[] = {
        0x01, 0x7A, 0x2F, 0x55, 0x2F, 0xA9, 0x31, 0xB3,
        0x74, 0xCE, 0xFB, 0x10, 0x63, 0x80, 0xCC, 0x2E,
        0xCD, 0xD2, 0x05, 0xE9, 0x17, 0xF0, 0x9C, 0x86,
        0xFA, 0x2E, 0xD5, 0xB0, 0x86, 0x24, 0xEB, 0x71,
        0xCB, 0x1E, 0x12, 0x1B, 0xB8
    };
    bigint_free(&c);
    bigint_init(&c, 1);
    bigint_from_bytes(&a, a3, 37);
    bigint_from_bytes(&b, b3, 16);
    bigint_from_bytes(&res, exp_res3, 37);
    bigint_sub(&a, &b, &c);

    rv = bigint_cmp(&c, &res);
    if(!rv){
        bigint_print(&c, "c = ");
        print_passed("bigint_sub_test - random bigint with different size 2 passed");
    } else {
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&res, "r = ");
        print_failed("bigint_sub_test - random bigint with different size 2 failed");
    }
    bigint_free(&c);

    bigint_init(&c, 1);
    bigint_set_zero(&b);
    bigint_sub(&a, &b, &c);
    rv = bigint_cmp(&c, &a);
    if(!rv){
        bigint_print(&c, "c = ");
        print_passed("bigint_sub_test - random bigint sub 0 passed");
    } else {
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&a, "r = ");
        print_failed("bigint_sub_test - random bigint sub 0 failed");
    }

    bigint_free(&a);
    bigint_free(&b);
    bigint_free(&c);
    bigint_free(&res);
    return failed;
}

int bigint_sub_digit_test(){
    int failed = 0;
    // print_msg(YELLOW, "bigint_sub_digit_test");

    unsigned char a1[] = {
        0x04, 0x94, 0xF7, 0x07, 0xF5, 0xF2, 0xC9, 0x87,
        0x98, 0xE2, 0x19, 0x23, 0x5A, 0x87, 0xF9, 0x27
    };
    digit b = 0x05D58E;
    unsigned char exp_res[] = {
        0x04, 0x94, 0xF7, 0x07, 0xF5, 0xF2, 0xC9, 0x87,
        0x98, 0xE2, 0x19, 0x23, 0x5A, 0x82, 0x23, 0x99
    };

    int rv = 0;
    bigint a, c, r;
    bigint_init(&a, 1);
    bigint_init(&c, 1);
    bigint_init(&r, 1);

    bigint_from_bytes(&a, a1, 16);
    bigint_from_bytes(&r, exp_res, 16);
    bigint_sub_digit(&a, b, &c);
    rv = bigint_cmp(&c, &r);
    if(!rv){
        bigint_print(&c, "c = ");
        print_passed("bigint_sub_digit_test - random bigint passed");
    } else {
        failed = 1;
        bigint_print(&a, "a = ");
        printf("b = %08X\n", b);
        // bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_sub_digit_test - random bigint failed");
    }

    bigint_free(&a);
    bigint_free(&r);
    bigint_free(&c);
    return failed;
}

int bigint_mul_test(){
    int failed = 0;
    bigint a, b, c, r;
    int res = 0;
    unsigned char a1[] = {
        0xC6, 0x0B, 0x2C, 0xFD, 0x16, 0x9D, 0x96, 0x84,
        0xA1, 0xD9, 0x16, 0x5D, 0xBA, 0xA9, 0x9A, 0x43
    };

    unsigned char b1[] = {
        0x0E, 0x43, 0xB0, 0x3C, 0xE1, 0xB4, 0xB0, 0x31,
        0xA8, 0x9B, 0xCF, 0x2A, 0xA4, 0x41, 0x9C, 0xB7
    };

    unsigned char exp_res[] = {
        0x0B, 0x08, 0xF9, 0xB9, 0x65, 0xA1, 0xAF, 0x0B,
        0x76, 0x50, 0x94, 0x09, 0x96, 0x81, 0x74, 0xF8,
        0xF7, 0x76, 0xBA, 0xB1, 0x34, 0x4E, 0xF3, 0xF1,
        0xD7, 0x51, 0xA2, 0xA0, 0xE0, 0x41, 0x19, 0xE5
    };

    bigint_init(&a, 1);
    bigint_init(&b, 1);
    bigint_init(&c, 1);
    bigint_init(&r, 1);

    bigint_from_bytes(&a, a1, 16);
    bigint_from_bytes(&b, b1, 16);
    bigint_from_bytes(&r, exp_res, 32);

    bigint_mul(&a, &b, &c);
    res = bigint_cmp(&c, &r);
    if(!res){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_test - random bigint passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_test - random bigint sub 0 failed");
    }

    bigint_free(&c);

    unsigned char a2[] = {
        0x2B, 0xD7, 0xE5, 0x45, 0x67, 0xAF, 0x8F, 0xE6,
        0xBA, 0xC1, 0x89, 0xFE, 0xF6, 0xCF, 0x09, 0x7D
    };

    unsigned char b2[] = {
        0x04, 0xAA, 0xAC, 0x5F, 0x05, 0x32, 0x1E, 0xCA,
        0x6A, 0x9C, 0x52, 0x38, 0xAF, 0xDC, 0x11, 0x83
    };

    unsigned char exp_res2[] = {
        0xCC, 0x9A, 0x78, 0xA9, 0xC2, 0x1C, 0x2B, 0xB3,
        0x08, 0x5A, 0x5E, 0x45, 0xD2, 0x05, 0xBD, 0x4F,
        0x5E, 0xE5, 0x20, 0xEB, 0x55, 0xDE, 0xAE, 0x25,
        0x87, 0x4A, 0x9C, 0xA5, 0xFF, 0x27, 0xF7
    };

    bigint_from_bytes(&a, a2, 16);
    bigint_from_bytes(&b, b2, 16);
    bigint_from_bytes(&r, exp_res2, 31);

    bigint_init(&c, 1);
    bigint_mul(&a, &b, &c);
    res = bigint_cmp(&c, &r);
    if(!res){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_test - random bigint 2 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_test - random bigint 2 failed");
    }

    bigint_free(&c);

    unsigned char a3[65] = {
        0xFC, 0xA9, 0xD0, 0xFE, 0xD9, 0x51, 0xCC, 0x3C,
        0x54, 0x1E, 0x6D, 0xE5, 0x4D, 0x3E, 0x15, 0xE9,
        0x30, 0x41, 0x92, 0xFC, 0x5E, 0xAA, 0xA6, 0x89,
        0x35, 0xE3, 0x6A, 0x76, 0xA4, 0xBD, 0xE6, 0x17,
        0x53, 0x6F, 0xEF, 0x86, 0x73, 0x0D, 0xE7, 0xC6,
        0xA6, 0x63, 0x2A, 0x1F, 0xC9, 0xB5, 0x7B, 0xAE,
        0x2E, 0x25, 0x7E, 0x5E, 0xF2, 0x5C, 0x70, 0x31,
        0x76, 0x3C, 0x27, 0x80, 0x60, 0x14, 0xFA, 0x76,
        0xA5
    };

    unsigned char b3[16] = {
        0x5F, 0xB5, 0x46, 0xD1, 0x30, 0xAD, 0xE7, 0x3F,
        0x4C, 0x39, 0x74, 0xB5, 0x89, 0x58, 0x0C, 0x17
    }; 

    unsigned char exp_res3[81] = {
        0x5E, 0x75, 0xEE, 0x84, 0x3C, 0xA9, 0xCB, 0x49,
        0x4D, 0x12, 0xB6, 0xFA, 0xB8, 0x39, 0x03, 0xED,
        0x78, 0x4A, 0x99, 0x58, 0x1E, 0xF2, 0x97, 0x44,
        0xA2, 0x0C, 0x9B, 0x64, 0xAF, 0xBD, 0xC1, 0xC6,
        0x7C, 0x5D, 0x48, 0xC6, 0xFC, 0xE5, 0x93, 0xC7,
        0x35, 0xE5, 0x0C, 0x4E, 0x83, 0x18, 0xBD, 0x05,
        0x7F, 0x87, 0xB7, 0x9C, 0x82, 0x75, 0xFD, 0x97,
        0x65, 0x73, 0x70, 0x45, 0xFD, 0x72, 0x78, 0xF1,
        0x7E, 0xE2, 0x82, 0xC8, 0xB0, 0xA9, 0x27, 0x37,
        0xC1, 0x87, 0x67, 0xEF, 0xDD, 0xB5, 0xC8, 0x64,
        0xD3
    };

    bigint_from_bytes(&a, a3, 65);
    bigint_from_bytes(&b, b3, 16);
    bigint_from_bytes(&r, exp_res3, 81);

    bigint_init(&c, 1);
    bigint_mul(&a, &b, &c);
    res = bigint_cmp(&c, &r);
    if(!res){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_test - random bigint 3 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_test - random bigint 3 failed");
    }

    bigint_free(&c);

    unsigned char zeros[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    bigint_from_bytes(&b, zeros, 8);
    bigint_init(&c, 1); 

    bigint_mul(&a, &b, &c);
    res = bigint_cmp_zero(&c);

    if(!res){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_test - random bigint mul zero passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_test - random bigint mul zero failed");
    }

    bigint_free(&c);

    unsigned char a4[] = {
        0x85, 0x1B, 0xD0, 0x7B, 0xA8, 0x84, 0x8B, 0x78,
        0x72, 0x66, 0xE4, 0x9E, 0xBF, 0x99, 0x06, 0x2E
    };

    unsigned char b4[] = { 0x07, 0x83, 0xCF };

    unsigned char exp_res4[] = {
        0x03, 0xE8, 0x4B, 0x90, 0x8E, 0x76, 0xD8, 0xE6,
        0xD6, 0x89, 0xBF, 0x68, 0x87, 0x55, 0x97, 0x66,
        0x27, 0x89, 0x32
    };

    bigint_from_bytes(&a, a4, 16);
    bigint_from_bytes(&b, b4, 3);
    bigint_from_bytes(&r, exp_res4, 19);
    bigint_init(&c, 1); 

    bigint_mul(&a, &b, &c);
    res = bigint_cmp(&c, &r);

    if(!res){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_test - random bigint 4 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_test - random bigint 4 failed");
    }

    bigint_free(&a);
    bigint_free(&b);
    bigint_free(&c);
    bigint_free(&r);
    return failed;
}

int bigint_double_test(){
    int failed = 0;
    bigint a, c, r;

    unsigned char a1[] = {
        0xEF, 0xBC, 0xEF, 0xE1, 0x21, 0x2F, 0xC6, 0x7E,
        0xEA, 0x46, 0x1B, 0x98, 0x96, 0xB9, 0xC1, 0x97
    };

    unsigned char exp_res[] = {
        0x01, 0xDF, 0x79, 0xDF, 0xC2, 0x42, 0x5F, 0x8C,
        0xFD, 0xD4, 0x8C, 0x37, 0x31, 0x2D, 0x73, 0x83,
        0x2E
    };

    bigint_init(&a, 1);
    bigint_init(&c, 1);
    bigint_init(&r, 1);

    bigint_from_bytes(&a, a1, 16);
    bigint_from_bytes(&r, exp_res, 17);
    
    bigint_double(&a, &c);
    int res = bigint_cmp(&c, &r);
    if(!res){
        bigint_print(&c, "c = ");
        print_passed("bigint_double_test - random bigint passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_double_test - random bigint failed");
    }

    bigint_set_zero(&a);
    bigint_double(&a, &c);
    res = bigint_cmp_zero(&c);

    if(!res){
        bigint_print(&c, "c = ");
        print_passed("bigint_double_test - zero passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_double_test - zero failed");
    }

    bigint_free(&a);
    bigint_free(&c);
    bigint_free(&r);

    return failed;
}

int bigint_half_test(){
    int failed = 0;

    bigint a, c, r;

    unsigned char a1[] = {
        0xDA, 0x81, 0xFF, 0x99, 0x8B, 0x70, 0x29, 0x4F,
        0xE1, 0x75, 0xA6, 0x55, 0xA9, 0xFA, 0xA1, 0x16
    };

    unsigned char r1[] = {
        0x6D, 0x40, 0xFF, 0xCC, 0xC5, 0xB8, 0x14, 0xA7,
        0xF0, 0xBA, 0xD3, 0x2A, 0xD4, 0xFD, 0x50, 0x8B
    };

    bigint_init(&a, 1);
    bigint_init(&c, 1);
    bigint_init(&r, 1);

    bigint_from_bytes(&a, a1, 16);
    bigint_from_bytes(&r, r1, 16);
    
    bigint_half(&a, &c);
    int res = bigint_cmp(&r, &c);
    if(!res){
        bigint_print(&c, "c = ");
        print_passed("bigint_half_test - random bigint passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_half_test - random bigint failed");
    }

    bigint_set_zero(&a);
    bigint_half(&a, &c);
    res = bigint_cmp_zero(&c);
    if(!res){
        bigint_print(&c, "c = ");
        print_passed("bigint_half_test - zero passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        print_failed("bigint_half_test - zero failed");
    }

    unsigned char a2[] = {
        0x1F, 0x7E, 0x77, 0x68, 0x2F, 0x15, 0x24, 0x9C,
        0x9B, 0xD1, 0xD2, 0xED, 0xDE, 0x4C, 0xF4, 0x61
    };

    unsigned char r2[] = {
        0x0F, 0xBF, 0x3B, 0xB4, 0x17, 0x8A, 0x92, 0x4E,
        0x4D, 0xE8, 0xE9, 0x76, 0xEF, 0x26, 0x7A, 0x30
    };

    bigint_from_bytes(&a, a2, 16);
    bigint_from_bytes(&r, r2, 16);

    bigint_set_zero(&c);
    bigint_half(&a, &c);
    res = bigint_cmp(&c, &r);
    if(!res){
        bigint_print(&c, "c = ");
        print_passed("bigint_half_test - odd number passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_half_test - odd number failed");
    }

    bigint_free(&a);
    bigint_free(&c);
    bigint_free(&r);

    return failed;
}

int bigint_bitwise_op_test(){
    int failed = 0;

    bigint a,b,c,r;
    print_msg(YELLOW, "=== bitwise AND test ===");
    unsigned char a_and1[] = {
        0x32, 0x45, 0x63, 0x35, 0x97, 0x14, 0xF3, 0x4D,
        0x6C, 0xBE, 0xEA, 0x00, 0x24, 0x83, 0x70, 0x6E
    };

    unsigned char b_and1[] = {
        0x3C, 0x1B, 0xBF, 0x1C, 0xD1, 0xE2, 0x9D, 0x17,
        0xF6, 0x67, 0x75, 0x23, 0xF7, 0x41, 0x70, 0xE7
    };

    unsigned char exp_res_and1[] = {
        0x30, 0x01, 0x23, 0x14, 0x91, 0x00, 0x91, 0x05,
        0x64, 0x26, 0x60, 0x00, 0x24, 0x01, 0x70, 0x66
    };

    bigint_init(&a, 1);
    bigint_init(&b, 1);
    bigint_init(&c, 1);
    bigint_init(&r, 1);

    bigint_from_bytes(&a, a_and1, 16);
    bigint_from_bytes(&b, b_and1, 16);
    bigint_from_bytes(&r, exp_res_and1, 16);

    bigint_and(&a, &b, &c);

    int res = bigint_cmp(&c, &r);
    if(!res){
        bigint_print(&c, "c = ");
        print_passed("bigint_and_test - random bigint 4 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_and_test - random bigint 4 failed");
    }

    bigint_free(&c);
    
    unsigned char b_and2[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    bigint_init(&c, 1);
    bigint_set_zero(&b);
    bigint_from_bytes(&b, b_and2, 8);
    bigint_and(&a, &b, &c);
    res = bigint_cmp_zero(&c);

    if(!res){
        bigint_print(&c, "c = ");
        print_passed("bigint_and_test - random bigint 4 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_and_test - random bigint 4 failed");
    }

    print_msg(YELLOW, "=== bitwise OR test ===");

    unsigned char a_or1[] = {
        0x88, 0x19, 0x3A, 0x31, 0x8B, 0xDC, 0xEF, 0x4F,
        0x45, 0xBF, 0x79, 0xB4, 0xBE, 0x7B, 0x3E, 0x2D
    };

    unsigned char b_or1[] = {
        0xCD, 0xC2, 0x82, 0x49, 0xC2, 0xFE, 0xF4, 0xC5,
        0x6F, 0xE5, 0xD4, 0x4C, 0xD3, 0xB1, 0xB1, 0x15
    };

    unsigned char exp_res_or1[] = {
        0xCD, 0xDB, 0xBA, 0x79, 0xCB, 0xFE, 0xFF, 0xCF,
        0x6F, 0xFF, 0xFD, 0xFC, 0xFF, 0xFB, 0xBF, 0x3D
    };

    bigint_from_bytes(&a, a_or1, 16);
    bigint_from_bytes(&b, b_or1, 16);
    bigint_from_bytes(&r, exp_res_or1, 16);
    bigint_set_zero(&c);
    bigint_or(&a, &b, &c);
    res = bigint_cmp(&c, &r);

    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_or_test - random bigint 128 bits passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_or_test - random bigint 128 bits failed");
    }

    unsigned char b_or2[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }; 
    bigint_from_bytes(&b, b_or2, 8);
    bigint_set_zero(&c);
    bigint_or(&a, &b, &c);
    res = bigint_cmp(&c, &a);

    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_or_test - random bigint OR 0 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_or_test - random bigint OR 0 failed");
    }

    unsigned char b_or3[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    bigint_from_bytes(&b, b_or3, 16);
    bigint_set_zero(&c);
    bigint_or(&a, &b, &c);
    res = bigint_cmp(&c, &b);

    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_or_test - random bigint OR all 1 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_or_test - random bigint OR all 1 failed");
    }

    print_msg(YELLOW, "=== bitwise XOR test ===");

    unsigned char a_xor1[] = {
        0x58, 0x33, 0x33, 0xC9, 0x19, 0xCF, 0xD1, 0x4C,
        0x9B, 0x35, 0x6D, 0xEC, 0x63, 0x2C, 0x9E, 0xF8
    };

    unsigned char b_xor1[] = {
        0x57, 0xC5, 0x02, 0x4A, 0x06, 0x05, 0x62, 0x41,
        0x8B, 0x8F, 0x09, 0xBA, 0x0B, 0x1B, 0x1C, 0xB6
    };

    unsigned char exp_res_xor1[] = {
        0x0F, 0xF6, 0x31, 0x83, 0x1F, 0xCA, 0xB3, 0x0D,
        0x10, 0xBA, 0x64, 0x56, 0x68, 0x37, 0x82, 0x4E
    };

    bigint_from_bytes(&a, a_xor1, 16);
    bigint_from_bytes(&b, b_xor1, 16);
    bigint_from_bytes(&r, exp_res_xor1, 16);
    bigint_set_zero(&c);
    bigint_xor(&a, &b, &c);
    res = bigint_cmp(&c, &r);

    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_xor_test - random bigint 128 bits passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_xor_test - random bigint 128 bits failed");
    }

    unsigned char a_xor2[] = {
        0xEF, 0x84, 0x29, 0x73, 0x75, 0x4A, 0xFD, 0xD0,
        0x0D, 0xA3, 0x9A, 0x59, 0xB5, 0x51, 0xA8, 0xF0
    };

    unsigned char b_xor2[] = {
        0x03, 0xEA, 0xE4, 0x14, 0x10, 0xEB, 0x27, 0xDB,
        0xFF, 0x10, 0xC8, 0x1A, 0x19, 0x87, 0x68, 0xA3,
        0x02, 0x7A, 0x40, 0x01, 0xB0, 0xC0, 0xEB, 0xF5,
        0xBC, 0xE9, 0xD7, 0xA3, 0x86, 0xBE, 0xB1, 0x16,
        0xBF
    };

    unsigned char exp_res_xor2[] = {
        0x03, 0xEA, 0xE4, 0x14, 0x10, 0xEB, 0x27, 0xDB,
        0xFF, 0x10, 0xC8, 0x1A, 0x19, 0x87, 0x68, 0xA3,
        0x02, 0x95, 0xC4, 0x28, 0xC3, 0xB5, 0xA1, 0x08,
        0x6C, 0xE4, 0x74, 0x39, 0xDF, 0x0B, 0xE0, 0xBE,
        0x4F
    };

    bigint_from_bytes(&a, a_xor2, 16);
    bigint_from_bytes(&b, b_xor2, 33);
    bigint_from_bytes(&r, exp_res_xor2, 33);
    bigint_set_zero(&c);
    bigint_xor(&a, &b, &c);
    res = bigint_cmp(&c, &r);

    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_xor_test - random bigint passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&b, "b = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_xor_test - random bigint failed");
    }

    bigint_free(&a);
    bigint_free(&b);
    bigint_free(&c);
    bigint_free(&r);
    return failed;

}

int bigint_div_base_test(){
    int failed = 0;
    bigint a, c, r;

    unsigned char a1[] = {
        0x34, 0x6B, 0xA5, 0xF4, 0x83, 0x93, 0x0C, 0x73,
        0x1A, 0x16, 0x23, 0x68, 0x2C, 0x83, 0x7E, 0xA8
    };

    unsigned char r1[] = {
        0x00, 0x00, 0x00, 0x00, 0x34, 0x6B, 0xA5, 0xF4, 
        0x83, 0x93, 0x0C, 0x73, 0x1A, 0x16, 0x23, 0x68
    };

    bigint_init(&a, 1);
    bigint_init(&c, 1);
    bigint_init(&r, 1);

    bigint_from_bytes(&a, a1, 16);
    bigint_from_bytes(&r, r1, 16);

    bigint_div_base(&a, &c);
    int res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_div_base_test - divide by BASE passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_div_base_test - divide by BASE failed");
    }

    unsigned char r2[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x34, 0x6B, 0xA5, 0xF4, 0x83, 0x93, 0x0C, 0x73
    };

    bigint_from_bytes(&r, r2, 16);
    bigint_div_base_b(&a, &c, 2);
    res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_div_base_test - divide by BASE^2 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_div_base_test - divide by BASE^2 failed");
    }

    unsigned char r3[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    bigint_from_bytes(&r, r3, 16);
    bigint_div_base_b(&a, &c, 4);
    res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_div_base_test - divide by BASE^4 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_div_base_test - divide by BASE^4 failed");
    }

    bigint_div_base_b(&a, &c, 90);
    res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_div_base_test - divide by BASE^90 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_div_base_test - divide by BASE^90 failed");
    }

    unsigned char r4[] = {
        0x34, 0x6B, 0xA5, 0xF4, 0x83, 0x93, 0x0C, 0x73,
        0x1A, 0x16, 0x23, 0x68, 0x2C, 0x83, 0x7E, 0xA8
    };
    bigint_from_bytes(&r, r4, 16);
    bigint_div_base_b(&a, &c, 0);
    res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_div_base_test - divide by BASE^0 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_div_base_test - divide by BASE^0 failed");
    }

    bigint_free(&a);
    bigint_free(&c);
    bigint_free(&r);

    return failed;
}

int bigint_mul_base_test(){
    int failed = 0;
    bigint a, c, r;

    unsigned char a1[] = {
        0x34, 0x6B, 0xA5, 0xF4, 0x83, 0x93, 0x0C, 0x73,
        0x1A, 0x16, 0x23, 0x68, 0x2C, 0x83, 0x7E, 0xA8
    };

    unsigned char r1[] = {
        0x34, 0x6B, 0xA5, 0xF4, 0x83, 0x93, 0x0C, 0x73, 
        0x1A, 0x16, 0x23, 0x68, 0x2C, 0x83, 0x7E, 0xA8,
        0x00, 0x00, 0x00, 0x00, 
    };

    bigint_init(&a, 1);
    bigint_init(&c, 1);
    bigint_init(&r, 1);

    bigint_from_bytes(&a, a1, 16);
    bigint_from_bytes(&r, r1, 20);

    bigint_mul_base(&a, &c);
    int res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_base_test - multiply by BASE passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_base_test - multiply by BASE failed");
    }

    unsigned char r2[] = {
        0x34, 0x6B, 0xA5, 0xF4, 0x83, 0x93, 0x0C, 0x73, 
        0x1A, 0x16, 0x23, 0x68, 0x2C, 0x83, 0x7E, 0xA8,
    };

    bigint_from_bytes(&r, r2, 16);

    bigint_mul_base_b(&a, &c, 0);
    res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_base_test - multiply by BASE^0 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_base_test - multiply by BASE^0 failed");
    }

    unsigned char r3[] = {
        0x34, 0x6B, 0xA5, 0xF4, 0x83, 0x93, 0x0C, 0x73, 
        0x1A, 0x16, 0x23, 0x68, 0x2C, 0x83, 0x7E, 0xA8,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    };
    bigint_from_bytes(&r, r3, 32);

    bigint_mul_base_b(&a, &c, 4);
    res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_base_test - multiply by BASE^4 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_base_test - multiply by BASE^4 failed");
    }

    bigint_free(&a);
    bigint_free(&c);
    bigint_free(&r);

    return failed;
}

int bigint_mul_pow_2_test(){
    int failed = 0;
    bigint a, c, r;

    unsigned char a1[] = {
        0x34, 0x6B, 0xA5, 0xF4, 0x83, 0x93, 0x0C, 0x73,
        0x1A, 0x16, 0x23, 0x68, 0x2C, 0x83, 0x7E, 0xA8
    };

    unsigned char r1[] = {
        0x34, 0x6B, 0xA5, 0xF4, 0x83, 0x93, 0x0C, 0x73, 
        0x1A, 0x16, 0x23, 0x68, 0x2C, 0x83, 0x7E, 0xA8,
        0x00, 0x00, 0x00, 0x00, 
    };

    bigint_init(&a, 1);
    bigint_init(&c, 1);
    bigint_init(&r, 1);

    bigint_from_bytes(&a, a1, 16);
    bigint_from_bytes(&r, r1, 20);

    bigint_mul_pow_2(&a, 32, &c);
    int res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_base_test - multiply by 2^32 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_base_test - multiply by 2^32 failed");
    }

    unsigned char a2[] = {
        0x1C, 0x41, 0x33, 0x0A, 0x14, 0x6A, 0xBC, 0x70,
        0x71, 0x3A, 0x1D, 0x1F, 0xF4, 0xEF, 0x96, 0x14
    };

    unsigned char r2[] = {
        0x01, 0xC4, 0x13, 0x30, 0xA1, 0x46, 0xAB, 0xC7,
        0x07, 0x13, 0xA1, 0xD1, 0xFF, 0x4E, 0xF9, 0x61,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00
    };

    bigint_from_bytes(&a, a2, 16);
    bigint_from_bytes(&r, r2, 25);

    bigint_set_zero(&c);
    bigint_mul_pow_2(&a, 68, &c);
    res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_base_test - multiply by 2^68 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_base_test - multiply by 2^68 failed");
    }

    bigint_from_small_int(&a, 1);
    bigint_from_small_int(&r, 4);
    bigint_mul_pow_2(&a, 2, &c);
    res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_base_test - multiply by 2^2 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_base_test - multiply by 2^2 failed");
    }

    bigint_free(&a);
    bigint_free(&c);
    bigint_free(&r);

    return failed;
}

int bigint_div_pow_2_test(){
    int failed = 0;
    bigint a, c, r;
    int res = 0;

    unsigned char a1[] = {
        0xDD, 0x68, 0xA1, 0x80, 0x9A, 0x03, 0xFE, 0xA1,
        0xF5, 0x8B, 0x2B, 0x9D, 0xB1, 0x5E, 0x24, 0xB6
    };

    unsigned char r1[] = {
        0x0D, 0xD6, 0x8A, 0x18, 0x09, 0xA0, 0x3F, 0xEA,
        0x1F, 0x58, 0xB2, 0xB9, 0xDB 
    };

    bigint_init(&a, 1);
    bigint_init(&c, 1);
    bigint_init(&r, 1);

    bigint_from_bytes(&a, a1, 16);
    bigint_from_bytes(&r, r1, 13);

    bigint_div_pow_2(&a, 28, &c);
    res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_base_test - divide by 2^28 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_base_test - divide by 2^28 failed");
    }

    unsigned char a2[] = {
        0xDC, 0xAE, 0x9E, 0xEA, 0x8E, 0xCE, 0x7D, 0xC5,
        0xE1, 0xAC, 0x61, 0x2B, 0x5D, 0xB1, 0x8F, 0x12
    };

    unsigned char r2[] = {
        0xDC, 0xAE, 0x9E, 0xEA, 0x8E, 0xCE, 0x7D, 0xC5,
        0xE1, 0xAC, 0x61, 0x2B
    };

    bigint_from_bytes(&a, a2, 16);
    bigint_from_bytes(&r, r2, 12);

    bigint_div_pow_2(&a, 32, &c);
    res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_base_test - divide by 2^32 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_base_test - divide by 2^32 failed");
    }

    unsigned char a3[] = {
        0x05, 0xAF, 0x65, 0xFB, 0xFE, 0xF0, 0x9D, 0x0F,
        0xC6, 0x24, 0xF2, 0x8A, 0x94, 0x3C, 0xFA, 0xA5,
        0xFB, 0x80
    };

    unsigned char r3[] = {
        0xB5
    };

    bigint_from_bytes(&a, a3, 18);
    bigint_from_bytes(&r, r3, 1);

    bigint_div_pow_2(&a, 131, &c);
    res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_base_test - divide by 2^131 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_base_test - divide by 2^131 failed");
    }
    
    bigint_free(&a);
    bigint_free(&c);
    bigint_free(&r);
    return failed;
}

int bigint_mod_pow_2_test(){
    int failed = 0;
    bigint a, c, r;
    int res = 0;

    unsigned char a1[] = {
        0x43, 0x4F, 0x09, 0x01, 0x72, 0x5C, 0x98, 0xFA,
        0x4E, 0x38, 0x4B, 0x9A, 0xC9, 0xAC, 0x2D, 0xF5
    };

    unsigned char r1[] = {
        0x09, 0xAC, 0x2D, 0xF5
    };

    bigint_init(&a, 1);
    bigint_init(&c, 1);
    bigint_init(&r, 1);

    bigint_from_bytes(&a, a1, 16);
    bigint_from_bytes(&r, r1, 4);

    bigint_mod_pow_2(&a, 28, &c);
    res = bigint_cmp(&c, &r);
    if(res == 0){
        bigint_print(&c, "c = ");
        print_passed("bigint_mul_base_test - divide by 2^131 passed");
    } else{
        failed = 1;
        bigint_print(&a, "a = ");
        bigint_print(&c, "c = ");
        bigint_print(&r, "r = ");
        print_failed("bigint_mul_base_test - divide by 2^131 failed");
    }

    bigint_free(&a);
    bigint_free(&c);
    bigint_free(&r);
    return failed;
}