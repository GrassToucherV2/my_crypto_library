#include "bigint_test.h"
#include "../util/bigint.h"
#include "test_util.h"

void bigint_inc_test(){
    print_msg(YELLOW, "bigint_int_test case 1");
    bigint b;
    bigint_init(&b, 1); // Initialize bigint with one digit.
    bigint_from_int(&b, 0); // Set bigint to 0.
    bigint_inc(&b); // Increment bigint.
    bigint_print(&b); // Expected output: 1
    // bigint_free(&b); // Cleanup.

    print_msg(YELLOW, "bigint_int_test case 2");
    bigint_init(&b, 2); // Allocate two digits for potential carry over.
    b.digits[0] = BASE - 1; // Set the least significant digit to BASE - 1.
    bigint_inc(&b); // Increment bigint.
    bigint_print(&b); // Expected output: 10 (in base 2^31, visualized as decimal for simplicity)
    // bigint_free(&b); // Cleanup.

    print_msg(YELLOW, "bigint_int_test case 3");
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
}
