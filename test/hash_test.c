#include "../lib/crypto_api.h"
#include "test_util.h"
#include "../util/tools.h"
#include <stdio.h>

int md5_test(){
    int failed = 0;

    md5_ctx ctx = {0};
    unsigned char digest[16] = {0};
    const unsigned char a1[] = {0};
    const unsigned char d1[] = {
        0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 
        0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e
    };

    md5(&ctx, a1, sizeof(a1), digest, sizeof(digest));
    if(memcmp(digest, d1, sizeof(d1))){
        print_failed("MD5 test 1 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d1, sizeof(d1), "expected digest = ");
        failed = 1;
    } else {
        print_passed("MD5 test 1 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }
    return failed;
}
