#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tools.h"
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

    int found_nonzero_mask = 0;
    for(unsigned int i = a->MSD; i > 0; i--){
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

int bigint_cmp_zero(const bigint *a){
    if(!a){
        printf("The given bigint is NULL\n");
        return -1;
    }

    int64_t b = 0;
    for(int i = a->MSD; i >= 0; i--){
        b |= a->digits[i];
    }
    /*
        if a != 0, then the lower 32 bits of b will be nonzero, when negated using 2's complement
        all of its upper 32 bits will be set to 1.
        if a == 0, then all 64 bits of b will be 0, and when negated using 2's complement, it will
        still be 0
    */
    return (-b >> (sizeof(digit) * 8)) & 1 ; 
}

int bigint_cmp(const bigint *a, const bigint *b){
    if(!a || !b) return BIGINT_ERROR_NULLPTR;
    
    unsigned int msd_a = a->MSD;
    unsigned int msd_b = b->MSD;
    int i = 0;

    // is this part constant-time ???
    if(msd_a > msd_b){
        i = msd_a;
    } else{
        i = msd_b;
    }

    int a_lt_b_mask = 0;
    int equal_mask = 0;
    int64_t c = 0;
    int64_t c1 = 0;
    int c1_sign = 0;

    /*
        c is used to check whether a == b, just like how it is done in bigint_cmp_zero
        c1 is the difference between each digits of a and b, from MSD to LSD, if we get 
        a negative result, we know that a < b, so we extract the sign bit and negate it to set 
        the a_lt_b_mask to all 1's.
        equal_mask is the same as bigint_cmp_zero, except for that when a == b, equal_mask would be 0
        so subtract 1, to make it -1 to set the mask
    */
    for(; i >= 0; i--){
        c |= a->digits[i] ^ b->digits[i];
        c1 = (int)a->digits[i] - (int)b->digits[i];
        c1_sign = (c1 >> (8 * sizeof(int64_t) - 1)) & 1;
        a_lt_b_mask |= -c1_sign;   
    }

    equal_mask = ((-c >> (sizeof(digit) * 8)) & 1) - 1;

    /*
        A magic trick learned during my internship from my mentor
        It conditionally checks which value to return. The conditional part is the mask &,
        if the mask is not set, it just makes (res ^ 0) and (res ^ -1) 0, so res remains 1 
    */
    int res = 1;
    res ^= a_lt_b_mask & (res ^ -1);
    res ^= equal_mask & (res ^ 0);
    return res;
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

bigint_err bigint_add(bigint *a, bigint *b, bigint *c){
    if(!a || !b || !c) return BIGINT_ERROR_NULLPTR;

    unsigned int a_msd = a->MSD;
    unsigned int b_msd = b->MSD;
    unsigned int min, max, old_num_digits_used_c;
    bigint *x;

    if(a_msd > b_msd){
        min = b_msd;
        max = a_msd;
        x = a;
    } else {
        min = a_msd;
        max = b_msd;
        x = b;
    }
    old_num_digits_used_c = c->MSD + 1;
    // making sure c can hold enough digits, max + 2 as MSD serves as the index for the MSD digit
    if(c->num_of_digit < max + 2){
        CHECK_OKAY(bigint_expand(c, max + 2));
    }

    uint64_t tmp = 0;
    int carry = 0;
    int i = 0;
    // compute lower min digits 
    for(; i < min; i++){
        tmp = (uint64_t)a->digits[i] + (uint64_t)b->digits[i] + carry;
        carry = (tmp >> DIGIT_BIT) & 1;
        c->digits[i] = tmp & BASE;  // only keeping the lower 32 bits, works like tmp mod BASE
    }

    // compute the rest of digits
    if(min != max){
        for(int i = min; i < max; i++){
            tmp = x->digits[i] + carry;
            carry = (tmp >> DIGIT_BIT) & 1;
            c->digits[i] = tmp & BASE; 
        }
    }

    // in case there is still carry leftover 
    if(carry){
        i++;
        c->digits[i] += carry;
    }
    c->MSD = i;
    
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

