#ifndef BIGINT_TEST_H
#define BIGINT_TEST_H

typedef struct {
    char *name;
    int (*unit_test_fn)();
    int enabled;
} bigint_tests;

int bigint_inc_test();
int bigint_expand_test();
int bigint_clamp_test();
int bigint_set_zero_test();
int bigint_left_shift_test();
int bigint_from_to_bytes_test();
int bigint_cmp_zero_test();
int bigint_cmp_test();
int bigint_add_test();

#endif /* BIGINT_TEST_H */