#include "bigint_test.h"
#include "../util/bigint.h"
#include "test_util.h"
#include <assert.h>

void bigint_expand_test() {
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
    printf("test_bigint_expand passed\n");
}

void bigint_inc_test(){
    print_msg(YELLOW, "bigint_inc_test");
    bigint b;
    bigint_init(&b, 1); // Initialize bigint with one digit.
    bigint_from_int(&b, 0); // Set bigint to 0.
    bigint_inc(&b); // Increment bigint.
    bigint_print(&b); // Expected output: 1
    // bigint_free(&b); // Cleanup.

    // print_msg(YELLOW, "bigint_int_test case 2");
    bigint_init(&b, 2); // Allocate two digits for potential carry over.
    b.digits[0] = BASE - 2; // Set the least significant digit to BASE - 1.
    bigint_inc(&b); // Increment bigint.
    bigint_print(&b); // Expected output: 10 (in base 2^31, visualized as decimal for simplicity)
    // unsigned char str[60] = {0};
    // // bigint_to_str(&b, str, 60);
    // for(int i = 0; i < 60; i++){
    //     printf("%c", str[i]);
    // }
    printf("\n");
    // bigint_free(&b); // Cleanup.

    // print_msg(YELLOW, "bigint_int_test case 3");
    bigint_init(&b, 2); // Assume we're working with 2 digits for this test.
    b.digits[0] = BASE - 1;
    b.digits[1] = BASE - 1;
    b.MSD = 1; // Manually set MSD to simulate a filled bigint.
    bigint_err result = bigint_inc(&b); // Attempt to increment.
    // Expected result: BIGINT_ERROR_OVERFLOW
    if (result == BIGINT_ERROR_OVERFLOW) {
        printf("Overflow detected as expected.\n");
    } else {
        printf("Unexpected behavior.\n");
    }
    bigint_free(&b); // Cleanup.
    printf("end of bigint_inc_test\n");
}

void bigint_clamp_test() {
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

    printf("All bigint_clamp tests passed\n");
}

