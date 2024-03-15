#ifndef BIGINT_H
#define BIGINT_H

/*
    My attempt at creating a big integer library, heavily inspired by
    BigNum Math - Implementing Cryptographic Multiple Precision Arithmetic,
    Mozilla NSS library, the HAC and cryptographic coding rules from 
    https://web.archive.org/web/20160525230135/https://cryptocoding.net/index.php/Coding_rules
*/

#include <stdint.h>

#define BASE 0xFFFFFFFF // max value of unsigned int - also the max value of a digit
#define DIGIT_BIT sizeof(digit) * 8

#define CHECK_OKAY(a) do {if (a != BIGINT_OKAY) return a; } while(0) 
#define MIN(a, b) ((a) < (b) ? (a) : (b)) // returns min(a, b)
#define MAX(a, b) ((a) > (b) ? (a) : (b)) // returns max(a, b)

typedef uint32_t digit;

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
    BIGINT_ERROR_SHIFTING,
    BIGINT_ERROR_DIVIDE_BY_ZERO,
    BIGINT_ERROR_MUL_INVERSE_DOES_NOT_EXIST,

} bigint_err;

/* bigint utils */

void print_bigint_ctx(const bigint *a);

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
bigint_err bigint_print(const bigint *b, char *str);

/* This function sets the given bigint b to 0, then set the first digit to a */
bigint_err bigint_from_small_int(bigint *b, digit a); 

/* This function takes in a byte array and converts it to a bigint */
bigint_err bigint_from_bytes(bigint *a, const unsigned char *str, unsigned int len);

/* This function converts a bigint to a byte array */
bigint_err bigint_to_bytes(const bigint *a, unsigned char *output, 
                            unsigned int len, int ignore_leading_zero);

/* This function compares the given bigint to 0, returns 0 if the given bigint = 0 
    and 1 otherwise */
int bigint_cmp_zero(const bigint *a);

/* This function compares two bigints returns 0 if a == b, -1 if a < b, 1 if a > b */
int bigint_cmp(const bigint *a, const bigint *b);
int bigint_cmp_noCT(const bigint *a, const bigint *b);

/* Pad zeros to given bigint, and make sure MSD points to msd */
bigint_err bigint_pad_zero(bigint *a, unsigned int msd);

/* Counts the number of bits in a given bigint */
int bigint_bits_count(const bigint *a);

//////////////////////////////////////////////////////////////////////////////////////////////

/* bigint arithmetic */

/* c = a + b, may results in leading zeros in c */
bigint_err bigint_add(const bigint *a, const bigint *b, bigint *c);
/* c = a + b, where b is a small integer */
bigint_err bigint_add_digit(const bigint *a, digit b, bigint *c);
/* a = a + 1 */
bigint_err bigint_inc(bigint *a);

/* c = |a - b| */
bigint_err bigint_sub(const bigint *a, const bigint *b, bigint *c);
bigint_err bigint_sub_digit(const bigint *a, digit b, bigint *c);

/* c = |a * b| */
bigint_err bigint_mul(const bigint *a, const bigint *b, bigint *c);
bigint_err bigint_mul_karatsuba(const bigint *a, const bigint *b, bigint *c);
bigint_err bigint_mul_digit(const bigint *a, digit b, bigint *c);
/* c = a * 2 - wrapper for bigint_left_bit_shift  */
bigint_err bigint_double(const bigint *a, bigint *c);
/* Multiply the given bigint by the base - wrapper for bigint_right_shift */
bigint_err bigint_mul_base(const bigint *a, bigint *c);
/* Multiply the given bigint by the base^b - wrapper for bigint_right_shift_digits */
bigint_err bigint_mul_base_b(const bigint *a, bigint *c, unsigned int b);
/* Multiply the given bigint by 2^b */
bigint_err bigint_mul_pow_2(const bigint *a, unsigned int b, bigint *c);

/* c = a^2 */
bigint_err bigint_square(const bigint *a, bigint *c);

/* c = |a / b| */
bigint_err bigint_div(const bigint *a, const bigint *b, bigint *q, bigint *r);
bigint_err bigint_int_div(const bigint *a, const bigint *b, bigint *q, bigint *r);
bigint_err bigint_div_digit(const bigint *a, digit b, bigint *c);
/* c = a / 2 - wrapper for bigint_right_bit_shift */
bigint_err bigint_halve(const bigint *a, bigint *c);
/* Divide the given bigint by the base - wrapper for bigint_left_shift */
bigint_err bigint_div_base(const bigint *a, bigint *c);
/* Divide the given bigint by the base^b - wrapper for bigint_right_shift_digits */
bigint_err bigint_div_base_b(const bigint *a, bigint *c, unsigned int b);
/* Divide the given bigint by 2^b */
bigint_err bigint_div_pow_2(const bigint *a, unsigned int b, bigint *c);

/* c = a % b */
bigint_err bigint_mod(const bigint *a, const bigint *b, bigint *c);
bigint_err bigint_mod_pow_2(const bigint *a, unsigned int b, bigint *c);

/* c = a * b (mod m) */
bigint_err bigint_mul_mod(const bigint *a, const bigint *b, const bigint *m, bigint *c);
/* c = a^2 (mod m) */
bigint_err bigint_square_mod(const bigint *a, const bigint *m, bigint *c);
/* c = a^e mod m */
bigint_err bigint_expt_mod(const bigint *a, const bigint *e, const bigint *m, bigint *c);
/* c = a^(-1) (mod m) */
bigint_err bigint_inverse_mod(const bigint *a, const bigint *m, bigint *c);

/////////////////////////////////////////////////////////////////////////////////////////

/* bigint number theory stuff */

/* c = gcd(a, b) */
bigint_err bigint_gcd(const bigint *a, const bigint *b, bigint *c);

/* c = lcm(a, b) */
bigint_err bigint_lcm(const bigint *a, const bigint *b, bigint *c);

/* Extended Euclidean algorithm, used to find modular multiplicative inverse here */
bigint_err bigint_extended_gcd(const bigint *a, const bigint *m, bigint *gcd, bigint *x);



/////////////////////////////////////////////////////////////////////////////////////////


/* bigint bitwise operation */
/* c = a >> 1 */
bigint_err bigint_right_bit_shift(const bigint *a, bigint *c);
/* c = a << 1 */
bigint_err bigint_left_bit_shift(const bigint *a, bigint *c);
/* c = a & b */
bigint_err bigint_and(const bigint *a, const bigint *b, bigint *c);
/* c = a | b */
bigint_err bigint_or(const bigint *a, const bigint *b, bigint *c);
/* c = a ^ b */
bigint_err bigint_xor(const bigint *a, const bigint *b, bigint *c);
/* c = ~a */
bigint_err bigint_not(const bigint *a, bigint *c);


// These shifting functions are not constant-time, more works needed
/* 
    These functions shift the given bigint by one digit to the left or right 
    This is equivalent to multiplying or dividing by the base (2^32 for this library) 
*/
bigint_err bigint_left_shift(bigint *a);
bigint_err bigint_right_shift(bigint *a);

/* 
    These functions shift the given bigint by b digits 
    This is equivalent to multiplying or dividing by the base^b    
*/
bigint_err bigint_left_shift_digits(bigint *a, unsigned int b);
bigint_err bigint_right_shift_digits(bigint *a, unsigned int b);

/* 
    These functions shift the given bigint by b bits 
    This is equivalent to multiplying or dividing by the 2^b    
*/
bigint_err bigint_left_shift_bits(bigint *a, unsigned int b);
bigint_err bigint_right_shift_bits(bigint *a, unsigned int b);

#endif /* BIGINT_H  */