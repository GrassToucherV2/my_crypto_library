#ifndef BIGINT_TEST_H
#define BIGINT_TEST_H

typedef struct {
    char *name;
    void (*unit_test_fn)();
    int enabled;
} bigint_tests;

void bigint_inc_test();

#endif /* BIGINT_TEST_H */