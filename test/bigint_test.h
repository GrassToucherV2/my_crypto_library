#ifndef BIGINT_TEST_H
#define BIGINT_TEST_H

typedef struct {
    char *name;
    void (*unit_test_fn)();
    int enabled;
} bigint_tests;

void bigint_inc_test();
void bigint_expand_test();
void bigint_clamp_test();
void bigint_set_zero_test();
void bigint_left_shift_test();
void bigint_from_to_bytes_test();

#endif /* BIGINT_TEST_H */