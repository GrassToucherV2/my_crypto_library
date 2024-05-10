#include <stdio.h>
#include "test/test_util.h"
#include "test/bigint_test.h"
#include "test/hash_test.h"
#include "test/cipher_test.h"
#include "set1/set1_challenge.h"

#define NUM_UTIL_TEST 4
#define NUM_CHAL_TEST 5
#define NUM_BIGINT_TEST (sizeof(bigint_test) / sizeof(bigint_test[0]))
#define NUM_HASH_TEST (sizeof(hash_test) / sizeof(hash_test[0]))

util_unit_test util_tests[] = {
    {"hex_to_bytes_conv_test", &hex_to_bytes_conv_test, 1},
    {"three_bytes_to_base64_test", &three_bytes_to_base64_test, 1},
    {"is_odd_test", &is_odd_test, 1},
    {"bytes_to_base64_test", &bytes_to_base64_test, 1},
        
};

challeng1_test set1_test[] = {
    // {"set1_chal1_hex_to_base64", &set1_challenge1, 1},
    // {"set1_chal2_fixed_length_xor", &set1_challenge2, 1},
    // {"set1_chal3_single_byte_XOR_cipher", &set1_challenge3, 1},
    // {"set1_chal4_Detect_single_character_XOR", &set1_challenge4, 1},
    // {"set1_chal5_repeating_byte_XOR", &set1_challenge5, 1}
};

bigint_tests bigint_test[] = {
    {"bigint_expand_test", &bigint_expand_test, 1},
    {"bigint_clamp_test", &bigint_clamp_test, 1},
    {"bigint_left_shift_test", &bigint_left_shift_test, 1},
    {"bigint_right_shift_test", &bigint_right_shift_test, 1},
    {"bigint_set_zero_test", &bigint_set_zero_test, 1},
    {"bigint_from_bytes_test", &bigint_from_to_bytes_test, 1},
    {"bigint_cmp_zero_test", &bigint_cmp_zero_test, 1},
    {"bigint_cmp_test", &bigint_cmp_test, 1},
    {"bigint_pad_zero_test", &bigint_pad_zero_test, 1},
    {"bigint_add_digit_test", &bigint_add_digit_test, 1},
    {"bigint_add_test", &bigint_add_test, 1},
    {"bigint_inc_test", &bigint_inc_test, 1},
    {"bigint_sub_test", &bigint_sub_test, 1},
    {"bigint_sub_digit_test", &bigint_sub_digit_test, 1},
    {"bigint_mul_test", &bigint_mul_test, 1},
    {"bigint_double_test", &bigint_double_test, 1},
    {"bigint_halve_test", &bigint_halve_test, 1},
    {"bigint_bitwise_op_test", &bigint_bitwise_op_test, 1},
    {"bigint_div_base_test", &bigint_div_base_test, 1},
    {"bigint_mul_base_test", &bigint_mul_base_test, 1},
    {"bigint_mul_pow_2_test", &bigint_mul_pow_2_test, 1},
    {"bigint_div_pow_2_test", &bigint_div_pow_2_test, 1},
    {"bigint_mod_pow_2_test", &bigint_mod_pow_2_test, 1},
    {"bigint_div_test", &bigint_div_test, 1},
    {"bigint_mod_test", &bigint_mod_test, 1},
    {"bigint_gcd_test", &bigint_gcd_test, 1},
    {"bigint_lcm_test", &bigint_lcm_test, 1},
    {"bigint_inverse_mod_test", &bigint_inverse_mod_test, 1},
    {"bigint_mul_mod_test", &bigint_mul_mod_test, 1},
    {"bigint_square_mod_test", &bigint_square_mod_test, 1},
    {"bigint_expt_mod_test", &bigint_expt_mod_test, 1},
};

hash_tests hash_test[] = {
    {"md5_test", &md5_test, 1},
    {"sha1_test", &sha1_test, 1},
    {"sha256_test", &sha256_test, 1},
    {"sha512_test", &sha512_test, 1},
};

cipher_tests cipher_test[] = {
    {"chacha20_test", &chacha20_test, 1},
    {"poly1305_test", &poly1305_test, 1},
    {"chacha20_poly1305_test", &chacha20_poly1305_test, 1},
    {"des_test", &des_test, 1},
    {"3DES_test", &tdes_test, 1},
    {"AES_test", &aes_test, 1},
    {"AES_CBC_test", &AES_CBC_test, 1},
    {"AES_CTR_test", &AES_CTR_test, 1},
    {"HMAC_SHA256_test", &hmac_test, 1},
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
    // int num_set1_test = sizeof(set1_test) / sizeof(set1_test[0]);
    // for(int i = 0; i < num_set1_test; i++){
    //     if(set1_test[i].enabled){
    //         set1_test[i].challenge_fn();
    //         printf("\n");
    //     }
    // }
}

