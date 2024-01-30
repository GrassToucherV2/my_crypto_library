#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../util/tools.h"

#define ASSERT_EQ(a, b) ((a) == (b))

#define RED 31
#define GREEN 32
#define YELLOW 33

// return true if a == b
bool assert_eq(int a, int b);

// return 1 if two strings are the same 
bool cmp_str(char *a, char *b, int len);

void print_msg(int color, char *msg);

/* utils unit tests */
typedef struct {
    char *name;
    void (*unit_test_fn)();
    int enabled;
} util_unit_test;

void hex_to_bytes_conv_test();
void is_odd_test();
void three_bytes_to_base64_test();
void bytes_to_base64_test();


#endif /* TEST_UTIL_H */