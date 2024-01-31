#ifndef BIGINT_H
#define BIGINT_H

#include <stdint.h>

#define BASE (0x80000000) /* 2^31 = 2147483648*/

void buffers_xor(const char *a, const char *b, int len, char *output);

typedef struct {
    unsigned int num_of_digit;
    unsigned int *digits;
    unsigned int MSD;       /* index of most significant digit */
} bigint;

typedef enum {
    BIGINT_OKAY = 0,
    BIGINT_ERROR_NULLPTR,
    BIGINT_ERROR_OVERFLOW,
    BIGINT_ERROR_GENERIC,

} bigint_err;

/* bigint utils */
void bigint_init(bigint* b, unsigned int num_of_digit);
bigint_err bigint_free(bigint *b);
bigint_err bigint_print(bigint *b);
bigint_err bigint_from_int(bigint *b, unsigned int a);
bigint_err bigint_to_str(bigint *b, unsigned char *str);

/* bigint arithmetic */
// a += 1
bigint_err bigint_inc(bigint *a);
// c = a + b
bigint_err bigint_add(bigint *a, bigint *b, bigint *c);

#endif /* BIGINT_H */