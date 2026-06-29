#include <stdio.h>
#include <string.h>
#include "test/test_util.h"
#include "test/bigint_test.h"
#include "test/hash_test.h"
#include "test/cipher_test.h"

#define NUM_BIGINT_TEST ((int)(sizeof(bigint_test) / sizeof(bigint_test[0])))
#define NUM_HASH_TEST   ((int)(sizeof(hash_test) / sizeof(hash_test[0])))
#define NUM_CIPHER_TEST ((int)(sizeof(cipher_test) / sizeof(cipher_test[0])))

// Struct to hold test statistics
typedef struct {
    const char* category_name;
    int total;
    int passed;
    int failed;
    int skipped;
} TestStats;

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
    {"bigint_mod_test", &bigint_mod_test, 1},
    {"bigint_gcd_test", &bigint_gcd_test, 1},
    {"bigint_lcm_test", &bigint_lcm_test, 1},
    {"bigint_inverse_mod_test", &bigint_inverse_mod_test, 1},
    {"bigint_mul_mod_test", &bigint_mul_mod_test, 1},
    {"bigint_square_mod_test", &bigint_square_mod_test, 1},
    {"bigint_expt_mod_test", &bigint_expt_mod_test, 1},
    {"bigint_is_bit_set_test", &bigint_is_bit_set_test, 1},
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
    {"AES_GCM_test", &AES_GCM_test, 1},
    {"HMAC_SHA256_test", &hmac_test, 1},
};

// --- Test Runners ---

TestStats run_bigint_test(){
    TestStats stats = {"Bigint", NUM_BIGINT_TEST, 0, 0, 0};
    if (stats.total == 0) return stats;

    printf("========== bigint library tests ==========\n");
    for(int i = 0; i < stats.total; i++){
        if(bigint_test[i].enabled){
            print_msg(YELLOW, bigint_test[i].name);
            int failed = bigint_test[i].unit_test_fn();
            stats.failed += failed;
            if (failed == 0) stats.passed++;
            printf("\n");
        } else {
            stats.skipped++;
        }
    }
    return stats;
}

TestStats run_hash_test(){
    TestStats stats = {"Hash", NUM_HASH_TEST, 0, 0, 0};
    if (stats.total == 0) return stats;

    printf("========== Hashing Algorithms tests ==========\n");
    for(int i = 0; i < stats.total; i++){
        if(hash_test[i].enabled){
            print_msg(YELLOW, hash_test[i].name);
            int failed = hash_test[i].unit_test_fn();
            stats.failed += failed;
            if (failed == 0) stats.passed++;
            printf("\n");
        } else {
            stats.skipped++;
        }
    }
    return stats;
}

TestStats run_cipher_test(){
    TestStats stats = {"Cipher", NUM_CIPHER_TEST, 0, 0, 0};
    if (stats.total == 0) return stats;

    printf("========== Cipher Suite tests ==========\n");
    for(int i = 0; i < stats.total; i++){
        if(cipher_test[i].enabled){
            print_msg(YELLOW, cipher_test[i].name);
            int failed = cipher_test[i].unit_test_fn();
            stats.failed += failed;
            if (failed == 0) stats.passed++;
            printf("\n");
        } else {
            stats.skipped++;
        }
    }
    return stats;
}

// --- Utilities ---

void print_category_summary(TestStats stats) {
    if (stats.total == 0) return;
    printf("%-10s | Total: %-3d | Passed: %-3d | Failed: %-3d | Skipped: %-3d\n", 
           stats.category_name, stats.total, stats.passed, stats.failed, stats.skipped);
}

void print_test_list() {
    printf("========== Configured Tests ==========\n");
    printf("[*] = Enabled, [ ] = Disabled\n");
    
    printf("\n--- Bigint Tests (b) ---\n");
    for(int i = 0; i < NUM_BIGINT_TEST; i++)
        printf("[%c] %s\n", bigint_test[i].enabled ? '*' : ' ', bigint_test[i].name);
    
    printf("\n--- Hash Tests (ha) ---\n");
    for(int i = 0; i < NUM_HASH_TEST; i++)
        printf("[%c] %s\n", hash_test[i].enabled ? '*' : ' ', hash_test[i].name);
    
    printf("\n--- Cipher Tests (c) ---\n");
    for(int i = 0; i < NUM_CIPHER_TEST; i++)
        printf("[%c] %s\n", cipher_test[i].enabled ? '*' : ' ', cipher_test[i].name);
    printf("======================================\n");
}

