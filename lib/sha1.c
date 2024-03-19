#include "sha1.h"
#include <string.h>

/*
    https://public.websites.umich.edu/~x509/ssleay/fip180/fip180-1.htm
*/

#define SHA1_CIRCULAR_SHIFT(word, bits) ((word << bits) | ((word & 0xFFFFFFFF) >> (32 - bits))) 

// logical functions used for SHA1, see FIPS 180-1, section 5
#define F(X, Y, Z) ((X & Y) | ((~X) & Z))
#define H(X, Y, Z) (X ^ Y ^ Z)
#define G(X, Y, Z) ((X & Y) | (X & Z) | (Y & Z))

// constants defined for SHA 1, see FIPS 180-1, section 6
#define K1 0x5A827999
#define K2 0x6ED9EBA1
#define K3 0x8F1BBCDC
#define K4 0xCA62C1D6

static const unsigned char pad_bytes[] = {
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


crypt_status sha1_init(sha1_ctx *ctx){
    if(!ctx) return CRYPT_NULL_PTR;

    ctx->bit_count = 0;

    // magic numbers from RFC 3174
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;

    return CRYPT_OKAY;
}

void sha1_process_buffer(sha1_ctx *ctx, const unsigned char *block){

    int i;                      // loop counter
    uint32_t tmp;               // tmp word value
    uint32_t W[80];             // word sequence
    uint32_t A, B, C, D, E;     // word buffers

    // initializing the first 16 words in the array W
    uint32_t *block_32 = (uint32_t *)block;
    for(i = 0; i < 16; i++){
        W[i] = LE32TOBE32(block_32[i]);
    }
    
    uint32_t wxor; 
    for(i = 16; i < 80; i++){
        wxor = W[i-3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16];
        W[i] = SHA1_CIRCULAR_SHIFT(wxor, 1);
    }

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];

    for(i = 0; i < 20; i++){
        tmp = SHA1_CIRCULAR_SHIFT(A, 5) + F(B, C, D) + E + W[i] + K1;
        E = D;
        D = C;
        C = SHA1_CIRCULAR_SHIFT(B, 30);
        B = A;
        A = tmp;
    }

    for(i = 20; i < 40; i++){
        tmp = SHA1_CIRCULAR_SHIFT(A, 5) + H(B, C, D) + E + W[i] + K2;
        E = D;
        D = C;
        C = SHA1_CIRCULAR_SHIFT(B, 30);
        B = A;
        A = tmp;
    }

    for(i = 40; i < 60; i++){
        tmp = SHA1_CIRCULAR_SHIFT(5, A) + G(B, C, D) + E + W[i] + K3;
        E = D;
        D = C;
        C = SHA1_CIRCULAR_SHIFT(B, 30);
        B = A;
        A = tmp;
    }

    for(i = 60; i < 80; i++){
        tmp = SHA1_CIRCULAR_SHIFT(5, A) + H(B, C, D) + E + W[i] + K4;
        E = D;
        D = C;
        C = SHA1_CIRCULAR_SHIFT(B, 30);
        B = A;
        A = tmp;
    }

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
    ctx->state[4] += E;

    // zero the local buffer to avoid leaking sensitive information
    memset(W, 0, sizeof(W));
}

crypt_status sha1_update(sha1_ctx *ctx, const unsigned char *input, unsigned int input_len){
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
    partial_len = SHA1_MEG_BLOCK_LEN_BYTES - index;
    // Transform the state for as many times as possible until we have 
    // an incomplete block
    // if we can form a full block
    if(input_len >= partial_len){
        // complete the first block and apply the transformation
        memcpy(&ctx->buffer[index], input, partial_len);
        sha1_process_buffer(ctx, ctx->buffer);
        i = partial_len;
        input_len -= partial_len;

        while(input_len >= SHA1_MEG_BLOCK_LEN_BYTES){
            sha1_process_buffer(ctx, &input[i]);
            input_len -= SHA1_MEG_BLOCK_LEN_BYTES;
            i += SHA1_MEG_BLOCK_LEN_BYTES;
        }
    } 
    // buffer the remaining input
    if (input_len > 0) {
        memcpy(ctx->buffer, &input[i], input_len);
    }

    return CRYPT_OKAY;
}

crypt_status sha1_finish(sha1_ctx *ctx, unsigned char digest[20], unsigned int digest_len){
    if(!ctx || !digest) return CRYPT_NULL_PTR;

    if(digest_len < 20){
        return CRYPT_BAD_BUFFER_LEN;
    }

    unsigned int index, pad_len;

    // apply the padding
    // index = num_bytes mod 64
    index = (unsigned int)((ctx->bit_count >> 3) & 0x3f); 
    // pad the message with a bit of 1, then 0s until the size of
    // the message in bit in congruent to 448 (mod 512)
    pad_len = (index < 56) ? (56 - index) : (120 - index);
    memcpy(&ctx->buffer[index], pad_bytes, pad_len);
    
    index += pad_len;
    
    // append the message length as 64-bit value
    uint64_t bit_count_be = LE64TOBE64(ctx->bit_count);
    memcpy(&ctx->buffer[index], &bit_count_be, 8);
    sha1_process_buffer(ctx, ctx->buffer);
    
    // generate 20-byte or 160-bit digest
    uint32_t state_le[5];
    state_le[0] = LE32TOBE32(ctx->state[0]);
    state_le[1] = LE32TOBE32(ctx->state[1]);
    state_le[2] = LE32TOBE32(ctx->state[2]);
    state_le[3] = LE32TOBE32(ctx->state[3]);
    state_le[4] = LE32TOBE32(ctx->state[4]);
    memcpy(digest, state_le, SHA1_DIGEST_LEN_BYTES);
    
    memset(ctx, 0, sizeof(*ctx));
    return CRYPT_OKAY;
}
