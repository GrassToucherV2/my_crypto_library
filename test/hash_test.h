#ifndef HASH_TEST_H
#define HASH_TEST_H

typedef struct {
    char *name;
    int (*unit_test_fn)();
    int enabled;
} hash_tests;

int md5_sanity_test();


#endif /* HASH_TEST_H */