void run_bigint_test(){
    printf("========== bigint library tests ==========\n");
    int num = sizeof(bigint_test) / sizeof(bigint_test[0]);
    int num_failed = 0;
    int num_skipped = 0;
    for(int i = 0; i < num; i++){
        if(bigint_test[i].enabled){
            print_msg(YELLOW, bigint_test[i].name);
            num_failed += bigint_test[i].unit_test_fn();
            printf("\n");
        } else {
            num_skipped++;
        }
    }
    printf("========== Summary ==========\n");
    printf("Total tests: %d\n", num);
    printf("Passed: %d\n", num - num_failed - num_skipped);
    printf("Failed: %d\n", num_failed);
    printf("Skipped: %d\n", num_skipped);
}

void run_hash_test(){
    printf("========== Hashing Algorithms tests ==========\n");
    int num = sizeof(hash_test) / sizeof(hash_test[0]);
    int num_failed = 0;
    int num_skipped = 0;
    for(int i = 0; i < num; i++){
        if(hash_test[i].enabled){
            print_msg(YELLOW, hash_test[i].name);
            num_failed += hash_test[i].unit_test_fn();
            printf("\n");
        } else {
            num_skipped++;
        }
    }
    printf("========== Summary ==========\n");
    printf("Total tests: %d\n", num);
    printf("Passed: %d\n", num - num_failed - num_skipped);
    printf("Failed: %d\n", num_failed);
    printf("Skipped: %d\n", num_skipped);
}

void run_cipher_test(){
    printf("========== Cipher Suite tests ==========\n");
    int num = sizeof(cipher_test) / sizeof(cipher_test[0]);
    int num_failed = 0;
    int num_skipped = 0;
    for(int i = 0; i < num; i++){
        if(cipher_test[i].enabled){
            print_msg(YELLOW, cipher_test[i].name);
            num_failed += cipher_test[i].unit_test_fn();
            printf("\n");
        } else {
            num_skipped++;
        }
    }
    printf("========== Summary ==========\n");
    printf("Total tests: %d\n", num);
    printf("Passed: %d\n", num - num_failed - num_skipped);
    printf("Failed: %d\n", num_failed);
    printf("Skipped: %d\n", num_skipped);
}

int main(int argc, char *argv[]){
    if(argc == 1){
        printf("Running all tests\n\n");
        run_util_tests();
        // run_challenge_test();
        run_bigint_test();
        run_hash_test();
        run_cipher_test();
    } else if(argc == 2){
        if(!strcmp(argv[1], "u")){
            printf("Running util tests\n\n");
            run_util_tests();
        } 
        // else if(!strcmp(argv[1], "c")){
        //     printf("Running challenge tests\n\n");
        //     run_challenge_test();
        // }
        else if(!strcmp(argv[1], "b")){
            printf("Running bigint tests\n\n");
            run_bigint_test();
        }
        else if(!strcmp(argv[1], "h")){
            printf("Running hashing algorithms tests\n\n");
            run_hash_test();
        }
        else if(!strcmp(argv[1], "c")){
            printf("Running cipher suite tests\n\n");
            run_cipher_test();
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
        // else if(!strcmp(argv[1], "c")){
        //     for(int i = 0; i < NUM_CHAL_TEST; i++){
        //         if(!strcmp(argv[2], set1_test[i].name)){
        //             set1_test[i].challenge_fn();
        //             printf("\n");
        //             return 0;
        //         }
        //     }
        //     printf("Test %s not found\n", argv[2]);
        //     return 0;
        // }
        else if(!strcmp(argv[1], "b")){
            for(int i = 0; i < (int)NUM_BIGINT_TEST; i++){
                if(!strcmp(argv[2], bigint_test[i].name)){
                    bigint_test[i].unit_test_fn();
                    printf("\n");
                    return 0;
                }
            }
            printf("Test %s not found\n", argv[2]);
            return 0;
        }
        else if(!strcmp(argv[1], "h")){
            for(int i = 0; i < (int)NUM_HASH_TEST; i++){
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
