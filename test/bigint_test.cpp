#include "crypt_test.h"
extern "C"{
    #include "../lib/bigint.h"
    #include "test_util.h"
    #include <valgrind/memcheck.h>
    #include <assert.h>
    #include <stdlib.h>
}

class BigIntTestsuite : public Testsuite {};

// Define a specific test

BigIntTestsuite bigint_testsuite;

TEST(SimpleTest, bigint_testsuite) {
    cout << "This is a simple test with direct commands, no subtests." << endl;

    SUBTEST(SimpleTest, FirstSubtest) {
        cout << "Details of FirstSubtest." << endl;
    };

    SUBTEST(SimpleTest, SecondSubtest) {
        cout << "Details of SecondSubtest." << endl;
    };
};

TEST(SimpleTest, bigint_testsuite) {
    cout << "This is a simple test with direct commands, no subtests." << endl;
    // You can add any logic here that you want to execute as part of this test
    cout << "Performing some checks..." << endl;
    // Example check (just a placeholder)
    if (true) { // Simulating a test condition
        cout << "Check passed!" << endl;
    } else {
        cout << "Check failed!" << endl;
    }
};

int main(){
    bigint_testsuite.run_test_by_name("test");
    bigint_testsuite.run_test_by_name("SimpleTest");
    return 0;
}




