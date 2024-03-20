#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "md5.h"
#include "../test/test_util.h"

/*
    MD5 is a hashing algorithm that played an important role in data integrity 
    and authentication over the years. However, in recent years, it has become 
    insecure, and is known to be vulnerable to collision attack and preimage attack. 
    Therefore new systems should not use MD5. Instead, they should choose more secure 
    algorithms, like SHA256

    This implementation will not work on 32-bit devices.
*/

// four words specified in RFC 1321, step 3.3
const uint32_t word_A = 0x67452301;
const uint32_t word_B = 0xefcdab89;
const uint32_t word_C = 0x98badcfe;
const uint32_t word_D = 0x10325476;

// constants for MD5 transformation routine
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

// basic MD5 functions
#define F(X, Y, Z) ((X & Y) | ((~X) & Z))
#define G(X, Y, Z) ((X & Z) | (Y & (~Z)))
#define H(X, Y, Z) (X ^ Y ^ Z)
#define I(X, Y, Z) (Y ^ (X | (~Z)))

// rotates X to the left N bits
#define ROTATE_LEFT(X, N) ((X << N) | (X >> (32-N)))

// transformations for round 1, 2, 3 and 4
#define FF(A, B, C, D, X, S, AC) do{    \
    A += F(B, C, D) + X + (uint32_t)AC; \
    A = ROTATE_LEFT(A, S);              \
    A += B;                             \
} while(0)

#define GG(A, B, C, D, X, S, AC) do{    \
    A += G(B, C, D) + X + (uint32_t)AC; \
    A = ROTATE_LEFT(A, S);              \
    A += B;                             \
} while(0)

#define HH(A, B, C, D, X, S, AC) do{    \
    A += H(B, C, D) + X + (uint32_t)AC; \
    A = ROTATE_LEFT(A, S);              \
    A += B;                             \
} while(0)

#define II(A, B, C, D, X, S, AC) do{    \
    A += I(B, C, D) + X + (uint32_t)AC; \
    A = ROTATE_LEFT(A, S);              \
    A += B;                             \
} while(0)

// 4 Rounds of MD5 transformation, updates the state based on the 64-byte block
static void md5_transformation(md5_ctx *ctx, const unsigned char *block){
    uint32_t a = ctx->state[0];
    uint32_t b = ctx->state[1];
    uint32_t c = ctx->state[2];
    uint32_t d = ctx->state[3];

    uint32_t x[16] = {0};
    memcpy(x, block, 64);

    /* Round 1 */
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

      /* Round 4 */
    II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;

    // setting sensitive information to 0 at the end of the transformation
    memset(x, 0, sizeof(x));
}

crypt_status md5_init(md5_ctx *ctx){
    if(!ctx) return CRYPT_NULL_PTR;

    ctx->bit_count = 0;

    ctx->state[0] = word_A;
    ctx->state[1] = word_B;
    ctx->state[2] = word_C;
    ctx->state[3] = word_D;

    memset(ctx->buffer, 0, sizeof(ctx->buffer));

    return CRYPT_OKAY;
}

crypt_status md5_update(md5_ctx *ctx, const unsigned char *input, unsigned int input_len){
    if(!ctx || !input) return CRYPT_NULL_PTR;

    if(input_len <= 0){
        return CRYPT_OKAY;
    }

    unsigned int index = 0; 
    unsigned int i = 0; 
    unsigned int partial_len = 0;

    // compute number of bytes mod 64
    // >> 3 is equivalent to integer dividing by 8
    // & 0x3F == &0b00111111, which is equivalent to mod 64
    // index counts byte because the buffer is unsigned char array
    index = (ctx->bit_count >> 3) & 0x3F;  
    // bit_count is the total number of bits processed, used in md5_final
    // to ensure proper padding and complete the MD5 digest
    ctx->bit_count += (input_len << 3);
    // using partial_len in case there are already things in the context buffer
    partial_len = MD5_BLOCK_SIZE_BYTES - index;
    // Transform the state for as many times as possible until we have 
    // an incomplete block
    // if we can form a full block
    if(input_len >= partial_len){
        // complete the first block and apply the transformation
        memcpy(&ctx->buffer[index], input, partial_len);
        md5_transformation(ctx, ctx->buffer);
        i = partial_len;
        input_len -= partial_len;

        while(input_len >= MD5_BLOCK_SIZE_BYTES){
            md5_transformation(ctx, &input[i]);
            input_len -= MD5_BLOCK_SIZE_BYTES;
            i += MD5_BLOCK_SIZE_BYTES;
        }
    } 
    // buffer the remaining input
    if (input_len > 0) {
        memcpy(ctx->buffer, &input[i], input_len);
    }

    return CRYPT_OKAY;
}


crypt_status md5_finish(md5_ctx *ctx, unsigned char *digest){
    if(!ctx || !digest) return CRYPT_NULL_PTR;

    // index = num_bytes mod 64
    unsigned int index = (unsigned int)((ctx->bit_count >> 3) & 0x3f);

    // apply the padding, beginning with a bit of 1 right after what's left in the buffer
    ctx->buffer[index] = 0x80;
    index++; 

    // if there is no more room for appending the length (8 bytes), we pad it with 0
    // and then process it, then prepare a new block with the first 56 bytes 
    // being only 0
    if(index >= 56){    
        memset(&ctx->buffer[index], 0, MD5_BLOCK_SIZE_BYTES - index);
        md5_transformation(ctx, ctx->buffer);
        memset(ctx->buffer, 0, 56);
    } else {
        memset(&ctx->buffer[index], 0, 56 - index);
    }

    // append the message length as 64-bit value at index 56, MD5 is done in little endian
    uint64_t bit_count_be = BE64TOLE64(ctx->bit_count);
    memcpy(&ctx->buffer[56], &bit_count_be, 8);
    md5_transformation(ctx, ctx->buffer);
    
    // generate 16-byte or 128-bit digest
    uint32_t state_le[4];
    state_le[0] = BE32TOLE32(ctx->state[0]);
    state_le[1] = BE32TOLE32(ctx->state[1]);
    state_le[2] = BE32TOLE32(ctx->state[2]);
    state_le[3] = BE32TOLE32(ctx->state[3]);
    memcpy(digest, state_le, MD5_DIGESTS_LEN_BYTES);
    
    memset(ctx, 0, sizeof(*ctx));
    return CRYPT_OKAY;
}    



