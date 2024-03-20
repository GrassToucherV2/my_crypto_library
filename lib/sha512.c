#include "sha512.h"
#include "sha256.h"
#include <string.h>
#include "../util/tools.h"

/*
    FIPS PUB 180-4: https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf
    RFC 6234: https://www.ietf.org/rfc/rfc6234.txt
    This file implements the following:
        - SHA-512
        - SHA-512/224
        - SHA-512/256
        - SHA-384
*/

// SHA512 functions specified in FIPS PUB 180-4
#define SHR(word, bits) (word >> bits)
#define ROTR(word, bits) ((word >> bits) | (word << (64 - bits)))
#define CH(X, Y, Z) ((X & Y) ^ (~X & Z))
#define MAJ(X, Y, Z) ((X & Y) ^ (X & Z) ^ (Y & Z))
#define SHA512_SIGMA0(word) ((ROTR(word, 28)) ^ (ROTR(word, 34)) ^ (ROTR(word, 39)))
#define SHA512_SIGMA1(word) ((ROTR(word, 14)) ^ (ROTR(word, 18)) ^ (ROTR(word, 41)))
#define SHA512_sigma0(word) ((ROTR(word, 1)) ^ (ROTR(word, 8)) ^ (SHR(word, 7)))
#define SHA512_sigma1(word) ((ROTR(word, 19)) ^ (ROTR(word, 61)) ^ (SHR(word, 6)))

// SHA512 and SHA384 constants specified in FIPS PUB 180-4 section 5.3
static const uint64_t sha512_H[8] = {
    0x6a09e667f3bcc908, 
    0xbb67ae8584caa73b, 
    0x3c6ef372fe94f82b, 
    0xa54ff53a5f1d36f1, 
    0x510e527fade682d1, 
    0x9b05688c2b3e6c1f, 
    0x1f83d9abfb41bd6b, 
    0x5be0cd19137e2179
};

static const uint64_t sha512_224_H[8] = {
    0x8C3D37C819544DA2, 
    0x73E1996689DCD4D6, 
    0x1DFAB7AE32FF9C82, 
    0x679DD514582F9FCF, 
    0x0F6D2B697BD44DA8, 
    0x77E36F7304C48942, 
    0x3F9D85A86A1D36C8, 
    0x1112E6AD91D692A1
};

static const uint64_t sha512_256_H[8] = {
    0x22312194FC2BF72C, 
    0x9F555FA3C84C64C2, 
    0x2393B86B6F53B151, 
    0x963877195940EABD, 
    0x96283EE2A88EFFE3, 
    0xBE5E1E2553863992, 
    0x2B0199FC2C85B8AA, 
    0x0EB72DDC81C52CA2
};

static const uint64_t sha384_H[8] = {
    0xcbbb9d5dc1059ed8, 
    0x629a292a367cd507, 
    0x9159015a3070dd17, 
    0x152fecd8f70e5939, 
    0x67332667ffc00b31, 
    0x8eb44a8768581511, 
    0xdb0c2e0d64f98fa7, 
    0x47b5481dbefa4fa4
};

crypt_status sha384_512_init(sha384_512_ctx *ctx, SHA2 sha){
    if(!ctx) return CRYPT_NULL_PTR;

    if(sha == SHA384){
        ctx->state[0] = sha384_H[0];
        ctx->state[1] = sha384_H[1];
        ctx->state[2] = sha384_H[2];
        ctx->state[3] = sha384_H[3];
        ctx->state[4] = sha384_H[4];
        ctx->state[5] = sha384_H[5];
        ctx->state[6] = sha384_H[6];
        ctx->state[7] = sha384_H[7];
    } 
    else if(sha == SHA512){
        ctx->state[0] = sha512_H[0];
        ctx->state[1] = sha512_H[1];
        ctx->state[2] = sha512_H[2];
        ctx->state[3] = sha512_H[3];
        ctx->state[4] = sha512_H[4];
        ctx->state[5] = sha512_H[5];
        ctx->state[6] = sha512_H[6];
        ctx->state[7] = sha512_H[7];
    } 
    else if(sha == SHA512_224){
        ctx->state[0] = sha512_224_H[0];
        ctx->state[1] = sha512_224_H[1];
        ctx->state[2] = sha512_224_H[2];
        ctx->state[3] = sha512_224_H[3];
        ctx->state[4] = sha512_224_H[4];
        ctx->state[5] = sha512_224_H[5];
        ctx->state[6] = sha512_224_H[6];
        ctx->state[7] = sha512_224_H[7];
    }
    else if(sha == SHA512_256){
        ctx->state[0] = sha512_256_H[0];
        ctx->state[1] = sha512_256_H[1];
        ctx->state[2] = sha512_256_H[2];
        ctx->state[3] = sha512_256_H[3];
        ctx->state[4] = sha512_256_H[4];
        ctx->state[5] = sha512_256_H[5];
        ctx->state[6] = sha512_256_H[6];
        ctx->state[7] = sha512_256_H[7];
    }
    else {
        return CRYPT_UNSUPPORTED_ALGO;
    }
    
    ctx->bit_count = 0;
    memset(ctx->buffer, 0, sizeof(ctx->buffer));

    return CRYPT_OKAY;
}

