#include "chacha20.h"
#include "../util/tools.h"

#define ROTATE(v, c) ((v) << (c)) | ((v) >> (32 - (c)))
#define QUARTERROUND(a, b, c, d) \
    (a) += (b); (d) ^= (a); (d) = ROTATE((d), 16); \
    (c) += (d); (b) ^= (c); (b) = ROTATE((b), 12); \
    (a) += (b); (d) ^= (a); (d) = ROTATE((d), 8); \
    (c) += (d); (b) ^= (c); (b) = ROTATE((b), 7);

static void chacha20_quarter_round(uint32_t *state, unsigned int a, 
                                    unsigned int b, unsigned int c, unsigned int d)
{
    QUARTERROUND(state[a], state[b], state[c], state[d]);
}

static void chacha20_inner_block(uint32_t *state){
    chacha20_quarter_round(state, 0, 4, 8,  12);
    chacha20_quarter_round(state, 1, 5, 9,  13);
    chacha20_quarter_round(state, 2, 6, 10, 14);
    chacha20_quarter_round(state, 3, 7, 11, 15);

    chacha20_quarter_round(state, 0, 5, 10, 15);
    chacha20_quarter_round(state, 1, 6, 11, 12);
    chacha20_quarter_round(state, 2, 7, 8,  13);
    chacha20_quarter_round(state, 3, 4, 9,  14);
}

void chacha20_block(chacha20_ctx *ctx, unsigned char keystream[CHACHA20_KEYSTREAM_LEN_BYTES]){
    uint32_t tmp_state[16];
    for(int i = 0; i < 16; i++){
        tmp_state[i] = ctx->state[i];
    }

    // 10 rounds of inner block transformation
    for(int i = 0; i < 10; i++) {
        chacha20_inner_block(tmp_state);
    }

    for(int i = 0; i < 16; i++){
        tmp_state[i] += ctx->state[i];
    }

    // serialize - write the keystream in little endian
    for (int i = 0; i < 16; i++) {
        // Convert each 32-bit word in the state to 4 bytes in little-endian format
        keystream[i * 4] = (uint8_t)(tmp_state[i] & 0xFF);
        keystream[i * 4 + 1] = (uint8_t)((tmp_state[i] >> 8) & 0xFF);
        keystream[i * 4 + 2] = (uint8_t)((tmp_state[i] >> 16) & 0xFF);
        keystream[i * 4 + 3] = (uint8_t)((tmp_state[i] >> 24) & 0xFF);
    }
}

crypt_status chacha20_init(chacha20_ctx *ctx, const unsigned char *key)
{
    if(!ctx || !key) return CRYPT_NULL_PTR;

    ctx->state[0] = 0x61707865;
    ctx->state[1] = 0x3320646e;
    ctx->state[2] = 0x79622d32;
    ctx->state[3] = 0x6b206574;

    uint32_t *key_32 = (uint32_t *)key;
    for(int i = 4; i < 12; i++){
        ctx->state[i] = BE32TOLE32(key_32[i-4]);
    }

    return CRYPT_OKAY;
}

void print_chacha20_state(const uint32_t state[16]) {
    if (!state) {
        printf("State is NULL\n");
        return;
    }

    printf("ChaCha20 state:\n");
    for (int i = 0; i < 16; i++) {
        printf("0x%08x", state[i]);
        if ((i + 1) % 4 == 0)
            printf("\n");
        else
            printf(" ");
    }
}

