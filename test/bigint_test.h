#ifndef BIGINT_TEST_H
#define BIGINT_TEST_H

typedef struct {
    char *name;
    int (*unit_test_fn)();
    int enabled;
} bigint_tests;

int bigint_expand_test();
int bigint_clamp_test();
int bigint_set_zero_test();
int bigint_left_shift_test();
int bigint_right_shift_test();
int bigint_from_to_bytes_test();
int bigint_pad_zero_test();
int bigint_cmp_zero_test();
int bigint_cmp_test();
int bigint_add_test();
int bigint_sub_test();
int bigint_mul_test();

int bigint_double_test();
int bigint_half_test();

int bigint_div_base_test();
int bigint_div_pow_2_test();
int bigint_mul_base_test();
int bigint_mul_pow_2_test();

int bigint_bitwise_op_test();

int bigint_add_digit_test();
int bigint_sub_digit_test();
int bigint_mul_digit_test();

#endif /* BIGINT_TEST_H */