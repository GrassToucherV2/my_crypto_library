#ifndef BIGINT_H
#define BIGINT_H

/*
    My attempt at creating a big integer library, heavily inspired by:
    BigNum Math - Implementing Cryptographic Multiple Precision Arithmetic
    and Mozilla NSS library
*/

#include <stdint.h>

#define BASE 0x3B9ACA00 /* 10^9 = 0x3B9ACA00 */ /* 2^31 = 2147483648*/

void buffers_xor(const char *a, const char *b, int len, char *output);

typedef struct {
    unsigned int num_of_digit; /* number of digits allocated */
    uint64_t *digits; /* the digits themselves */
    unsigned int MSD;       /* index of most significant digit */
} bigint;

typedef enum {
    BIGINT_OKAY = 0,
    BIGINT_ERROR_NULLPTR,
    BIGINT_ERROR_OVERFLOW,
    BIGINT_REALLOC_FAILURE,  
    BIGINT_ERROR_GENERIC,

} bigint_err;

/* bigint utils */
void bigint_init(bigint* b, unsigned int num_of_digit);
bigint_err bigint_free(bigint *b);
bigint_err bigint_expand(bigint *a, unsigned int size);
void bigint_copy(bigint *src, bigint *dest, int num); /* copy num digits from src to dest */
bigint_err bigint_clamp(bigint *a); /* this function removes the leading zeros */
bigint_err bigint_set_zero(bigint *a);
bigint_err bigint_print(bigint *b);
bigint_err bigint_from_int(bigint *b, unsigned int a);
bigint_err bigint_to_str(bigint *b, unsigned char *str, unsigned int str_size);

/* bigint arithmetic */
// a += 1
bigint_err bigint_inc(bigint *a);
// c = a + b
bigint_err bigint_add(bigint *a, bigint *b, bigint *c);

#endif /* BIGINT_H  */