void print_help(const char* prog_name) {
    printf("Usage: %s [category] [test_name]\n\n", prog_name);
    printf("Categories / Commands:\n");
    printf("  b         : Run all active Bigint tests\n");
    printf("  ha        : Run all active Hashing algorithm tests\n");
    printf("  c         : Run all active Cipher suite tests\n");
    printf("  list      : Display all configured tests and their status (* = enabled)\n");
    printf("  h, help   : Show this help menu\n\n");
    printf("Examples:\n");
    printf("  %s                   (Runs all active tests across all suites)\n", prog_name);
    printf("  %s b                 (Runs all active Bigint tests)\n", prog_name);
    printf("  %s c chacha20_test   (Runs only 'chacha20_test' within the Cipher suite)\n", prog_name);
    printf("  %s list              (Shows which tests are enabled or disabled)\n", prog_name);
}

int main(int argc, char *argv[]){
    TestStats s_bigint = {0}, s_hash = {0}, s_cipher = {0};
    int run_all = 0;

    if (argc == 1){
        printf("Running all active tests\n\n");
        run_all = 1;
        s_bigint = run_bigint_test();
        s_hash = run_hash_test();
        s_cipher = run_cipher_test();
    } 
    else if (argc == 2){
        if (!strcmp(argv[1], "h") || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help") || !strcmp(argv[1], "help")) {
            print_help(argv[0]);
            return 0;
        } else if (!strcmp(argv[1], "list")) {
            print_test_list();
            return 0;
        } else if (!strcmp(argv[1], "b")){
            printf("Running bigint tests\n\n");
            s_bigint = run_bigint_test();
        } else if (!strcmp(argv[1], "ha")){
            printf("Running hashing algorithms tests\n\n");
            s_hash = run_hash_test();
        } else if (!strcmp(argv[1], "c")){
            printf("Running cipher suite tests\n\n");
            s_cipher = run_cipher_test();
        } else {
            printf("Unrecognized arg: %s\n", argv[1]);
            printf("Use '%s h' for help.\n", argv[0]);
            return 1;
        }
    } 
    else if (argc == 3){
        if(!strcmp(argv[1], "b")){
            for(int i = 0; i < NUM_BIGINT_TEST; i++){
                if(!strcmp(argv[2], bigint_test[i].name)){
                    bigint_test[i].unit_test_fn();
                    printf("\n");
                    return 0;
                }
            }
            printf("Test '%s' not found in bigint\n", argv[2]);
            return 1;
        }
        else if(!strcmp(argv[1], "ha")){
            for(int i = 0; i < NUM_HASH_TEST; i++){
                if(!strcmp(argv[2], hash_test[i].name)){
                    hash_test[i].unit_test_fn();
                    printf("\n");
                    return 0;
                }
            }
            printf("Test '%s' not found in hash\n", argv[2]);
            return 1;
        }
        else if(!strcmp(argv[1], "c")){
            for(int i = 0; i < NUM_CIPHER_TEST; i++){
                if(!strcmp(argv[2], cipher_test[i].name)){
                    cipher_test[i].unit_test_fn();
                    printf("\n");
                    return 0;
                }
            }
            printf("Test '%s' not found in cipher\n", argv[2]);
            return 1;
        } else {
            printf("Unrecognized category arg: %s\n", argv[1]);
            printf("Use '%s h' for help.\n", argv[0]);
            return 1;
        }
    } else {
        printf("Too many arguments.\n");
        printf("Use '%s h' for help.\n", argv[0]);
        return 1;
    }
    
    // Print Global Summary
    if (argc == 1 || argc == 2) {
        printf("\n========== Final Test Summary ==========\n");
        print_category_summary(s_bigint);
        print_category_summary(s_hash);
        print_category_summary(s_cipher);
        
        if (run_all) {
            int tot = s_bigint.total + s_hash.total + s_cipher.total;
            int pass = s_bigint.passed + s_hash.passed + s_cipher.passed;
            int fail = s_bigint.failed + s_hash.failed + s_cipher.failed;
            int skip = s_bigint.skipped + s_hash.skipped + s_cipher.skipped;
            
            printf("----------------------------------------\n");
            printf("%-10s | Total: %-3d | Passed: %-3d | Failed: %-3d | Skipped: %-3d\n", 
                   "GLOBAL", tot, pass, fail, skip);
        }
        printf("========================================\n");
    }

    return 0;
}