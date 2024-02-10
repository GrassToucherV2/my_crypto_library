#ifndef BIGINT_H
#define BIGINT_H

/*
    My attempt at creating a big integer library, heavily inspired by
    BigNum Math - Implementing Cryptographic Multiple Precision Arithmetic,
    Mozilla NSS library and cryptographic coding rules from 
    https://web.archive.org/web/20160525230135/https://cryptocoding.net/index.php/Coding_rules
*/

#include <stdint.h>

// #define BASE 0x3B9ACA00 /* 10^9 = 0x3B9ACA00 */ /* 2^31 = 2147483648*/
#define BASE 0xFFFFFFFF // max value of unsigned int - also the max value of a digit
#define DIGIT_BIT sizeof(digit) * 8

#define CHECK_OKAY(a) do {if (a != BIGINT_OKAY) return a; } while(0) 
typedef uint32_t digit;

void buffers_xor(const char *a, const char *b, int len, char *output);

typedef struct {
    unsigned int num_of_digit;  /* number of digits allocated */
    digit *digits;              /* the digits themselves */
    unsigned int MSD;           /* index of most significant digit */
} bigint;

typedef enum {
    BIGINT_OKAY = 0,
    BIGINT_ERROR_OVERFLOW,
    BIGINT_ERROR_NULLPTR,
    BIGINT_REALLOC_FAILURE, 
    BIGINT_ERROR_SET_ZERO, 
    BIGINT_ERROR_INSUFFICIENT_BUF,
    BIGINT_ERROR_GENERIC,

} bigint_err;

/* bigint utils */

/* Initialize a bigint and setting it to 0 */
bigint_err bigint_init(bigint* b, unsigned int num_of_digit);

/* Free a bigint */
bigint_err bigint_free(bigint *b);

/* This function expands the bigint so that it has at least "num" digits */
bigint_err bigint_expand(bigint *a, unsigned int num);

/* Copy from src to dest */
bigint_err bigint_copy(const bigint *src, bigint *dest); 

/* This function makes a temporary copy of src */
bigint_err bigint_clone(const bigint *src, bigint *dest); 

/* This function removes the leading zeros */
bigint_err bigint_clamp(bigint *a); 

/* This function sets the given bigint to 0 */
bigint_err bigint_set_zero(bigint *a);

/* This function prints the given bigint */
bigint_err bigint_print(const bigint *b);

/* This function sets the given bigint b to 0, then set the first digit to a */
bigint_err bigint_from_small_int(bigint *b, digit a); 

/* This function takes in a byte array and converts it to a bigint */
bigint_err bigint_from_bytes(bigint *a, const unsigned char *str, unsigned int len);

/* This function converts a bigint to a byte array */
bigint_err bigint_to_bytes(const bigint *a, unsigned char *output, 
                            unsigned int len, int ignore_leading_zero);

/* This function shifts the given bigint by one digit to the left */
bigint_err bigint_left_shift(bigint *a);

/* This function compares the given bigint to 0, returns 0 if the given bigint = 0 
    and 1 otherwise */
int bigint_cmp_zero(const bigint *a);

/* This function compares two bigints returns 0 if a == b, -1 if a < b, 1 if a > b */
int bigint_cmp(const bigint *a, const bigint *b);

/* bigint arithmetic */
// a += 1
bigint_err bigint_inc(bigint *a);

// c = a + b, may results in leading zeros in c
bigint_err bigint_add(bigint *a, bigint *b, bigint *c);

// c = |a - b|
bigint_err bigint_sub(bigint *a, bigint *b, bigint *c);

#endif /* BIGINT_H  */