// constants defined in section 4.2.3, FIPS 180-4
static const uint64_t K[80] = {
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538, 
    0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe, 
    0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 
    0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab, 
    0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725, 
    0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 
    0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b, 
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218, 
    0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 
    0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 
    0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec, 
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c, 
    0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6, 
    0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 
    0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

// performs 80 rounds of transformation
static void sha384_512_process_block(sha384_512_ctx *ctx, const uint8_t *block){
    int t;                               // loop counter
    uint64_t tmp1, tmp2;                 // tmp word value
    uint64_t W[80];                      // word sequence
    uint64_t A, B, C, D, E, F, G, H;     // word buffers

    uint64_t *block_64 = (uint64_t *)block;

    // step 1
    for(t = 0; t < 16; t++){
        W[t] = LE64TOBE64(block_64[t]);
    }

    for(t = 16; t < 80; t++){
        W[t] = SHA512_sigma1(W[t-2]) + W[t-7] + SHA512_sigma0(W[t-15]) + W[t-16];
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
    for(t = 0; t < 80; t++){
        tmp1 = H + SHA512_SIGMA1(E) + CH(E,F,G) + K[t] + W[t];
        tmp2 = SHA512_SIGMA0(A) + MAJ(A,B,C);
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

crypt_status sha384_512_update(sha384_512_ctx *ctx, const unsigned char *input, unsigned int input_len){
    if(!ctx || !input) return CRYPT_NULL_PTR;

    if(input_len == 0){
        return CRYPT_OKAY;
    }

    unsigned int index = 0; 
    unsigned int i = 0; 
    unsigned int partial_len = 0;

    // compute number of bytes mod 128
    // >> 3 is equivalent to integer dividing by 8
    // & 0x80 == 128
    // index counts byte because the buffer is unsigned char array
    index = (ctx->bit_count >> 3) & 0x7F;  
    // bit_count is the total number of bits processed, used in md5_final
    // to ensure proper padding and complete the MD5 digest
    ctx->bit_count += (input_len << 3);
    // using partial_len in case there are already things in the context buffer
    partial_len = SHA512_MSG_BLOCK_LEN_BYTES - index;
    // Transform the state for as many times as possible until we have 
    // an incomplete block
    // if we can form a full block
    if(input_len >= partial_len){
        // complete the first block and apply the transformation
        memcpy(&ctx->buffer[index], input, partial_len);
        sha384_512_process_block(ctx, ctx->buffer);
        i = partial_len;
        input_len -= partial_len;

        while(input_len >= SHA512_MSG_BLOCK_LEN_BYTES){
            sha384_512_process_block(ctx, &input[i]);
            input_len -= SHA512_MSG_BLOCK_LEN_BYTES;
            i += SHA512_MSG_BLOCK_LEN_BYTES;
        }
    } 
    // buffer the remaining input
    if (input_len > 0) {
        memcpy(ctx->buffer, &input[i], input_len);
    }

    return CRYPT_OKAY;
}

crypt_status sha384_512_finish(sha384_512_ctx *ctx, unsigned char *digest, 
                                unsigned int digest_len, SHA2 sha)
{
    if(!ctx || !digest) return CRYPT_NULL_PTR;

    if(sha == SHA512){
        if(digest_len < SHA512_DIGEST_LEN_BYTES) return CRYPT_BAD_BUFFER_LEN;
    } 
    else if(sha == SHA384){
        if(digest_len < SHA384_DIGEST_LEN_BYTES) return CRYPT_BAD_BUFFER_LEN;
    } 
    else if(sha == SHA512_256){
        if(digest_len < SHA256_DIGEST_LEN_BYTES) return CRYPT_BAD_BUFFER_LEN;
    }
    else if(sha == SHA512_224){
        if(digest_len < SHA224_DIGEST_LEN_BYTES) return CRYPT_BAD_BUFFER_LEN;
    }
    
    unsigned int index = (unsigned int)((ctx->bit_count >> 3) & 0x7F);

    // apply the padding
    // index = num_bytes mod 128
    ctx->buffer[index] = 0x80;
    index++; 

    // if there is no more room for appending the length, we pad it with 0
    // and then process it, then prepare a new block with the first 56 bytes 
    // being only 0
    if(index >= 120){    
        memset(&ctx->buffer[index], 0, SHA512_MSG_BLOCK_LEN_BYTES - index);
        sha384_512_process_block(ctx, ctx->buffer);
        memset(ctx->buffer, 0, 120);
    } else {
        memset(&ctx->buffer[index], 0, 120 - index);
    } 

    // append the message length as 64-bit value at index 56
    uint64_t bit_count_be = LE64TOBE64(ctx->bit_count);
    memcpy(&ctx->buffer[120], &bit_count_be, 8);
    sha384_512_process_block(ctx, ctx->buffer);
    
    
    // generate 32-byte digest for SHA-256 or 28-byte digest for SHA-224
    uint64_t state_le[8];
    state_le[0] = LE64TOBE64(ctx->state[0]);
    state_le[1] = LE64TOBE64(ctx->state[1]);
    state_le[2] = LE64TOBE64(ctx->state[2]);
    state_le[3] = LE64TOBE64(ctx->state[3]);
    state_le[4] = LE64TOBE64(ctx->state[4]);
    state_le[5] = LE64TOBE64(ctx->state[5]);
    state_le[6] = LE64TOBE64(ctx->state[6]);
    state_le[7] = LE64TOBE64(ctx->state[7]);
    if(sha == SHA512){
        memcpy(digest, state_le, SHA512_DIGEST_LEN_BYTES);
    }
    else if(sha == SHA384){
        memcpy(digest, state_le, SHA384_DIGEST_LEN_BYTES);
    }
    else if(sha == SHA512_256){
        memcpy(digest, state_le, SHA256_DIGEST_LEN_BYTES);
    }
    else if(sha == SHA512_224){
        memcpy(digest, state_le, SHA224_DIGEST_LEN_BYTES);
    }
    memset(ctx, 0, sizeof(*ctx));
    return CRYPT_OKAY;
}
