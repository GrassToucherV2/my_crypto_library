#include "bigint_test.h"
#include "../util/bigint.h"
#include "test_util.h"
#include <valgrind/memcheck.h>
#include <assert.h>
#include <stdlib.h>

int bigint_expand_test() {
    int failed = 0;
    print_msg(YELLOW, "bigint_expand test");
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
    print_msg(YELLOW, "bigint_clamp_test");
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
    print_msg(YELLOW, "bigint_set_zero test");

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
    print_msg(YELLOW, "bigint_left_shift test");
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

int bigint_from_to_bytes_test() {
    int failed = 0;
    print_msg(YELLOW, "bigint_from_to_bytes_test");
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

int bigint_cmp_zero_test(){
    int failed = 0;
    print_msg(YELLOW, "bigint_cmp_zero_test");
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
    print_msg(YELLOW, "bigint_cmp_test");
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
    if(res){
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
    if(!res){
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
    if(res){
        print_passed("bigint_cmp_test -  python generated random bigint passed");
    } else{
        print_failed("bigint_cmp_test -  python generated random bigint failed");
        failed = 1;
    }

    res = 0;
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
        print_failed("bigint_cmp_test -  python generated random bigint 2 failed");
        printf("res = %d\n", res);
        failed = 1;
    }
    bigint_free(&a);
    bigint_free(&b);

    return failed;
}

int bigint_add_test(){
    int failed = 0;
    print_msg(YELLOW, "bigint_add_test");
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

int bigint_inc_test(){
    int failed = 0;
    print_msg(YELLOW, "bigint_inc_test");
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
    bigint_free(&b); 
    bigint_free(&c);
    bigint_free(&r);

    return failed;
}

int bigint_sub_test(){
    int failed = 0;
    print_msg(YELLOW, "bigint_sub_test");
    bigint a, b, c, res;
    int rv = 0;
    bigint_init(&a, 1);
    bigint_init(&b, 4);
    bigint_init(&c, 4);
    bigint_init(&res, 4);
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
    print_msg(YELLOW, "bigint_sub_digit_test");

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
