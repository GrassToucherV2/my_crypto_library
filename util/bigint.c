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
    b->digits = (digit *)malloc(num_of_digit * sizeof(digit));
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

    digit *tmp = (digit *)malloc(num * sizeof(digit));
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
    if(src->num_of_digit > dest->num_of_digit){
        CHECK_OKAY(bigint_expand(dest, src->num_of_digit));
    }
    CHECK_OKAY(bigint_set_zero(dest));
    
    memcpy(dest->digits, src->digits, (src->MSD + 1) * sizeof(digit));
    dest->MSD = src->MSD;

    return BIGINT_OKAY;
}

bigint_err bigint_from_small_int(bigint *b, digit a){
    if(!b) return BIGINT_ERROR_NULLPTR;

    CHECK_OKAY(bigint_set_zero(b));

    b->digits[0] = a;
    b->MSD = 0;  

    return BIGINT_OKAY;    
}

bigint_err bigint_pad_zero(bigint *a, unsigned int msd){
    if(!a) return BIGINT_ERROR_NULLPTR;

    if(a->MSD > msd){
        return BIGINT_OKAY;
    }

    if(msd > a->num_of_digit){
        CHECK_OKAY(bigint_expand(a, msd + 1));
    }

    memset(&a->digits[a->MSD + 1], 0, (msd - a->MSD) * sizeof(digit));
    a->MSD = msd;

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

// this is another comparison function I came up for Mozilla
// NSS mpi library, possibly a better solution

// int bigint_cmp(const bigint *a, const bigint *b) {
//     if (!a || !b) return BIGINT_ERROR_NULLPTR;

//     unsigned int max_msd = 0;
//     // Ensure temporary bigints are padded to the same size, omitted for brevity...
//     if(a->MSD > b->MSD){
//         max_msd = a->MSD;
//     } else {
//         max_msd = b->MSD;
//     }
    
//     int a_lt_b = 0; // Assume a == b initially.
//     int a_gt_b = 0;
//     uint64_t a_digit, b_digit; 
//     for (int i = max_msd; i >= 0; i--) {
//         a_digit = (i <= a->MSD) ? a->digits[i] : 0;
//         b_digit = (i <= b->MSD) ? b->digits[i] : 0;

//         // Use bitwise logic to set flags based on comparison.
//         // These operations are constant-time.
//         int diff_is_neg = ((a_digit - b_digit) >> 63) & 1;
//         int diff_is_pos = ((b_digit - a_digit) >> 63) & 1;

//         // Update flags if this is the first non-zero difference we've encountered.
//         a_lt_b |= (diff_is_neg & ~(a_lt_b | a_gt_b));
//         a_gt_b |= (diff_is_pos & ~(a_lt_b | a_gt_b));
//     }

//     int a_lt_b_mask = -a_lt_b;
//     int a_gt_b_mask = -a_gt_b;

//     int res = 0;
//     res ^= a_lt_b_mask & (res ^ -1);
//     res ^= a_gt_b_mask & (res ^ 1);
//     return res;
// }
/* 
   Aims to compare two bigints in constant-time
*/
int bigint_cmp(const bigint *a, const bigint *b){
    if(!a || !b) return BIGINT_ERROR_NULLPTR;

    unsigned int msd_a = a->MSD;
    unsigned int msd_b = b->MSD;
    unsigned int max_msd = 0;
    bigint tmp_a, tmp_b;
    
    bigint_init(&tmp_a, a->MSD + 1);
    bigint_init(&tmp_b, b->MSD + 1);

    bigint_copy(a, &tmp_a);
    bigint_copy(b, &tmp_b);
    
    // This part may not be constant time
    if(msd_a > msd_b){
        max_msd = msd_a;
        CHECK_OKAY(bigint_pad_zero(&tmp_b, max_msd)); // this fixes valgrind errors, but im not sure if i like it 
    } else{
        max_msd = msd_b;
        CHECK_OKAY(bigint_pad_zero(&tmp_a, max_msd));
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
        so subtract 1, to make it -1 to set the mask.
        During subtraction process, if c1 > 0 appeared before first c1 < 0, then a_lt_b_mask
        should remain 0
    */
   int positive_c1_seen_flag = 0;
   int a_lt_b_flag = 0;
    for(int i = max_msd; i >= 0; i--){
        c |= tmp_a.digits[i] ^ tmp_b.digits[i];
        c1 = (int64_t)tmp_a.digits[i] - (int64_t)tmp_b.digits[i];
        c1_sign = (c1 >> (8 * sizeof(int64_t) - 1)) & 1;
        
        // set positive_c1_seen flag if we have seen a positive c1
        // if(!c1_sign){
        //     positive_c1_seen_flag = 1;
        // }
        // 
        positive_c1_seen_flag |= (~c1_sign & 1);

        // update a_lt_b_flag to indicate a < b, 
        // only if we haven't seen a positive c1 and we got a negative c1
        // if (c1_sign && !positive_c1_seen_flag) {
        //     a_lt_b_flag = 1;
        // }
        a_lt_b_flag |= (c1_sign && !positive_c1_seen_flag);
        
    }

    bigint_free(&tmp_a);
    bigint_free(&tmp_b);
    
    if(a_lt_b_flag){
        a_lt_b_mask = -1;
    } else {
        a_lt_b_mask = 0;
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

bigint_err bigint_left_bit_shift(const bigint *a, bigint *c){
    if(!a || !c) return BIGINT_ERROR_NULLPTR;

    // making sure c has at least one more digit than a
    if(c->num_of_digit < a->MSD + 2){
        CHECK_OKAY(bigint_expand(c, a->MSD + 2));
    }

    int carry = 0;
    unsigned int i = 0;

    for(; i <= a->MSD; i++){
        c->digits[i] = (a->digits[i] << 1) | carry;
        // the MSB of each digit will be promoted and become the 
        // LSB of the next digit as a result of left shift
        carry = a->digits[i] >> ((sizeof(digit) * 8) - 1);
    }

    if(carry){
        c->digits[i] = carry;
        c->MSD = i;
    } else {
        c->MSD = i - 1;
    }

    return BIGINT_OKAY;
}

/* 
    This function shifts a by one digit to the left
*/
bigint_err bigint_left_shift(bigint *a){
    if(!a) return BIGINT_ERROR_NULLPTR;

    // making sure the digit is big enough to shift, otherwise expand it
    int diff = a->num_of_digit - (a->MSD + 1);
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

bigint_err bigint_left_shift_digits(bigint *a, unsigned int b){
    if(!a) return BIGINT_ERROR_NULLPTR;

    if( (a->MSD + 1 + b) > a->num_of_digit){
        CHECK_OKAY(bigint_expand(a, (a->MSD + 1 + b)));
    }

    for(unsigned int i = 0; i < b; i++){
        CHECK_OKAY(bigint_left_shift(a));
    }

    return BIGINT_OKAY;
}

// This function assumes b <= 32, otherwise it returns an error
bigint_err bigint_left_shift_bits(bigint *a, unsigned int b){
    if(!a) return BIGINT_ERROR_NULLPTR;
    if(b >= DIGIT_BIT)  return BIGINT_ERROR_SHIFTING;

    if(a->num_of_digit < a->MSD + 2){
        CHECK_OKAY(bigint_expand(a, a->MSD + 2));
    }

    digit carry = 0;
    digit current_digit = 0;
    for(unsigned int i = 0; i <= a->MSD; ++i){
        // Store the current digit, basically same logic as left shift by 1 bit 
        // promote "num_bits_to_shift" bits to the next digit
        current_digit = a->digits[i];
        a->digits[i] = (current_digit << b) | carry;
        carry = current_digit >> ((sizeof(digit) * 8) - b);
    }

    if(carry){
        a->digits[a->MSD + 1] = carry;
        a->MSD++;
    }

    return BIGINT_OKAY;
}

bigint_err bigint_right_bit_shift(const bigint *a, bigint *c){
    if(!a || !c) return BIGINT_ERROR_NULLPTR;

    if(c->num_of_digit < a->MSD + 1){
        CHECK_OKAY(bigint_expand(c, a->MSD + 1));
    }

    int tmp_lsb = 0;
    unsigned int i = 0;
    for(; i < a->MSD; i++){
        // extracting the LSB of the digit and it will become the MSD of the previous digit
        tmp_lsb = a->digits[i + 1] & 1;
        c->digits[i] = (a->digits[i] >> 1) | (tmp_lsb << ((sizeof(digit) * 8) - 1));
    }

    c->digits[i] = (a->digits[i] >> 1);
    c->MSD = i;

    return BIGINT_OKAY;
}

bigint_err bigint_right_shift(bigint *a){
    if(!a) return BIGINT_ERROR_NULLPTR;

    unsigned int i = 0;

    for(; i < a->MSD; i++){
        a->digits[i] = a->digits[i + 1];
    }
    a->digits[i] = 0;
    a->MSD--;

    return BIGINT_OKAY;    

}

bigint_err bigint_right_shift_digits(bigint *a, unsigned int b){
    if(!a) return BIGINT_ERROR_NULLPTR;

    // still run the loop to maintain constant-time behavior, rather than return 0 directly
    // is it necessary tho?
    if(b > a->MSD + 1){
        b = a->MSD + 1;
    }

    for(unsigned int i = 0; i < b; i++){
        CHECK_OKAY(bigint_right_shift(a));
    }

    // conditionally set a.MSD to 0 if b is greater than a.MSD. Note that this method
    // is dependent on a->MSD's initial value
    // There might be a better solution 
    // to incorporate this check directly into bigint_right_shift 
    a->MSD &= ~( -(b > a->MSD + 1));

    return BIGINT_OKAY;
    
}

bigint_err bigint_right_shift_bits(bigint *a, unsigned int b){
    if(!a) return BIGINT_ERROR_NULLPTR;
    if(b >= DIGIT_BIT)  return BIGINT_ERROR_SHIFTING;

    if(a->num_of_digit < a->MSD + 1){
        CHECK_OKAY(bigint_expand(a, a->MSD + 1));
    }

    int tmp_lsb = 0;
    unsigned int i = 0;
    unsigned int mask = (1 << b) - 1; 
    for(; i < a->MSD; i++){
        // extracting the b LSB of the digit and it will become the b MSD of the previous digit
        tmp_lsb = a->digits[i + 1] & mask;
        a->digits[i] = (a->digits[i] >> b) | (tmp_lsb << ((sizeof(digit) * 8) - b));
    }

    a->digits[i] = (a->digits[i] >> b);
    a->MSD = i;

    return BIGINT_OKAY;

}

bigint_err bigint_print(const bigint *b, char *str){
    if (!b)
        return BIGINT_ERROR_NULLPTR;
    
    printf("%s", str);
    for(int i = b->MSD; i >= 0; i--){
        printf("%08X ", b->digits[i]);
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

bigint_err bigint_add(const bigint *a, const bigint *b, bigint *c){
    if(!a || !b || !c) return BIGINT_ERROR_NULLPTR;

    unsigned int a_msd = a->MSD;
    unsigned int b_msd = b->MSD;
    unsigned int min, max;
    const bigint *x;      // used to hold the bigger bigint, which will be used later to finish addition

    if(a_msd > b_msd){
        min = b_msd;
        max = a_msd;
        x = a;
    } else {
        min = a_msd;
        max = b_msd;
        x = b;
    }

    // making sure c can hold enough digits, max + 2 as MSD serves as the index for the MSD digit
    if(c->num_of_digit < max + 2){
        CHECK_OKAY(bigint_expand(c, max + 2));
    }
    uint64_t tmp = 0;
    int carry = 0;
    unsigned int i = 0;
    // compute lower "min" digits 
    for(; i <= min; i++){
        tmp = (uint64_t)a->digits[i] + (uint64_t)b->digits[i] + carry;
        carry = (tmp >> DIGIT_BIT) & 1;
        c->digits[i] = tmp & BASE;  // only keeping the lower 32 bits, works like "tmp mod BASE"
    }

    // compute the rest of digits
    if(min != max){
        for(; i <= max; i++){
            tmp = x->digits[i] + carry;
            carry = (tmp >> DIGIT_BIT) & 1;
            c->digits[i] = tmp & BASE; 
        }
    }
    // in case there is still carry leftover, no need to increment i, as it should be max + 1 at this point 
    // if above loops ran successfully
    if(carry){
        c->digits[i] = carry;
        c->MSD = i;  
    } else{
        c->MSD = i - 1;  
    }
      
    return BIGINT_OKAY;
}

bigint_err bigint_add_digit(const bigint *a, digit b, bigint *c){
    if(!a || !c) return BIGINT_ERROR_NULLPTR;

    bigint b1;
    CHECK_OKAY(bigint_init(&b1, 1));
    CHECK_OKAY(bigint_from_small_int(&b1, b));
    CHECK_OKAY(bigint_add(a, &b1, c));

    bigint_free(&b1);

    return BIGINT_OKAY;
}

bigint_err bigint_sub(const bigint *a, const bigint *b, bigint *c){
    if(!a || !b || !c) return BIGINT_ERROR_NULLPTR;

    unsigned int max_msd = 0;
    unsigned int min_msd = 0;
    const bigint *bigger;
    const bigint *smaller;

    int r = bigint_cmp(a, b);
    if(r){
        max_msd = a->MSD;
        bigger = a;
        min_msd = b->MSD;
        smaller = b;
        
    } else {
        max_msd = b->MSD;
        bigger = b;
        min_msd = a->MSD;
        smaller = a;
    }

    if(c->num_of_digit < max_msd + 1){
        CHECK_OKAY(bigint_expand(c, max_msd + 1));
    }

    unsigned int i = 0;
    int borrowed = 0;
    int sign = 0;
    int64_t base = 0;
    int64_t tmp = 0;

    // computing lower digits first
    for(; i <= min_msd; i++){
        /*
            if the bigger->digits[i] - smaller->digits[i] < 0, we need to borrow BASE + 1 from the next digit
            therefore we add BASE + 1 after confirming the sign of tmp is negative
            borrow BASE + 1 because BASE = 0xFFFFFFFF, which is equivalent to 9 in decimal, hence plus 1 to get 10's equivalent 
            in base 2^32 
        */
        tmp = (int64_t)bigger->digits[i] - (int64_t)smaller->digits[i] - (int64_t)borrowed;
        sign = tmp >> (sizeof(int64_t) * 8 - 1) & 1;
        if(sign){
            base = BASE + 1;
        } else {
            base = 0;
        }
        c->digits[i] = tmp + base;
        borrowed = sign;
    }

    if(min_msd != max_msd){
        for(; i <= max_msd; i++){
            tmp = bigger->digits[i] - borrowed;
            sign = tmp >> (sizeof(int64_t) * 8 - 1) & 1;
            if(sign){
                base = BASE + 1;
            } else {
                base = 0;
            }
            c->digits[i] = tmp + base;
            borrowed = sign;
        }
    }

    c->MSD = i - 1;
    if(borrowed){
        c->digits[i] -= borrowed;
    }

    bigint_clamp(c);
    return BIGINT_OKAY;
}

bigint_err bigint_sub_digit(const bigint *a, digit b, bigint *c){
    if(!a || !c) return BIGINT_ERROR_NULLPTR;

    bigint b1;
    CHECK_OKAY(bigint_init(&b1, 1));
    CHECK_OKAY(bigint_from_small_int(&b1, b));
    CHECK_OKAY(bigint_sub(a, &b1, c));

    bigint_free(&b1);

    return BIGINT_OKAY;
}

/*
    This is the naive elementary school multiplication algorithm
    from wikipedia
    more performant ones are probably needed in the long run
*/
bigint_err bigint_mul(const bigint *a, const bigint *b, bigint *c){
    if(!a || !b || !c) return BIGINT_ERROR_NULLPTR;

    unsigned int a_digits_used = a->MSD + 1;
    unsigned int b_digits_used = b->MSD + 1;

    if(c->num_of_digit != a_digits_used + b_digits_used){
        CHECK_OKAY(bigint_expand(c, a_digits_used + b_digits_used));
    }

    uint64_t tmp = 0;
    uint64_t carry = 0;
    unsigned int i = 0;

    for(; i < a_digits_used; i++){
        carry = 0;
        for(unsigned int j = 0; j < b_digits_used; j++){
            tmp = (uint64_t)a->digits[i] * (uint64_t)b->digits[j] + c->digits[i + j] + carry;
            
            c->digits[i + j] = tmp & BASE;  

            carry = tmp >> (sizeof(digit) * 8);
        }
        c->digits[i + b_digits_used] = carry;
    }

    c->MSD = a_digits_used + b_digits_used - 1;
    while(c->MSD > 0 && c->digits[c->MSD] == 0) c->MSD--;

    return BIGINT_OKAY;
}

bigint_err bigint_mul_digit(const bigint *a, digit b, bigint *c){
    if(!a || !b) return BIGINT_ERROR_NULLPTR;

    bigint b1;
    CHECK_OKAY(bigint_init(&b1, 1));
    CHECK_OKAY(bigint_from_small_int(&b1, b));
    CHECK_OKAY(bigint_mul(a, &b1, c));
    CHECK_OKAY(bigint_free(&b1));
    
    return BIGINT_OKAY; 
}

/* Quick doubling of a given bigint by left shifting by one bit */
bigint_err bigint_double(const bigint *a, bigint *c){
    CHECK_OKAY(bigint_left_bit_shift(a, c));

    return BIGINT_OKAY;
}

bigint_err bigint_mul_base(const bigint *a, bigint *c){
    CHECK_OKAY(bigint_copy(a, c));
    CHECK_OKAY(bigint_left_shift(c));

    return BIGINT_OKAY;
}

bigint_err bigint_mul_base_b(const bigint *a, bigint *c, unsigned int b){
    CHECK_OKAY(bigint_copy(a, c));
    bigint_left_shift_digits(c, b);

    return BIGINT_OKAY;
}

/* This function is constant-time, more work needed */
bigint_err bigint_mul_pow_2(const bigint *a, unsigned int b, bigint *c){
    if(!a || !c) return BIGINT_ERROR_NULLPTR;

    unsigned int num_digit_to_shift = b / (sizeof(digit) * 8);
    unsigned int num_bits_to_shift = b % (sizeof(digit) * 8);
    unsigned int required_digits = a->MSD + 1 + num_digit_to_shift + 1;

    if(a->num_of_digit < required_digits){
        CHECK_OKAY(bigint_expand(c, required_digits));
    }
    CHECK_OKAY(bigint_copy(a, c));
    if(num_digit_to_shift)
        CHECK_OKAY(bigint_left_shift_digits(c, num_digit_to_shift));
    if(num_bits_to_shift)
        CHECK_OKAY(bigint_left_shift_bits(c, num_bits_to_shift));

    bigint_clamp(c);
    return BIGINT_OKAY;
}

bigint_err bigint_half(const bigint *a, bigint *c){
    CHECK_OKAY(bigint_right_bit_shift(a, c));

    return BIGINT_OKAY;
}

bigint_err bigint_div_base(const bigint *a, bigint *c){
    CHECK_OKAY(bigint_copy(a, c));
    bigint_right_shift(c);

    return BIGINT_OKAY;
}

bigint_err bigint_div_base_b(const bigint *a, bigint *c, unsigned int b){
    CHECK_OKAY(bigint_copy(a, c));
    bigint_right_shift_digits(c, b);

    return BIGINT_OKAY;
}

bigint_err bigint_div_pow_2(const bigint *a, unsigned int b, bigint *c){
    if(!a || !c) return BIGINT_ERROR_NULLPTR;

    unsigned int num_digit_to_shift = b / (sizeof(digit) * 8);
    unsigned int num_bits_to_shift = b % (sizeof(digit) * 8);
    unsigned int required_digits = a->MSD + 1;

    if(a->num_of_digit < required_digits){
        CHECK_OKAY(bigint_expand(c, required_digits));
    }
    CHECK_OKAY(bigint_copy(a, c));
    if(num_digit_to_shift)
        CHECK_OKAY(bigint_right_shift_digits(c, num_digit_to_shift));
    if(num_bits_to_shift)
        CHECK_OKAY(bigint_right_shift_bits(c, num_bits_to_shift));

    bigint_clamp(c);
    return BIGINT_OKAY;
}

/*
    this is essentially bitwise AND with 2^b - 1
*/
bigint_err bigint_mod_pow_2(const bigint *a, unsigned int b, bigint *c){
    if(!a || !c) return BIGINT_ERROR_NULLPTR;

    unsigned int num_whole_digits = b / (sizeof(digit) * 8);
    unsigned int num_bits_in_last_digit = b % (sizeof(digit) * 8);
    unsigned int required_digits = num_whole_digits + 1;
    unsigned int i;

    if(c->num_of_digit < num_whole_digits){
        CHECK_OKAY(bigint_expand(c, required_digits));
    }

    for(i = 0; i < num_whole_digits; i++){
        c->digits[i] = a->digits[i];
    }

    if(num_bits_in_last_digit){
        digit mask = (1UL << num_bits_in_last_digit) - 1;
        c->digits[num_whole_digits] = a->digits[num_whole_digits] & mask;
    }

    c->MSD = num_whole_digits; 
    return BIGINT_OKAY;
}

bigint_err bigint_and(const bigint *a, const bigint *b, bigint *c){
    if(!a || !b || !c) return BIGINT_ERROR_NULLPTR;

    bigint bigger, smaller;
    unsigned int i = 0;
    
    int r = bigint_cmp(a, b);
    if(r == -1){
        bigint_init(&bigger, b->num_of_digit);
        bigint_init(&smaller, a->num_of_digit);
        bigint_copy(a, &smaller);
        bigint_copy(b, &bigger);
    } else {
        bigint_init(&smaller, b->num_of_digit);
        bigint_init(&bigger, a->num_of_digit);
        bigint_copy(b, &smaller);
        bigint_copy(a, &bigger);
    }

    if(c->num_of_digit < bigger.num_of_digit){
        CHECK_OKAY(bigint_expand(c, bigger.num_of_digit));
    }

    for(; i < smaller.MSD + 1; i++){
        c->digits[i] = bigger.digits[i] & smaller.digits[i];
    }

    if(bigger.MSD > smaller.MSD){
        for(; i < bigger.MSD + 1; i++){
            // ANDed with zeros, since smaller bigint ran out of digits
            c->digits[i] = 0;  
        }
    }

    c->MSD = bigger.MSD;

    bigint_free(&bigger);
    bigint_free(&smaller);

    return BIGINT_OKAY;
}

bigint_err bigint_or(const bigint *a, const bigint *b, bigint *c){
    if(!a || !b || !c) return BIGINT_ERROR_NULLPTR;

    bigint bigger, smaller;
    unsigned int i = 0;
    
    int r = bigint_cmp(a, b);
    if(r == -1){
        bigint_init(&bigger, b->num_of_digit);
        bigint_init(&smaller, a->num_of_digit);
        bigint_copy(a, &smaller);
        bigint_copy(b, &bigger);
    } else {
        bigint_init(&smaller, b->num_of_digit);
        bigint_init(&bigger, a->num_of_digit);
        bigint_copy(b, &smaller);
        bigint_copy(a, &bigger);
    }

    if(c->num_of_digit < bigger.num_of_digit){
        CHECK_OKAY(bigint_expand(c, bigger.num_of_digit));
    }

    for(; i < smaller.MSD + 1; i++){
        c->digits[i] = bigger.digits[i] | smaller.digits[i];
    }

    if(smaller.MSD != bigger.MSD){
        for(; i < bigger.MSD + 1; i++){
            // ORed with zeros, since smaller bigint ran out of digits
            c->digits[i] = bigger.digits[i];  
        }
    }

    c->MSD = bigger.MSD;
    bigint_free(&bigger);
    bigint_free(&smaller);
    return BIGINT_OKAY;
}

bigint_err bigint_xor(const bigint *a, const bigint *b, bigint *c){
    if(!a || !b || !c) return BIGINT_ERROR_NULLPTR;

   bigint bigger, smaller;
    unsigned int i = 0;
    
    int r = bigint_cmp(a, b);
    if(r == -1){
        bigint_init(&bigger, b->num_of_digit);
        bigint_init(&smaller, a->num_of_digit);
        bigint_copy(a, &smaller);
        bigint_copy(b, &bigger);
    } else {
        bigint_init(&smaller, b->num_of_digit);
        bigint_init(&bigger, a->num_of_digit);
        bigint_copy(b, &smaller);
        bigint_copy(a, &bigger);
    }

    if(c->num_of_digit < bigger.num_of_digit){
        CHECK_OKAY(bigint_expand(c, bigger.num_of_digit));
    }

    for(; i < smaller.MSD + 1; i++){
        c->digits[i] = bigger.digits[i] ^ smaller.digits[i];
    }

    if(a->MSD != b->MSD){
        for(; i < bigger.MSD + 1; i++){
            // XORed with zeros, since smaller bigint ran out of digits
            c->digits[i] = bigger.digits[i] ^ 0;  
        }
    }
    
    c->MSD = bigger.MSD;
    bigint_free(&bigger);
    bigint_free(&smaller);

    return BIGINT_OKAY;
}

bigint_err bigint_not(const bigint *a, bigint *c){
    if(!a || !c) return BIGINT_ERROR_NULLPTR;

    if(c->num_of_digit < a->MSD + 1){
        CHECK_OKAY(bigint_expand(c, a->MSD + 1));
    }

    unsigned int i = 0;
    for(; i < a->MSD + 1; i++){
        c->digits[i] = ~a->digits[i];
    }

    c->MSD = i;
    return BIGINT_OKAY;
}

