#include "bigint_test.h"
#include "../util/bigint.h"
#include "test_util.h"
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

int bigint_inc_test(){
    int failed = 0;
    print_msg(YELLOW, "bigint_inc_test");
    bigint b, c;
    int res = 0;

    bigint_init(&b, 1); 
    bigint_init(&c, 1);
    bigint_from_small_int(&c, 1);
    bigint_inc(&b); 
    res = bigint_cmp(&b, &c);
    if(!res){
        print_passed("bigint_inc_test 1 passed");
    } else {
        print_failed("bigint_inc_test 1 failed");
        failed = 1;
    }
    bigint_free(&b); // Cleanup.
    bigint_free(&c);

    bigint_init(&b, 2); // Allocate two digits for potential carry over.
    bigint_init(&c, 1);
    b.digits[0] = BASE - 1; 
    c.digits[0] = BASE;
    bigint_inc(&b); // Increment bigint.
    res = bigint_cmp(&b, &c);
    if(!res){
        print_passed("bigint_inc_test 2 passed");
    } else {
        print_failed("bigint_inc_test 2 failed");
        failed = 1;
    }
    bigint_free(&b); // Cleanup.
    bigint_free(&c);

    bigint_init(&b, 2); // Assume we're working with 2 digits for this test.
    b.digits[0] = BASE - 1;
    b.digits[1] = BASE - 1;
    b.MSD = 1; // Manually set MSD to simulate a filled bigint.
    bigint_err result = bigint_inc(&b); // Attempt to increment.

    // Expected result: BIGINT_ERROR_OVERFLOW
    if (result == BIGINT_ERROR_OVERFLOW) {
        print_passed("Overflow detected as expected");
    } else {
        print_failed("Unexpected behavior");
        failed = 1;
    }
    bigint_free(&b); // Cleanup.
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
    bigint_print(&b);

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
        print_passed("bigint_add_test - python generated random bigint passed");
        bigint_to_bytes(&c, c_bytes, 16, 1);
        print_bytes(c_bytes, 16, "c = ");
    }
    else {
        print_failed("bigint_add_test - python generated random bigint failed");
        bigint_to_bytes(&c, c_bytes, 16, 1);
        print_bytes(c_bytes, 16, "c = ");
        print_bytes(exp_res, 16, "expected output: ");
        failed = 1;
    }

    bigint_set_zero(&c);
    bigint_from_small_int(&b, 1);
    bigint_add(&a, &b, &c);

    const unsigned char exp_res1[] = {0x79, 0xEE, 0x36, 0xD7, 0xBC, 0x26, 0xB3, 0xBC, 
                                    0x9A, 0x60, 0x4E, 0x98, 0xEA, 0xEB, 0x7C, 0xBC};
    bigint_from_bytes(&res, exp_res1, 16);
    bigint_print(&res);
    r = bigint_cmp(&c, &res);
    if(!r){
        print_passed("bigint_add_test - python generated random bigint 2 passed");
        bigint_to_bytes(&c, c_bytes, 16, 1);
        print_bytes(c_bytes, 16, "c = ");
        bigint_print(&c);
    }
    else {
        print_failed("bigint_add_test - python generated random bigint 2 failed");
        bigint_to_bytes(&c, c_bytes, 16, 1);
        print_bytes(c_bytes, 16, "c = ");
        print_bytes(exp_res1, 16, "expected output: ");
        failed = 1;
    }

    bigint_free(&a);
    bigint_free(&b);
    bigint_free(&c);
    bigint_free(&res);
    
    return failed;
}
