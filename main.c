#include <stdio.h>
#include "test/test_util.h"
#include "test/bigint_test.h"
#include "set1/set1_challenge.h"

#define NUM_UTIL_TEST 4
#define NUM_CHAL_TEST 5
#define NUM_BIGINT_TEST 1

util_unit_test util_tests[] = {
    {"hex_to_bytes_conv_test", &hex_to_bytes_conv_test, 1},
    {"three_bytes_to_base64_test", &three_bytes_to_base64_test, 1},
    {"is_odd_test", &is_odd_test, 1},
    {"bytes_to_base64_test", &bytes_to_base64_test, 1},
        
};

challeng1_test set1_test[] = {
    {"set1_chal1_hex_to_base64", &set1_challenge1, 1},
    {"set1_chal2_fixed_length_xor", &set1_challenge2, 1},
    {"set1_chal3_single_byte_XOR_cipher", &set1_challenge3, 1},
    {"set1_chal4_Detect_single_character_XOR", &set1_challenge4, 1},
    {"set1_chal5_repeating_byte_XOR", &set1_challenge5, 1}
};

bigint_tests bigint_test[] = {
    {"bigint test - bigint_inc_test", &bigint_inc_test, 1}
};

void run_util_tests(){
    int num_tests = sizeof(util_tests) / sizeof(util_tests[0]);
    for(int i = 0; i < num_tests; i++){
        if(util_tests[i].enabled){
            util_tests[i].unit_test_fn();  
        }
    }
}

void run_challenge_test(){
    int num_set1_test = sizeof(set1_test) / sizeof(set1_test[0]);
    for(int i = 0; i < num_set1_test; i++){
        if(set1_test[i].enabled){
            set1_test[i].challenge_fn();
            printf("\n");
        }
    }
}

void run_bigint_test(){
    int num = sizeof(bigint_test) / sizeof(bigint_test[0]);
    for(int i = 0; i < num; i++){
        if(bigint_test[i].enabled){
            bigint_test[i].unit_test_fn();
            printf("\n");
        }
    }
}
int main(int argc, char *argv[]){
    if(argc == 1){
        printf("Running all tests\n\n");
        run_util_tests();
        run_challenge_test();
    } else if(argc == 2){
        if(!strcmp(argv[1], "u")){
            printf("Running util tests\n\n");
            run_util_tests();
        } 
        else if(!strcmp(argv[1], "c")){
            printf("Running challenge tests\n\n");
            run_challenge_test();
        }
        else if(!strcmp(argv[1], "b")){
            printf("Running bigint tests\n\n");
            run_bigint_test();
        }
        else{
            printf("Unrecognized arg\n");
            return 1;
        }
    } else if (argc == 3){
        if(!strcmp(argv[1], "u")){
            for(int i = 0; i < NUM_UTIL_TEST; i++){
                if(!strcmp(argv[2], util_tests[i].name)){
                    util_tests[i].unit_test_fn();
                    printf("\n");
                    return 0;
                }
            }
            printf("Test %s not found\n", argv[2]);
            return 0;
        } 
        else if(!strcmp(argv[1], "c")){
            for(int i = 0; i < NUM_CHAL_TEST; i++){
                if(!strcmp(argv[2], set1_test[i].name)){
                    set1_test[i].challenge_fn();
                    printf("\n");
                    return 0;
                }
            }
            printf("Test %s not found\n", argv[2]);
            return 0;
        }
        else if(!strcmp(argv[1], "b")){
            for(int i = 0; i < NUM_BIGINT_TEST; i++){
                if(!strcmp(argv[2], bigint_test[i].name)){
                    bigint_test[i].unit_test_fn();
                    printf("\n");
                    return 0;
                }
            }
            printf("Test %s not found\n", argv[2]);
            return 0;
        }
    }
    
    else {
        printf("Bad arg\n");
        return 1;
    }
    
    return 0;
}