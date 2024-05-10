#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;

class Subtest {
private:
    string name;
    function<void()> test_function;

public:
    Subtest(string name, function<void()> func) : name(name), test_function(func) {}

    void run() {
        cout << "Running subtest: " << name << endl;
        test_function();
    }
};


class TestBase {
    private:
        string test_name;
        bool enabled;
        vector<Subtest> subtests;
    
    public:
        TestBase();
        virtual ~TestBase();

        void set_test_name(string test_name);
        void enable_test();
        void disable_test();
        string get_test_name();
        bool is_test_enabled();
        // virtual void run_test(){
        //     cout << "run test" << endl;
        // };

        virtual void run_test() {
            cout << "Running test: " << test_name << endl;
            for (auto& subtest : subtests) {
                subtest.run();
            }
        }

        void add_subtest(const string& subtest_name, function<void()> test_function) {
            subtests.emplace_back(subtest_name, test_function);
        }
};

class Testsuite{
    private:
        vector<TestBase *> tests;

    public:
        Testsuite() {}
        virtual ~Testsuite() {
            for (auto test : tests) delete test;
            tests.clear();
        }
        void run_test_by_name(string name);
        void add_test(TestBase *test);
        void remove_test(TestBase *test);
        void get_test_list();
};

#define SUBTEST(parentTest, subtestName) \
class subtestName : public Subtest { \
public: \
    subtestName() : Subtest(#subtestName, [this]() { this->subtest_function(); }) { \
        parentTest.add_subtest(#subtestName, [this]() { this->subtest_function(); }); \
    } \
    void subtest_function(); \
}; \
void subtestName::subtest_function()


#define TEST_CLASS_NAME(testName) testName
#define REGISTER_TEST_INSTANCE(testName, instanceName) static bool instanceName = (testName::__register(), true)
#define TEST(testName, testsuiteName) \
class testName : public TestBase { \
public: \
    TEST_CLASS_NAME(testName)() { set_test_name(#testName); enable_test(); } \
    static void __register() { \
        testsuiteName.add_test(new TEST_CLASS_NAME(testName)); \
    } \
    void run_test() override; \
}; \
REGISTER_TEST_INSTANCE(TEST_CLASS_NAME(testName), instance_##testName); \
void TEST_CLASS_NAME(testName)::run_test() // Ensure no newline after this line
