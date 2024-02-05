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

bigint_err bigint_init(bigint *b, unsigned int num_of_digit){
    b->digits = malloc(num_of_digit * sizeof(digit));
    if(!b->digits){
        printf("malloc failed in bigint_init\n");
        return BIGINT_REALLOC_FAILURE;
    }
    memset(b->digits, 0, num_of_digit * sizeof(digit));
    b->MSD = 0;
    b->num_of_digit = num_of_digit;
    return BIGINT_OKAY;
}

bigint_err bigint_free(bigint *b){
    if(b == NULL) 
        return BIGINT_ERROR_NULLPTR;
    free(b->digits);
    b->digits = NULL;
    return BIGINT_OKAY;
}

bigint_err bigint_expand(bigint *a, unsigned int num){
    if(!a) return BIGINT_ERROR_NULLPTR;
    
    if(a->num_of_digit >= num)
        return BIGINT_OKAY;

    digit *tmp = malloc(num * sizeof(digit));
    if(!tmp)
        return BIGINT_REALLOC_FAILURE;
    memset(tmp, 0, num * sizeof(digit));
    memcpy(tmp, a->digits, a->num_of_digit * sizeof(digit));
    bigint_free(a);

    a->digits = tmp;
    a->num_of_digit = num;
    return BIGINT_OKAY;

}

bigint_err bigint_set_zero(bigint *a){
    if(!a) return BIGINT_ERROR_NULLPTR;
    memset(a->digits, 0, a->num_of_digit);
    a->MSD = 0;
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
        a->num_of_digit -= (~found_nonzero_mask) & 1;
    }
    return BIGINT_OKAY;
}

bigint_err bigint_copy(const bigint *src, bigint *dest){
    if(!src || !dest) return BIGINT_ERROR_NULLPTR;

    // if dest does not have enough digits allocated (src->MSD + 1) is the num of digits being used in src. 
    // Expand dest if necessary
    if(src->MSD + 1 > dest->num_of_digit){
        CHECK_OKAY(bigint_expand(dest, src->MSD+1));
    }
    CHECK_OKAY(bigint_set_zero(dest));
       
    memcpy(dest->digits, src->digits, src->num_of_digit * sizeof(digit));
    dest->MSD = src->MSD;

    return BIGINT_OKAY;
}

bigint_err bigint_from_small_int(bigint *b, digit a){
    if(!b) return BIGINT_ERROR_NULLPTR;

    CHECK_OKAY(bigint_set_zero(b));

    b->digits[b->MSD] = a;

    return BIGINT_OKAY;    
}

// TODO: unit test for this function
int bigint_cmp_zero(const bigint *a){
    if(!a){
        printf("The given bigint is NULL\n");
        return -1;
    }

    int64_t b = 0;
    for(int i = a->MSD; i >= 0; i--){
        b |= a->digits[i];
    }

    return ((-b) >> (sizeof(digit) * 8)) & 1 ;
}

/* 
    This function shifts a by one digit to the left
*/
bigint_err bigint_left_shift(bigint *a){
    if(!a) return BIGINT_ERROR_NULLPTR;

    // making sure the digit is big enough to shift, otherwise expand it
    int diff  = a->num_of_digit - (a->MSD + 1);
    if(diff <= 0)
        CHECK_OKAY(bigint_expand(a, a->num_of_digit + 1));

    // shifting all the digits over to the left by one digit
    unsigned int i = 0;
    for(i = a->MSD; i > 0; i--)
        a->digits[i + 1] = a->digits[i];
    
    // handling the last digit and  then setting the lowest digit to 0
    a->digits[1] = a->digits[0];
    a->digits[0] = 0;
    a->MSD++;

    return BIGINT_OKAY;

}

bigint_err bigint_print(const bigint *b){
    if (!b)
        return BIGINT_ERROR_NULLPTR;
    
    for(int i = b->MSD; i >= 0; i--){
        printf("%016X ", b->digits[i]);
    }
    printf("\n");
    return BIGINT_OKAY;
}

bigint_err bigint_from_int(bigint *b, unsigned int a){
    if(!b) return BIGINT_ERROR_NULLPTR;

    b->digits[0] = a;
    return BIGINT_OKAY;
}

bigint_err bigint_from_bytes(bigint *a, const unsigned char *str, unsigned int len){
    if(!a || !str) return BIGINT_ERROR_NULLPTR;

    CHECK_OKAY(bigint_set_zero(a));

    digit d = 0;
    // if the length of the string is not a multiple of sizeof(digit)
    int leftover_bytes = len % sizeof(digit);
    if(leftover_bytes){
        for(int i = leftover_bytes; i > 0; i--){
            d <<= 8;
            d |= *str;
            str++;
        }
        a->digits[0] = d;
        len -= leftover_bytes;
        if(len > sizeof(digit))
            CHECK_OKAY(bigint_left_shift(a));
    }

    // Read the rest of the digits 
    for(; len > 0; len -= sizeof(digit)){
        for(int i = sizeof(digit); i > 0; i--){
            d <<= 8;
            d |= *str;
            str++;
        }
        a->digits[0] = d;
        if(len > sizeof(digit))
            CHECK_OKAY(bigint_left_shift(a));
    }
    return BIGINT_OKAY;
}

// need to make sure output is long enough by using len arg
bigint_err bigint_to_bytes(const bigint *a, unsigned char *output, 
                            unsigned int len, int ignore_leading_zero){
    if(!a || !output) return BIGINT_ERROR_NULLPTR;

    unsigned int num_bytes = (a->MSD + 1) * sizeof(digit);
    if(num_bytes > len) 
        return BIGINT_ERROR_INSUFFICIENT_BUF;
    
    memset(output, 0, len);

    // iterating over each digit
    int i;
    int index = 0;
    int leading_zero_count = 0;
    unsigned char c;
    digit d  = 0;
    for(i = a->MSD; i >= 0; i--){
        d = a->digits[i];
        
        // processing each digit
        for(int j = sizeof(digit) - 1; j >= 0; j--){
            c = (unsigned char) (d >> (j * 8));    // 8 bits = 1 byte 
            if(ignore_leading_zero){
                if(!c && !index){
                    leading_zero_count++;
                    continue;                   // if we ignore leading zeroes, then if index = 0 and 
                }                               // c = 0, move on to next iteration
            }
            output[index + leading_zero_count] = c;
            index++;
        }
    }

    if(!index){
        output[index] = 0;
    }

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

