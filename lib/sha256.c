#include "sha256.h"
#include <string.h>
#include "../util/tools.h"

/*
    FIPS PUB 180-4: https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf
    RFC 6234: https://www.ietf.org/rfc/rfc6234.txt
    This file implements the following:
        - SHA-224
        - SHA-256
*/

// SHA224 and SHA256 functions specified in FIPS PUB 180-4
#define SHR(word, bits) (word >> bits)
#define ROTR(word, bits) ((word >> bits) | (word << (32 - bits)))
#define CH(X, Y, Z) ((X & Y) ^ (~X & Z))
#define MAJ(X, Y, Z) ((X & Y) ^ (X & Z) ^ (Y & Z))
#define SHA256_SIGMA0(word) ((ROTR(word, 2)) ^ (ROTR(word, 13)) ^ (ROTR(word, 22)))
#define SHA256_SIGMA1(word) ((ROTR(word, 6)) ^ (ROTR(word, 11)) ^ (ROTR(word, 25)))
#define SHA256_sigma0(word) ((ROTR(word, 7)) ^ (ROTR(word, 18)) ^ (SHR(word, 3)))
#define SHA256_sigma1(word) ((ROTR(word, 17)) ^ (ROTR(word, 19)) ^ (SHR(word, 10)))

// SHA224 and SHA256 constants specified in FIPS PUB 180-4
static const uint32_t sha256_H[8] = {
    0x6a09e667,
    0xbb67ae85,
    0x3c6ef372,
    0xa54ff53a,
    0x510e527f,
    0x9b05688c,
    0x1f83d9ab,
    0x5be0cd19
};

static const uint32_t sha224_H[8] = {
    0xc1059ed8,
    0x367cd507,
    0x3070dd17,
    0xf70e5939,
    0xffc00b31,
    0x68581511,
    0x64f98fa7,
    0xbefa4fa4
};

crypt_status sha224_256_init(sha224_256_ctx *ctx, SHA2 sha){
    if(!ctx) return CRYPT_NULL_PTR;

    if(sha == SHA224){
        ctx->state[0] = sha224_H[0];
        ctx->state[1] = sha224_H[1];
        ctx->state[2] = sha224_H[2];
        ctx->state[3] = sha224_H[3];
        ctx->state[4] = sha224_H[4];
        ctx->state[5] = sha224_H[5];
        ctx->state[6] = sha224_H[6];
        ctx->state[7] = sha224_H[7];
    } 
    else if(sha == SHA256){
        ctx->state[0] = sha256_H[0];
        ctx->state[1] = sha256_H[1];
        ctx->state[2] = sha256_H[2];
        ctx->state[3] = sha256_H[3];
        ctx->state[4] = sha256_H[4];
        ctx->state[5] = sha256_H[5];
        ctx->state[6] = sha256_H[6];
        ctx->state[7] = sha256_H[7];
    } 
    else {
        return CRYPT_UNSUPPORTED_ALGO;
    }
    
    ctx->bit_count = 0;
    memset(ctx->buffer, 0, sizeof(ctx->buffer));

    return CRYPT_OKAY;
}

static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b,
    0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
    0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7,
    0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152,
    0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
    0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
    0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08,
    0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
    0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void sha224_256_process_block(sha224_256_ctx *ctx, const uint8_t *block){
    int t;                               // loop counter
    uint32_t tmp1, tmp2;                 // tmp word value
    uint32_t W[64];                      // word sequence
    uint32_t A, B, C, D, E, F, G, H;     // word buffers

    // prepare W arraym FIPS PUB 180-4 6.2.2 step 1
    uint32_t *block_32 = (uint32_t *)block;
    for(t = 0; t < 16; t++){
        W[t] = LE32TOBE32(block_32[t]);
    }

    for(t = 16; t < 64; t++){
        W[t] = SHA256_sigma1(W[t-2]) + W[t-7] + SHA256_sigma0(W[t - 15]) + W[t-16];
    }

    // step 2
    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];
    F = ctx->state[5];
    G = ctx->state[6];
    H = ctx->state[7];

    // step 3
    for (t = 0; t < 64; t++) {
        tmp1 = H + SHA256_SIGMA1(E) + CH(E,F,G) + K[t] + W[t];
        tmp2 = SHA256_SIGMA0(A) + MAJ(A,B,C);
        H = G;
        G = F;
        F = E;
        E = D + tmp1;
        D = C;
        C = B;
        B = A;
        A = tmp1 + tmp2;
    }

    // step 4
    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
    ctx->state[4] += E;
    ctx->state[5] += F;
    ctx->state[6] += G;
    ctx->state[7] += H;

    memset(W, 0, sizeof(W));
}

