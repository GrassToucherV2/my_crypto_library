#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bigint.h"

void buffers_xor(const char *a, const char *b, int len, char *output){
    for(int i = 0; i < len; i++){
        output[i] = a[i] ^ b[i];
        // printf("a[%d] = %c b[%d] = %c, output[%d] = %c\n", i, a[i], i, b[i], i, output[i]);
    }
}

void bigint_init(bigint *b, unsigned int num_of_digit){
    b->digits = malloc(num_of_digit * sizeof(int));
    memset(b->digits, 0, num_of_digit * sizeof(int));
    b->MSD = 0;
    b->num_of_digit = num_of_digit;
}

bigint_err bigint_free(bigint *b){
    if(b == NULL) 
        return BIGINT_ERROR_NULLPTR;
    free(b->digits);
    return BIGINT_OKAY;
}

bigint_err bigint_print(bigint *b){
    if (!b)
        return BIGINT_ERROR_NULLPTR;
    
    for(int i = b->MSD; i >= 0; i--){
        printf("%d", b->digits[i]);
    }
    printf("\n");
    return BIGINT_OKAY;
}

bigint_err bigint_from_int(bigint *b, unsigned int a){
    if(!b) return BIGINT_ERROR_NULLPTR;

    b->digits[0] = a;
    return BIGINT_OKAY;
}

bigint_err bigint_to_str(bigint *b, unsigned char* str){
    
}

bigint_err bigint_inc(bigint *a){
    if(a == NULL) return BIGINT_ERROR_NULLPTR;

    unsigned int carry = 1;
    unsigned int i = 0;
    while(carry && i <= a->MSD) { 
        a->digits[i] += carry;
        if(a->digits[i] >= BASE) { 
            a->digits[i] = 0;
            if (i == a->MSD && i < a->num_of_digit - 1) {
                // If incrementing the MSD and there's room, increase MSD.
                a->MSD++;
                a->digits[a->MSD] = 1;
                carry = 0; 
            }
        } else {
            carry = 0;
        }
        i++;
    }

    if (carry) {
        // If still have carry here, it means we've run out of digits.
        return BIGINT_ERROR_OVERFLOW;
    }
    
    return BIGINT_OKAY;
}

