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
    uint32_t *keystream32 = (uint32_t *)keystream;
    for(int i = 0; i < 16; i++){
        keystream32[i] = BE32TOLE32(tmp_state[i]);
    }
}

crypt_status chacha20_init(chacha20_ctx *ctx, const unsigned char *key)
{
    if(!ctx || !key) return CRYPT_NULL_PTR;

    memset_s(ctx->state, 0, sizeof(ctx->state));

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

    memset_s(ctx->state, 0, CHACHA20_STATE_LEN_BYTES);

    return CRYPT_OKAY;
}