crypt_status sha224_256_update(sha224_256_ctx *ctx, const unsigned char *input, unsigned int input_len){
    if(!ctx || !input) return CRYPT_NULL_PTR;

    if(input_len == 0){
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
    partial_len = SHA2_MSG_BLOCK_LEN_BYTES - index;
    // Transform the state for as many times as possible until we have 
    // an incomplete block
    // if we can form a full block
    if(input_len >= partial_len){
        // complete the first block and apply the transformation
        memcpy(&ctx->buffer[index], input, partial_len);
        sha224_256_process_block(ctx, ctx->buffer);
        i = partial_len;
        input_len -= partial_len;

        while(input_len >= SHA2_MSG_BLOCK_LEN_BYTES){
            sha224_256_process_block(ctx, &input[i]);
            input_len -= SHA2_MSG_BLOCK_LEN_BYTES;
            i += SHA2_MSG_BLOCK_LEN_BYTES;
        }
    } 
    // buffer the remaining input
    if (input_len > 0) {
        memcpy(ctx->buffer, &input[i], input_len);
    }

    return CRYPT_OKAY;
}

crypt_status sha224_256_finish(sha224_256_ctx *ctx, unsigned char *digest, 
                                unsigned int digest_len, SHA2 sha)
{
    if(!ctx || !digest) return CRYPT_NULL_PTR;

    if(sha == SHA256){
        if(digest_len < SHA256_DIGEST_LEN_BYTES) return CRYPT_BAD_BUFFER_LEN;
    } 
    else if(sha == SHA224){
        if(digest_len < SHA224_DIGEST_LEN_BYTES) return CRYPT_BAD_BUFFER_LEN;
    } 
    else {
        return CRYPT_UNSUPPORTED_ALGO;
    }

    unsigned int index = (unsigned int)((ctx->bit_count >> 3) & 0x3f);

    // apply the padding
    // index = num_bytes mod 64
    ctx->buffer[index] = 0x80;
    index++; 

    // if there is no more room for appending the length, we pad it with 0
    // and then process it, then prepare a new block with the first 56 bytes 
    // being only 0
    if(index >= 56){    
        memset(&ctx->buffer[index], 0, SHA2_MSG_BLOCK_LEN_BYTES - index);
        sha224_256_process_block(ctx, ctx->buffer);
        memset(ctx->buffer, 0, 56);
    } else {
        memset(&ctx->buffer[index], 0, 56 - index);
    } 

    // append the message length as 64-bit value at index 56
    uint64_t bit_count_be = LE64TOBE64(ctx->bit_count);
    memcpy(&ctx->buffer[56], &bit_count_be, 8);
    sha224_256_process_block(ctx, ctx->buffer);
    
    
    // generate 32-byte digest for SHA-256 or 28-byte digest for SHA-224
    uint32_t state_le[8];
    state_le[0] = LE32TOBE32(ctx->state[0]);
    state_le[1] = LE32TOBE32(ctx->state[1]);
    state_le[2] = LE32TOBE32(ctx->state[2]);
    state_le[3] = LE32TOBE32(ctx->state[3]);
    state_le[4] = LE32TOBE32(ctx->state[4]);
    state_le[5] = LE32TOBE32(ctx->state[5]);
    state_le[6] = LE32TOBE32(ctx->state[6]);
    state_le[7] = LE32TOBE32(ctx->state[7]);
    if(sha == SHA256){
        memcpy(digest, state_le, SHA256_DIGEST_LEN_BYTES);
    }
    else if(sha == SHA224){
        memcpy(digest, state_le, SHA224_DIGEST_LEN_BYTES);
    }
    
    memset(ctx, 0, sizeof(*ctx));
    return CRYPT_OKAY;
}