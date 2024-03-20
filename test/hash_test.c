#include "../lib/crypto_api.h"
#include "test_util.h"
#include "../util/tools.h"
#include <stdio.h>

// test vectors from RFC 1321
// https://www.ietf.org/rfc/rfc1321.txt
int md5_test(){
    int failed = 0;

    unsigned char digest[16] = {0};
    const unsigned char a1[] = {""};
    const unsigned char d1[] = {
        0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 
        0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e
    };

    // sizeof(a1) - 1 to remove additional byte due to null char at the end 
    crypt_md5(a1, sizeof(a1) - 1, digest, sizeof(digest));
    if(memcmp(digest, d1, sizeof(d1))){
        print_failed("MD5 test 1 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d1, sizeof(d1), "expected digest = ");
        failed = 1;
    } else {
        print_passed("MD5 test 1 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }

    const unsigned char a2[] = {"a"};
    const unsigned char d2[] = {
        0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8, 
        0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61
    };
    crypt_md5(a2, sizeof(a2) - 1, digest, sizeof(digest));
    if(memcmp(digest, d2, sizeof(d2))){
        print_failed("MD5 test 2 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d2, sizeof(d2), "expected digest = ");
        failed = 1;
    } else {
        print_passed("MD5 test 2 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }

    const unsigned char a3[] = {"abc"};
    const unsigned char d3[] = {
        0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 
        0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72
    };
    crypt_md5(a3, sizeof(a3) - 1, digest, sizeof(digest));
    if(memcmp(digest, d3, sizeof(d3))){
        print_failed("MD5 test 3 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d3, sizeof(d3), "expected digest = ");
        failed = 1;
    } else {
        print_passed("MD5 test 3 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }

    const unsigned char a4[] = {"message digest"};
    const unsigned char d4[] = {
        0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d, 
        0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0
    };
    crypt_md5(a4, sizeof(a4) - 1, digest, sizeof(digest));
    if(memcmp(digest, d4, sizeof(d4))){
        print_failed("MD5 test 4 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d4, sizeof(d4), "expected digest = ");
        failed = 1;
    } else {
        print_passed("MD5 test 4 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }

    const unsigned char a5[] = {"abcdefghijklmnopqrstuvwxyz"};
    const unsigned char d5[] = {
        0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00, 
        0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b
    };
    crypt_md5(a5, sizeof(a5) - 1, digest, sizeof(digest));
    if(memcmp(digest, d5, sizeof(d5))){
        print_failed("MD5 test 5 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d5, sizeof(d5), "expected digest = ");
        failed = 1;
    } else {
        print_passed("MD5 test 5 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }

    const unsigned char a6[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"};
    const unsigned char d6[] = {
        0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5, 
        0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f
    };
    crypt_md5(a6, sizeof(a6) - 1, digest, sizeof(digest));
    if(memcmp(digest, d6, sizeof(d6))){
        print_failed("MD5 test 6 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d6, sizeof(d6), "expected digest = ");
        failed = 1;
    } else {
        print_passed("MD5 test 6 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }

    const unsigned char a7[] = {"12345678901234567890123456789012345678901234567890123456789012345678901234567890"};
    const unsigned char d7[] = {
        0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55, 
        0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a
    };
    crypt_md5(a7, sizeof(a7) - 1, digest, sizeof(digest));
    if(memcmp(digest, d7, sizeof(d7))){
        print_failed("MD5 test 7 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d7, sizeof(d7), "expected digest = ");
        failed = 1;
    } else {
        print_passed("MD5 test 7 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }

    const unsigned char a8[] = {
        0xF9, 0xF7, 0xC5, 0xA3, 0xDF, 0x15, 0x94, 0xAA,
        0x82, 0x6C, 0x55, 0x3A, 0xAB, 0x98, 0xE4, 0xC3,
        0x28, 0xA6, 0x86, 0x29, 0x81, 0xAA, 0x3B, 0xCA,
        0x69, 0x4E, 0x4C, 0x5E, 0x2D, 0xE6, 0x00, 0xE0,
        0x8E, 0x94, 0xE3, 0xCE, 0x96, 0x05, 0xFB, 0x9F,
        0x97, 0xA8, 0x1B, 0xD2, 0x76, 0xAB, 0x7D, 0x64,
        0x99, 0xF5, 0xC4, 0x31, 0x53, 0xC7, 0x77, 0xDC,
        0xA1, 0x64, 0x7C, 0x7F, 0x87, 0x36, 0x9C, 0xA3
    };
    const unsigned char d8[] = {
        0x4D, 0xC7, 0x4A, 0xDE, 0xF2, 0x28, 0xFB, 0x19,
        0xE1, 0xDB, 0x79, 0xBD, 0x05, 0xA4, 0xAE, 0x3C
    };
    crypt_md5(a8, sizeof(a8), digest, sizeof(digest));
    if(memcmp(digest, d8, sizeof(d8))){
        print_failed("MD5 test 8 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d8, sizeof(d8), "expected digest = ");
        failed = 1;
    } else {
        print_passed("MD5 test 8 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }

    // MD5 collision - https://twitter.com/realhashbreaker/status/1770161965006008570
    printf("\n\nHere is a MD5 collision using 72-byte alphanum with only 1 byte difference\n\n");
    const unsigned char a9[] = {"TEXTCOLLBYfGiJUETHQ4hAcKSMd5zYpgqf1YRDhkmxHkhPWptrkoyz28wnI9V0aHeAuaKnak"};
    const unsigned char a10[] = {"TEXTCOLLBYfGiJUETHQ4hEcKSMd5zYpgqf1YRDhkmxHkhPWptrkoyz28wnI9V0aHeAuaKnak"};
    unsigned char digest1[16] = {0};
    crypt_md5(a9, sizeof(a9) - 1, digest, sizeof(digest));
    crypt_md5(a10, sizeof(a10) - 1, digest1, sizeof(digest));
    print_bytes_array(digest, sizeof(digest), "computed digest using TEXTCOLLBYfGiJUETHQ4hAcKSMd5zYpgqf1YRDhkmxHkhPWptrkoyz28wnI9V0aHeAuaKnak");
    print_bytes_array(digest1, sizeof(digest1), "computed digest using TEXTCOLLBYfGiJUETHQ4hEcKSMd5zYpgqf1YRDhkmxHkhPWptrkoyz28wnI9V0aHeAuaKnak");

    return failed;
}

// test vectors from FIPS 180-1
// https://public.websites.umich.edu/~x509/ssleay/fip180/fip180-1.htm
int sha1_test(){
    int failed = 0;
    unsigned char digest[20] = {0};

    const unsigned char a1[] = {"abc"};
    const unsigned char d1[] = {
        0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 
        0xBA, 0x3E, 0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 
        0x9C, 0xD0, 0xD8, 0x9D
    };

    // sizeof(a1) - 1 to remove additional byte due to null char at the end 
    crypt_sha1(a1, sizeof(a1) - 1, digest, sizeof(digest));
    if(memcmp(digest, d1, sizeof(d1))){
        print_failed("SHA-1 test 1 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d1, sizeof(d1), "expected digest = ");
        failed = 1;
    } else {
        print_passed("SHA-1 test 1 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }

    const unsigned char a2[] = {"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"};
    const unsigned char d2[] = {
        0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 
        0xBA, 0xAE, 0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 
        0xE5, 0x46, 0x70, 0xF1
    };

    // sizeof(a1) - 1 to remove additional byte due to null char at the end 
    crypt_sha1(a2, sizeof(a2) - 1, digest, sizeof(digest));
    if(memcmp(digest, d2, sizeof(d2))){
        print_failed("SHA-1 test 2 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d2, sizeof(d2), "expected digest = ");
        failed = 1;
    } else {
        print_passed("SHA-1 test 2 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }

    unsigned char a3[1000000];
    const unsigned char d3[] = {
        0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 
        0xF6, 0x1E, 0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 
        0x65, 0x34, 0x01, 0x6F
    };

    for(int i = 0; i < 1000000; i++){
        a3[i] = 'a';
    }

    // not sizeof(a3) - 1 here because we manually set the string
    crypt_sha1(a3, sizeof(a3), digest, sizeof(digest));
    if(memcmp(digest, d3, sizeof(d3))){
        print_failed("SHA-1 test 3 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d3, sizeof(d3), "expected digest = ");
        failed = 1;
    } else {
        print_passed("SHA-1 test 3 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }

    return failed;
}

int sha256_test(){
    int failed = 0;
    unsigned char digest[32] = {0};
    unsigned char digest_224[28] = {0};

    const unsigned char a1[] = {"abc"};
    const unsigned char d1_256[] = {
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
        0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
        0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
        0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
    };
    const unsigned char d1_224[] = {
        0x23, 0x09, 0x7d, 0x22, 0x34, 0x05, 0xd8, 0x22,
        0x86, 0x42, 0xa4, 0x77, 0xbd, 0xa2, 0x55, 0xb3,
        0x2a, 0xad, 0xbc, 0xe4, 0xbd, 0xa0, 0xb3, 0xf7,
        0xe3, 0x6c, 0x9d, 0xa7
    };

    // sizeof(a1) - 1 to remove additional byte due to null char at the end 
    crypt_sha256(a1, sizeof(a1) - 1, digest, sizeof(digest), SHA256);
    if(memcmp(digest, d1_256, sizeof(d1_256))){
        print_failed("SHA-256 test 1 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d1_256, sizeof(d1_256), "expected digest = ");
        failed = 1;
    } else {
        print_passed("SHA-256 test 1 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }
    crypt_sha256(a1, sizeof(a1) - 1, digest_224, sizeof(digest_224), SHA224);
    if(memcmp(digest_224, d1_224, sizeof(d1_224))){
        print_failed("SHA-224 test 1 - failed");
        print_bytes_array(digest_224, sizeof(digest_224), "computed digest = ");
        print_bytes_array(d1_224, sizeof(d1_224), "expected digest = ");
        failed = 1;
    } else {
        print_passed("SHA-224 test 1 - passed");
        print_bytes_array(digest_224, sizeof(digest_224), "digest = ");
    }

    const unsigned char a2[] = {"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"};
    const unsigned char d2_256[] = {
        0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
        0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
        0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
        0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
    };
    const unsigned char d2_224[] = {
        0x75, 0x38, 0x8b, 0x16, 0x51, 0x27, 0x76, 0xcc,
        0x5d, 0xba, 0x5d, 0xa1, 0xfd, 0x89, 0x01, 0x50,
        0xb0, 0xc6, 0x45, 0x5c, 0xb4, 0xf5, 0x8b, 0x19,
        0x52, 0x52, 0x25, 0x25
    };

    crypt_sha256(a2, sizeof(a2) - 1, digest, sizeof(digest), SHA256);
    if(memcmp(digest, d2_256, sizeof(d2_256))){
        print_failed("SHA-256 test 2 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d2_256, sizeof(d2_256), "expected digest = ");
        failed = 1;
    } else {
        print_passed("SHA-256 test 2 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }
    crypt_sha256(a2, sizeof(a2) - 1, digest_224, sizeof(digest_224), SHA224);
    if(memcmp(digest_224, d2_224, sizeof(d2_224))){
        print_failed("SHA-224 test 2 - failed");
        print_bytes_array(digest_224, sizeof(digest_224), "computed digest = ");
        print_bytes_array(d2_224, sizeof(d2_224), "expected digest = ");
        failed = 1;
    } else {
        print_passed("SHA-224 test 2 - passed");
        print_bytes_array(digest_224, sizeof(digest_224), "digest = ");
    }

    const unsigned char a3[] = {""};
    const unsigned char d3_256[] = {
        0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
        0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
        0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
        0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55
    };
    const unsigned char d3_224[] = {
        0xd1, 0x4a, 0x02, 0x8c, 0x2a, 0x3a, 0x2b, 0xc9,
        0x47, 0x61, 0x02, 0xbb, 0x28, 0x82, 0x34, 0xc4,
        0x15, 0xa2, 0xb0, 0x1f, 0x82, 0x8e, 0xa6, 0x2a,
        0xc5, 0xb3, 0xe4, 0x2f
    };

    crypt_sha256(a3, sizeof(a3) - 1, digest, sizeof(digest), SHA256);
    if(memcmp(digest, d3_256, sizeof(d3_256))){
        print_failed("SHA-256 test 3 - failed");
        print_bytes_array(digest, sizeof(digest), "computed digest = ");
        print_bytes_array(d3_256, sizeof(d3_256), "expected digest = ");
        failed = 1;
    } else {
        print_passed("SHA-256 test 3 - passed");
        print_bytes_array(digest, sizeof(digest), "digest = ");
    }
    crypt_sha256(a3, sizeof(a3) - 1, digest_224, sizeof(digest_224), SHA224);
    if(memcmp(digest_224, d3_224, sizeof(d3_224))){
        print_failed("SHA-224 test 3 - failed");
        print_bytes_array(digest_224, sizeof(digest_224), "computed digest = ");
        print_bytes_array(d3_224, sizeof(d3_224), "expected digest = ");
        failed = 1;
    } else {
        print_passed("SHA-224 test 3 - passed");
        print_bytes_array(digest_224, sizeof(digest_224), "digest = ");
    }

    return failed;
}
