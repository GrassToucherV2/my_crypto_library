#include "crypt_test.h"

void TestBase::disable_test(){
    this->enabled = false;
}

void TestBase::enable_test(){
    this->enabled = true;
}

void TestBase::set_test_name(string test_name){
    this->test_name = test_name;
}

bool TestBase::is_test_enabled(){
    return this->enabled;
}

string TestBase::get_test_name(){
    return this->test_name;
}

TestBase::TestBase(){
    this->enabled = false;
    this->test_name = " ";
}

TestBase::~TestBase(){

}

void Testsuite::add_test(TestBase *test){
    this->tests.push_back(test);
}

void Testsuite::remove_test(TestBase *test){
    // Remove the element using erase function and iterators 
    auto it = find(this->tests.begin(), this->tests.end(), test); 
  
    // If element is found found, erase it 
    if (it != this->tests.end()) { 
        this->tests.erase(it); 
    } 
}

void Testsuite::get_test_list(){
    for (auto test : tests) {
        cout << "Test: " << test->get_test_name()
            << " - Enabled: " << (test->is_test_enabled() ? "Yes" : "No") << endl;
    }
}

void Testsuite::run_test_by_name(string name){
    for (auto test: tests){
        if (test->get_test_name() == name){
            test->run_test();
            break;
        }
    }
}