// This function handles both encrypt and decrypt for chacha20
crypt_status chacha20_crypt(chacha20_ctx *ctx, unsigned int counter,
                            const unsigned char *nonce, unsigned int nonce_len, 
                            const unsigned char *input, unsigned int input_len, 
                            unsigned char *output, unsigned int output_len)
{
    if(!ctx || !input || !output) return CRYPT_NULL_PTR;

    if(output_len < input_len){
        return CRYPT_BAD_BUFFER_LEN;
    }

    if(nonce_len != CHACHA20_NONCE_LEN_BYTES){
        return CRYPT_BAD_NONCE;
    }

    uint32_t *nonce_32 = (uint32_t *)nonce;
    for(int i = 13; i < 16; i++){
        ctx->state[i] = BE32TOLE32(nonce_32[i-13]);
    }

    ctx->state[12] = counter;

    unsigned char keystream[CHACHA20_KEYSTREAM_LEN_BYTES] = {0};
    unsigned int remaining_len = input_len;
    for(unsigned int j = 0; j < input_len / CHACHA20_KEYSTREAM_LEN_BYTES; j++){
        chacha20_block(ctx, keystream);
        for(unsigned int i = 0; i < CHACHA20_KEYSTREAM_LEN_BYTES; i++){
            output[i + j * CHACHA20_KEYSTREAM_LEN_BYTES] = input[i + j * CHACHA20_KEYSTREAM_LEN_BYTES] ^ keystream[i];
        }
        ctx->state[12]++; // incrementing the counter
        remaining_len -= CHACHA20_KEYSTREAM_LEN_BYTES;
    }
    if(remaining_len > 0){
        chacha20_block(ctx, keystream);
        for(unsigned int i = 0; i < remaining_len; i++){
            output[i + (input_len - remaining_len)] = input[i + (input_len - remaining_len)] ^ keystream[i];
        }
    }

    return CRYPT_OKAY;
}

crypt_status chacha20_cleanup(chacha20_ctx *ctx){
    if(!ctx) return CRYPT_NULL_PTR;

    memset(ctx->state, 0, CHACHA20_STATE_LEN_BYTES);

    return CRYPT_OKAY;
}

// quarter round test
// int tmp_test(){
    
//     const unsigned char key[] = {
//         0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
//         0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
//         0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
//         0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
//     };

//     const unsigned char nonce[] = {
//         0x00, 0x00, 0x00, 0x00, 
//         0x00, 0x00, 0x00, 0x4a, 
//         0x00, 0x00, 0x00, 0x00
//     };

//     unsigned int counter = 1;

//     unsigned char keystream[64] = {0};
//     chacha20_ctx ctx1 = {0};
//     chacha20_init(&ctx1, key, nonce, counter);
//     // chacha20_block(&ctx1, keystream);
//     // print_chacha20_state(ctx1.state);
//     // print_bytes_array(keystream, 64, "keystream");
//     unsigned char plaintext[] = {
//         0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c,
//         0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73,
//         0x73, 0x20, 0x6f, 0x66, 0x20, 0x27, 0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63,
//         0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65, 0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f,
//         0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20, 0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20,
//         0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73,
//         0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69,
//         0x74, 0x2e
//     };

//     unsigned char exp_cipher[] = {
//         0x6e, 0x2e, 0x35, 0x9a, 0x25, 0x68, 0xf9, 0x80, 0x41, 0xba, 0x07, 0x28, 0xdd, 0x0d, 0x69, 0x81,
//         0xe9, 0x7e, 0x7a, 0xec, 0x1d, 0x43, 0x60, 0xc2, 0x0a, 0x27, 0xaf, 0xcc, 0xfd, 0x9f, 0xae, 0x0b,
//         0xf9, 0x1b, 0x65, 0xc5, 0x52, 0x47, 0x33, 0xab, 0x8f, 0x59, 0x3d, 0xab, 0xcd, 0x62, 0xb3, 0x57,
//         0x16, 0x39, 0xd6, 0x24, 0xe6, 0x51, 0x52, 0xab, 0x8f, 0x53, 0x0c, 0x35, 0x9f, 0x08, 0x61, 0xd8,
//         0x07, 0xca, 0x0d, 0xbf, 0x50, 0x0d, 0x6a, 0x61, 0x56, 0xa3, 0x8e, 0x08, 0x8a, 0x22, 0xb6, 0x5e,
//         0x52, 0xbc, 0x51, 0x4d, 0x16, 0xcc, 0xf8, 0x06, 0x81, 0x8c, 0xe9, 0x1a, 0xb7, 0x79, 0x37, 0x36,
//         0x5a, 0xf9, 0x0b, 0xbf, 0x74, 0xa3, 0x5b, 0xe6, 0xb4, 0x0b, 0x8e, 0xed, 0xf2, 0x78, 0x5e, 0x42,
//         0x87, 0x4d
//     };


//     unsigned char ciphertext[114];

//     chacha20_crypt(&ctx1, plaintext, 114, ciphertext, 114);
    
//     print_bytes_array(plaintext, 114, "plaintext");
//     if(!memcmp(exp_cipher, ciphertext, 114)){
//         print_bytes_array(ciphertext, 114, "ciphertext");
//         print_bytes_array(exp_cipher, 114, "exp ciphertext");
//     }


//     return 0;

// }

