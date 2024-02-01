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
    b->digits = malloc(num_of_digit * sizeof(uint64_t));
    memset(b->digits, 0, num_of_digit * sizeof(uint64_t));
    b->MSD = 0;
    b->num_of_digit = num_of_digit;
}

bigint_err bigint_free(bigint *b){
    if(b == NULL) 
        return BIGINT_ERROR_NULLPTR;
    free(b->digits);
    b->digits = NULL;
    return BIGINT_OKAY;
}

bigint_err bigint_expand(bigint *a, unsigned int size){
    if(!a) return BIGINT_ERROR_NULLPTR;
    
    if(a->num_of_digit >= size)
        return BIGINT_OKAY;

    uint64_t *tmp = malloc(size * sizeof(uint64_t));
    if(!tmp)
        return BIGINT_REALLOC_FAILURE;
    memset(tmp, 0, size * sizeof(uint64_t));
    memcpy(tmp, a->digits, a->num_of_digit * sizeof(uint64_t));
    bigint_free(a);

    a->digits = tmp;
    a->num_of_digit = size;
    return BIGINT_OKAY;

}

bigint_err bigint_set_zero(bigint *a){
    if(!a) return BIGINT_ERROR_NULLPTR;
    memset(a->digits, 0, a->num_of_digit);
    return BIGINT_OKAY;
}

/*
    if a->digits[i] is not zero, then (a->digits[i] != 0) evaluates to 1. Then take the 
    negative of 1, which in 2's complement is all 1's, and OR it onto found_nonzero_mask
    to set the mask indicating that we have found a non zero value 

    finally, by negating the mask, if the mask is set, then when it's ANDed with 1, it returns 0
    so MSD will no longer decrement once a nonzero digit is found

    I am intentionally letting the loop run through the entire digits buffer to ensure 
    constant-time execution when it's used in cryptographic operations 
*/
bigint_err bigint_clamp(bigint *a){
    if(!a) return BIGINT_ERROR_NULLPTR;
    unsigned int i = a->MSD;
    int found_nonzero_mask = 0;
    for(; i > 0; i--){
        found_nonzero_mask |= -(a->digits[i] != 0);
        a->MSD -= (~found_nonzero_mask) & 1;
    }
    return BIGINT_OKAY;
}

void bigint_copy(bigint *src, bigint *dest, int num){
    memcpy(dest->digits, src->digits, num * sizeof(uint64_t));
}

bigint_err bigint_print(bigint *b){
    if (!b)
        return BIGINT_ERROR_NULLPTR;
    
    for(int i = b->MSD; i >= 0; i--){
        printf("%ld", b->digits[i]);
    }
    printf("\n");
    return BIGINT_OKAY;
}

bigint_err bigint_from_int(bigint *b, unsigned int a){
    if(!b) return BIGINT_ERROR_NULLPTR;

    b->digits[0] = a;
    return BIGINT_OKAY;
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

