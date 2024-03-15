#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "md5.h"
#include "../test/test_util.h"

// the padded length must be congruent to 448 mod 512 -> pad_len = 448 (mod 512)
// then we pad the message length as a 64 bit number to form a string such that 
// output_len % 512 = 0
crypt_status md5_pad(unsigned char *msg, unsigned int msg_len, 
                    unsigned char *output, unsigned int output_len)
{   
    // making sure the output buffer is a multiple of 64 bytes (512 bits)
    if((output_len) % MD5_REQ_LEN_BYTES != 0){
        return CRYPT_BAD_BUFFER_LEN;
    }

    memcpy(output, msg, msg_len);
    
    // the message is padded with 1 bit followed by 0 bits until the length 
    // is congruent to 448 (mod 512)
    // 0x80 = 0b10000000
    output[msg_len] = 0x80;

    unsigned int current_len = msg_len + 1;
    unsigned int len_to_pad_zero = (MD5_REQ_LEN_BYTES - MD5_LEN_FIELD_SIZE_BYTES) - (current_len % MD5_REQ_LEN_BYTES);
    memset(&output[current_len], 0, len_to_pad_zero);
    current_len += len_to_pad_zero;

    // append the original message length in bits as a 64 bit number to the end of the output buffer
    uint64_t len_bits = msg_len * NUM_BITS_IN_BYTE;
    // len_bits = LE64TOBE64(len_bits);
    memcpy(&output[current_len], &len_bits, MD5_LEN_FIELD_SIZE_BYTES);

    return CRYPT_OKAY;

}