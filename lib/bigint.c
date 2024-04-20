#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../util/tools.h"
#include "bigint.h"
#include <time.h>

// static void print_bigint_ctx(const bigint *a){
//     printf("MSD              = %u\n", a->MSD);
//     printf("number of digits = %u\n", a->num_of_digit);
// }

bigint_err bigint_init(bigint *b, unsigned int num_of_digit){
    b->digits = (digit *)malloc(num_of_digit * sizeof(digit));
    if(!b->digits){
        printf("malloc failed in bigint_init\n");
        return BIGINT_REALLOC_FAILURE;
    }
    memset_s(b->digits, 0, num_of_digit * sizeof(digit));
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

    unsigned msd = a->MSD;
    digit *tmp = (digit *)malloc(num * sizeof(digit));
    if(!tmp)
        return BIGINT_REALLOC_FAILURE;
    memset_s(tmp, 0, num * sizeof(digit));
    memcpy(tmp, a->digits, a->num_of_digit * sizeof(digit));
    bigint_free(a);

    a->MSD = msd;
    a->digits = tmp;
    a->num_of_digit = num;
    return BIGINT_OKAY;
}

bigint_err bigint_set_zero(bigint *a){
    if(!a) return BIGINT_ERROR_NULLPTR;
    memset_s(a->digits, 0, a->num_of_digit * sizeof(digit));
    a->MSD = 0;
    // bigint_clamp(a);
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
        CHECK_OKAY(bigint_expand(dest, src->MSD + 1));
    }
    // CHECK_OKAY(bigint_set_zero(dest));
    
    memcpy(dest->digits, src->digits, (src->MSD + 1) * sizeof(digit));
    dest->MSD = src->MSD;

    return BIGINT_OKAY;
}

void bigint_swap(bigint *a, bigint *b){
    printf("bigint_swap\n");
    bigint tmp;
    bigint_init(&tmp, a->num_of_digit);
    bigint_copy(a, &tmp);
    a = b;
    b = &tmp;
    bigint_free(&tmp);
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

    if(msd >= a->num_of_digit){
        CHECK_OKAY(bigint_expand(a, msd + 1));
    }

    for(unsigned int i = a->MSD + 1; i <= msd; i++){
        a->digits[i] = 0;
    }
    // memset_s(&a->digits[a->MSD + 1], 0, (msd - a->MSD) * sizeof(digit));
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
   This function compares two bigints returns 0 if a == b, -1 if a < b, 1 if a > b
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
        
        // set positive_c1_seen flag if we have seen a positive, non zero c1
        // if(!c1_sign && c != 0){
        //     positive_c1_seen_flag = 1;
        // }
        // ANDing with c will reset the flag if c1 is 0
        // the conditional !(c == 0) is fine hopefully as it is an accumulator of the difference between a and b's digit, so it is all jumbled up
        positive_c1_seen_flag |= (~c1_sign & 1) & !(c == 0);

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

int bigint_cmp_noCT(const bigint *a, const bigint *b){
    if(!a || !b) return BIGINT_ERROR_NULLPTR;

    bigint tmp_a, tmp_b;
    CHECK_OKAY(bigint_init(&tmp_a, a->num_of_digit));
    CHECK_OKAY(bigint_init(&tmp_b, b->num_of_digit));

    CHECK_OKAY(bigint_copy(a, &tmp_a));
    CHECK_OKAY(bigint_copy(b, &tmp_b));

    CHECK_OKAY(bigint_clamp(&tmp_a));
    CHECK_OKAY(bigint_clamp(&tmp_b));

    if (tmp_a.MSD > tmp_b.MSD) return 1;
    if (tmp_a.MSD < tmp_b.MSD) return -1;
    
    // If MSDs are equal, compare the digits from most significant to least significant
    for (int i = a->MSD; i >= 0; i--) {
        if (tmp_a.digits[i] > tmp_b.digits[i]) return 1;
        if (tmp_a.digits[i] < tmp_b.digits[i]) return -1;
    }
    
    bigint_free(&tmp_a);
    bigint_free(&tmp_b);
    // If all digits are equal, the numbers are equal
    return 0;
}

bigint_err bigint_left_bit_shift(const bigint *a, bigint *c){
    if(!a || !c) return BIGINT_ERROR_NULLPTR;

    // making sure c has at least one more digit than a
    if(c->num_of_digit < a->MSD + 2){
        CHECK_OKAY(bigint_expand(c, a->MSD + 2));
    }

    int carry = 0;
    int next_carry = 0;
    unsigned int i = 0;

    for(; i <= a->MSD; i++){
        next_carry = a->digits[i] >> ((sizeof(digit) * 8) - 1);
        c->digits[i] = (a->digits[i] << 1) | carry;
        // the MSB of each digit will be promoted and become the 
        // LSB of the next digit as a result of left shift
        carry = next_carry;
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
    
    if(b == 0) 
        return BIGINT_OKAY;
    
    // unsigned int num_digits_to_shift = b / DIGIT_BIT;
    // unsigned int num_bits_to_shift = b % DIGIT_BIT;

    // printf("b = %u\n", b);
    // printf("num_digits = %u\n", num_digits_to_shift);
    // printf("num_bits = %u\n", num_bits_to_shift);
    
    if(a->num_of_digit < a->MSD + 2){
        CHECK_OKAY(bigint_expand(a, a->MSD + 2));
    }

    // CHECK_OKAY(bigint_left_shift_digits(a, num_digits_to_shift));

    digit carry = 0;
    digit current_digit = 0;
    for(unsigned int i = 0; i <= a->MSD; i++){
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
        if(len >= sizeof(digit))
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
    
    memset_s(output, 0, len);

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

// this is a bad PRNG, crypt_gen_rand() should be used instead, it is in /util/tool.c
// which utilizes linux's /dev/urandom 
bigint_err bigint_random(bigint *a, unsigned int num_digits){
    if(!a) return BIGINT_ERROR_NULLPTR;

    if(num_digits <= 0){
        return BIGINT_OKAY;
    }

    digit d = 0;
    CHECK_OKAY(bigint_set_zero(a));

    time_t t;
    srand((unsigned int)time(&t));

    do{
        d = (digit) abs(rand()) & BASE;
    } while(d == 0);

    CHECK_OKAY(bigint_add_digit(a, d, a));

    while(--num_digits > 0){
        CHECK_OKAY(bigint_right_shift_digits(a, 1));
        CHECK_OKAY(bigint_add_digit(a, (digit)abs(rand()), a));
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

bigint_err bigint_inc(bigint *a){
    if(!a) return BIGINT_ERROR_NULLPTR;

    if(a->num_of_digit < a->MSD + 2){
        CHECK_OKAY(bigint_expand(a, a->MSD + 2));
    }
    int64_t tmp;
    unsigned int carry = 0;
    for(unsigned int i = 0; i < a->MSD + 1; i++){
        tmp = (int64_t)a->digits[i] + 1 + carry;
        a->digits[i] = tmp & BASE;
        carry = (tmp >> DIGIT_BIT) & 1;
    }

    if(carry){
        a->digits[a->MSD + 1] = carry;
        a->MSD++;
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
    bigint bigger, smaller;
    
    int r = bigint_cmp(a, b);
    if(r){
        max_msd = a->MSD;
        min_msd = b->MSD;
        CHECK_OKAY(bigint_init(&bigger, a->num_of_digit));
        CHECK_OKAY(bigint_init(&smaller, b->num_of_digit));
        CHECK_OKAY(bigint_copy(a, &bigger));
        CHECK_OKAY(bigint_copy(b, &smaller));
        
    } else {
        max_msd = b->MSD;
        min_msd = a->MSD;
        CHECK_OKAY(bigint_init(&bigger, b->num_of_digit));
        CHECK_OKAY(bigint_init(&smaller, a->num_of_digit));
        CHECK_OKAY(bigint_copy(a, &smaller));
        CHECK_OKAY(bigint_copy(b, &bigger));
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
        tmp = (int64_t)bigger.digits[i] - (int64_t)smaller.digits[i] - (int64_t)borrowed;
        sign = tmp >> (sizeof(int64_t) * 8 - 1) & 1;
        if(sign){
            base = BASE + 1;
        } else {
            base = 0;
        }
        c->digits[i] = (tmp + base) & BASE;
        borrowed = sign;
    }

    if(min_msd != max_msd){
        for(; i <= max_msd; i++){
            tmp = bigger.digits[i] - borrowed;
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
    bigint_free(&bigger);
    bigint_free(&smaller);
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
    The elementary school multiplication algorithm
    from wikipedia
    Computes c = a * b
*/
bigint_err bigint_mul(const bigint *a, const bigint *b, bigint *c){
    if(!a || !b || !c) return BIGINT_ERROR_NULLPTR;

    unsigned int a_digits_used = a->MSD + 1;
    unsigned int b_digits_used = b->MSD + 1;

    if(c->num_of_digit != a_digits_used + b_digits_used){
        CHECK_OKAY(bigint_expand(c, a_digits_used + b_digits_used));
    }
    // bigint_print(c, "c in bigint_mul before setting to zero ");
    // print_bigint_ctx(c);
    // CHECK_OKAY(bigint_set_zero(c));
    // bigint_print(c, "after setting to zero ");
    // print_bigint_ctx(c);

    bigint tmp_res;
    CHECK_OKAY(bigint_init(&tmp_res, c->num_of_digit));
    CHECK_OKAY(bigint_set_zero(&tmp_res));

    uint64_t tmp = 0;
    uint32_t carry = 0;
    unsigned int i = 0;
    for(; i < a_digits_used; i++){
        carry = 0;
        for(unsigned int j = 0; j < b_digits_used; j++){
            // printf("c at i = %u, j = %u is %u\n",i, j, c->digits[i + j]);
            tmp = (uint64_t)a->digits[i] * (uint64_t)b->digits[j] + tmp_res.digits[i + j] + carry;
            tmp_res.digits[i + j] = tmp & BASE;  
            carry = tmp >> DIGIT_BIT;
        }
        tmp_res.digits[i + b_digits_used] = carry;
    }
    
    tmp_res.MSD = a_digits_used + b_digits_used - 1;

    while(tmp_res.MSD > 0 && tmp_res.digits[tmp_res.MSD] == 0) tmp_res.MSD--;

    CHECK_OKAY(bigint_copy(&tmp_res, c));
    bigint_free(&tmp_res);

    return BIGINT_OKAY;
}

/*
    Simpler version of the Karatsuba algorithm as it only splits the bigint once
    rather than doing it recursively
    Computes c = a * b
*/
bigint_err bigint_mul_karatsuba(const bigint *a, const bigint *b, bigint *c){
    if(!a || !b || !c) return BIGINT_ERROR_NULLPTR;

    bigint x0, x1, y0, y1, t1, x0y0, x1y1, x1plusx0, y1plusy0;
    unsigned int B;

    // min number of digits 
    B = MIN(a->MSD + 1, b->MSD + 1);
    // divide B by 2
    B >>= 1;

    CHECK_OKAY(bigint_init(&x0, B));
    CHECK_OKAY(bigint_init(&x1, a->MSD + 1 - B));
    CHECK_OKAY(bigint_init(&y0, B));
    CHECK_OKAY(bigint_init(&y1, b->MSD + 1 - B));
    CHECK_OKAY(bigint_init(&t1, 2 * B));
    CHECK_OKAY(bigint_init(&x0y0, 2 * B));
    CHECK_OKAY(bigint_init(&x1y1, 2 * B));
    CHECK_OKAY(bigint_init(&x1plusx0, B + 1));
    CHECK_OKAY(bigint_init(&y1plusy0, B + 1));

    // shift the digits
    x0.MSD = x0.num_of_digit - 1;
    y0.MSD = y0.num_of_digit - 1;
    x1.MSD = x1.num_of_digit - 1;
    y1.MSD = y1.num_of_digit - 1;

    memcpy(x0.digits, a->digits, B * sizeof(digit));
    memcpy(y0.digits, b->digits, B * sizeof(digit));
    memcpy(x1.digits, a->digits + B, (a->MSD + 1 - B) * sizeof(digit));
    memcpy(y1.digits, b->digits + B, (b->MSD + 1 - B) * sizeof(digit));

    bigint_clamp(&x0);
    bigint_clamp(&y0);
    
    // x0y0 = x0 * y0
    // x1y1 = x1 * y1
    CHECK_OKAY(bigint_mul(&x0, &y0, &x0y0));
    CHECK_OKAY(bigint_mul(&x1, &y1, &x1y1));

    // computing x1 + x0 and y1 + y0
    CHECK_OKAY(bigint_add(&x1, &x0, &x1plusx0));
    CHECK_OKAY(bigint_add(&y1, &y0, &y1plusy0));
    
    // computing t1 = (x1 + x0) * (y1 + y0)
    CHECK_OKAY(bigint_mul(&y1plusy0, &x1plusx0, &t1));

    // computing t1 = (x1 + x0) * (y1 + y0) - x0y0 - x1y1
    CHECK_OKAY(bigint_sub(&t1, &x0y0, &t1));
    CHECK_OKAY(bigint_sub(&t1, &x1y1, &t1));

    // multiply by BASE and BASE/2 
    CHECK_OKAY(bigint_left_shift_digits(&t1, B));
    CHECK_OKAY(bigint_left_shift_digits(&x1y1, B * 2));

    // adding the results
    CHECK_OKAY(bigint_add(&t1, &x0y0, &t1));
    CHECK_OKAY(bigint_add(&t1, &x1y1, c));

    bigint_free(&x0);
    bigint_free(&x1);
    bigint_free(&y0);
    bigint_free(&y1);
    bigint_free(&t1);
    bigint_free(&x0y0);
    bigint_free(&x1y1);
    bigint_free(&x1plusx0);
    bigint_free(&y1plusy0);

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

/* This function is not constant-time, more work needed */
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

bigint_err bigint_square(const bigint *a, bigint *c){
    if(!a || !c) return BIGINT_ERROR_NULLPTR;

    CHECK_OKAY(bigint_mul_karatsuba(a, a, c));

    return BIGINT_OKAY;
}

/*
    This function performs integer division, returns the quotient in q and remainder in r
    optimized version of repeated subtraction.
    First set r to the dividend, then repeatedly double b until it becomes greater than or
    equal to r, then we halve it and subtract it from r.
    Repeat this process until r < b.

    This function allows q to be NULL

    better division and modular algorithm needed 
*/
// let r = a
// let count = 0
// while r >= b
//     tmp_b = b
//     tmp_q = 1
//     while tmp_b <= r
//         tmp_b *= 2;
//         count++
    
//     if tmp_b > r
//         tmp_b /= 2
//         count --
    
//     r -= tmp_b
//     tmp_q <<= count // tmp_q = 2^count
//     q += tmp_q
bigint_err bigint_div(const bigint *a, const bigint *b, bigint *q, bigint *r) {
    if(!a || !b || !r) return BIGINT_ERROR_NULLPTR;

    if(!bigint_cmp_zero(b)){
        return BIGINT_ERROR_DIVIDE_BY_ZERO;
    }

    if(q && q->num_of_digit <= a->MSD + 1){
        CHECK_OKAY(bigint_expand(q, a->MSD + 1));
    }
    if(r && r->num_of_digit <= a->MSD + 1){
        CHECK_OKAY(bigint_expand(r, a->MSD + 1));
    }

    bigint tmp_b, tmp_q;
    CHECK_OKAY(bigint_init(&tmp_b, b->num_of_digit + 1));
    if(q){
        CHECK_OKAY(bigint_set_zero(q));
        CHECK_OKAY(bigint_init(&tmp_q, q->num_of_digit + 1));
    }
    // if(r){ 
    //     CHECK_OKAY(bigint_set_zero(r));
    // }

    CHECK_OKAY(bigint_copy(a, r));
    int count = 0;    
    
    // while r >= b
    while(bigint_cmp(r, b) != -1){
        CHECK_OKAY(bigint_copy(b, &tmp_b));

        if(q){
            CHECK_OKAY(bigint_from_small_int(&tmp_q, 1));
        }
        
        // while r >= tmp_b
        while(bigint_cmp(r, &tmp_b) != -1){
            CHECK_OKAY(bigint_double(&tmp_b, &tmp_b)); 
            count++;
        }
        
        // while r < tmp_b
        while(bigint_cmp(r, &tmp_b) == -1){
            CHECK_OKAY(bigint_halve(&tmp_b, &tmp_b));
            count--;
        }
        // one of the lines above causes MSD to be aligned to leading zeros
        // which causes bigint_sub confusion and subsequently, read/write out of bound
        CHECK_OKAY(bigint_clamp(r));
        CHECK_OKAY(bigint_clamp(&tmp_b));
        CHECK_OKAY(bigint_sub(r, &tmp_b, r));
        if(q){
            CHECK_OKAY(bigint_mul_pow_2(&tmp_q, count, &tmp_q));
            CHECK_OKAY(bigint_add(q, &tmp_q, q));
            CHECK_OKAY(bigint_set_zero(&tmp_q));
        }

        count = 0;
    }

    bigint_free(&tmp_b);
    if(q){
        bigint_free(&tmp_q);
    }
    return BIGINT_OKAY;
}

int bigint_bits_count(const bigint *a){
    if(!a) return BIGINT_ERROR_NULLPTR;

    // (number of digits - 1) * 32 bits 
    int bits_count = a->MSD * DIGIT_BIT;

    digit last_digit = a->digits[a->MSD];
    while(last_digit != 0){
        last_digit >>= 1;
        bits_count++;
    }

    return bits_count;

}

bigint_err bigint_halve(const bigint *a, bigint *c){
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

bigint_err bigint_mod(const bigint *a, const bigint *b, bigint *r){
    if(!a || !b || !r) return BIGINT_ERROR_NULLPTR;

    CHECK_OKAY(bigint_div(a, b, NULL, r));

    return BIGINT_OKAY;
}

/*
    this is essentially bitwise AND with 2^b - 1
    this function is not constant-time
*/
bigint_err bigint_mod_pow_2(const bigint *a, unsigned int b, bigint *c){
    if(!a || !c) return BIGINT_ERROR_NULLPTR;

    if(b >= (a->MSD + 1) * (sizeof(digit) * 8)){
        CHECK_OKAY(bigint_copy(a, c));
        return BIGINT_OKAY;
    }

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

    digit mask = (1UL << num_bits_in_last_digit) - 1;
    c->digits[num_whole_digits] = a->digits[num_whole_digits] & mask;    

    c->MSD = num_whole_digits; 
    return BIGINT_OKAY;
}

bigint_err bigint_mul_mod(const bigint *a, const bigint *b, const bigint *m, bigint *c){
    if(!a || !b || !m || !c) return BIGINT_ERROR_NULLPTR;

    if(!bigint_cmp_zero(m)){
        return BIGINT_ERROR_DIVIDE_BY_ZERO;
    }

    // bigint_mul doesn't work when c == a
    bigint res;
    CHECK_OKAY(bigint_init(&res, a->num_of_digit + b->num_of_digit));

    CHECK_OKAY(bigint_mul(a, b, &res));
    CHECK_OKAY(bigint_mod(&res, m, c));

    bigint_free(&res);
    return BIGINT_OKAY;
}

bigint_err bigint_square_mod(const bigint *a, const bigint *m, bigint *c){
    if(!a || !m || !c) return BIGINT_ERROR_NULLPTR;

    if(!bigint_cmp_zero(m)){
        return BIGINT_ERROR_DIVIDE_BY_ZERO;
    }

    CHECK_OKAY(bigint_square(a, c));
    CHECK_OKAY(bigint_mod(c, m, c));

    return BIGINT_OKAY;
}

/* 
    Right-to-left shift binary method, algorithm based on Applied Cryptography by Bruce Schneier
    https://en.wikipedia.org/wiki/Modular_exponentiation
*/
bigint_err bigint_expt_mod(const bigint *a, const bigint *e, const bigint *m, bigint *c){
    if(!a || !e || !m || !c) return BIGINT_ERROR_NULLPTR;

    if(!bigint_cmp_zero(m)){
        return BIGINT_ERROR_DIVIDE_BY_ZERO;
    }

    if(c->num_of_digit < m->MSD + 1){
        CHECK_OKAY(bigint_expand(c, m->MSD + 1));
    }

    CHECK_OKAY(bigint_set_zero(c));
    
    // if m = 1, then the result is always 0
    bigint one, two;
    CHECK_OKAY(bigint_init(&one, 1));
    CHECK_OKAY(bigint_init(&two, 2));
    CHECK_OKAY(bigint_from_small_int(&one, 1));
    CHECK_OKAY(bigint_from_small_int(&two, 2));
    if(!bigint_cmp(&one, m)){
        return BIGINT_OKAY;    
    }

    bigint base, exponent, res;
    CHECK_OKAY(bigint_init(&base, a->num_of_digit));
    CHECK_OKAY(bigint_init(&exponent, e->num_of_digit));
    CHECK_OKAY(bigint_init(&res, e->num_of_digit));
    CHECK_OKAY(bigint_copy(a, &base));
    CHECK_OKAY(bigint_copy(e, &exponent));

    CHECK_OKAY(bigint_from_small_int(c, 1));
    CHECK_OKAY(bigint_mod(&base, m, &base));
    // while exponent > 0, this should be okay even if digits are unsigned
    // due to exponent being shifted by 1 bit at a time, so exponent will have to be 0
    // which will terminate the loop before underflow happens
    while(bigint_cmp_zero(&exponent)){
        CHECK_OKAY(bigint_mod(&exponent, &two, &res));
        // if res = 1
        if(!bigint_cmp(&res, &one)){
            CHECK_OKAY(bigint_mul_mod(c, &base, m, c));
        }

        CHECK_OKAY(bigint_halve(&exponent, &exponent));
        CHECK_OKAY(bigint_square_mod(&base, m, &base));
    }

    bigint_free(&one);
    bigint_free(&two);
    bigint_free(&base);
    bigint_free(&res);
    bigint_free(&exponent);

    return BIGINT_OKAY;
}

/*
    This gcd functions implements Euclidean algorithm to finid the gcd(a, b)
*/
bigint_err bigint_gcd(const bigint *a, const bigint *b, bigint *c){
    if(!a || !b || !c) return BIGINT_ERROR_NULLPTR;

    if(c->num_of_digit <= a->MSD + 1){
        CHECK_OKAY(bigint_expand(c, a->MSD + 1));
    }

    bigint tmp_a, tmp_b, tmp_r;
    CHECK_OKAY(bigint_init(&tmp_a, a->num_of_digit));
    CHECK_OKAY(bigint_init(&tmp_b, b->num_of_digit));
    CHECK_OKAY(bigint_init(&tmp_r, a->num_of_digit));

    CHECK_OKAY(bigint_copy(a, &tmp_a));
    CHECK_OKAY(bigint_copy(b, &tmp_b));
    // while b != 0
    while(bigint_cmp_zero(&tmp_b)){
        CHECK_OKAY(bigint_mod(&tmp_a, &tmp_b, &tmp_r)); 
        CHECK_OKAY(bigint_copy(&tmp_b, &tmp_a)); 
        CHECK_OKAY(bigint_copy(&tmp_r, &tmp_b)); 
    }

    CHECK_OKAY(bigint_copy(&tmp_a, c));

    bigint_free(&tmp_a);
    bigint_free(&tmp_b);
    bigint_free(&tmp_r);
    return BIGINT_OKAY;
}

// lcm(a, b) = (a * b) // gcd(a, b), where // denotes integer division
bigint_err bigint_lcm(const bigint *a, const bigint *b, bigint *c){
    if(!a || !b || !c) return BIGINT_ERROR_NULLPTR;

    if(c->num_of_digit != a->num_of_digit + b->num_of_digit){
        CHECK_OKAY(bigint_expand(c, a->num_of_digit + b->num_of_digit));
    }

    bigint gcd, remainder, quotient;
    CHECK_OKAY(bigint_init(&gcd, a->num_of_digit));
    CHECK_OKAY(bigint_init(&remainder, b->num_of_digit));
    CHECK_OKAY(bigint_init(&quotient, a->num_of_digit));

    CHECK_OKAY(bigint_mul_karatsuba(a, b, c));
    CHECK_OKAY(bigint_gcd(a, b, &gcd));
    CHECK_OKAY(bigint_div(c, &gcd, &quotient, &remainder));

    CHECK_OKAY(bigint_copy(&quotient, c));

    bigint_free(&gcd);
    bigint_free(&remainder);
    bigint_free(&quotient);

    return BIGINT_OKAY;
}

// not used, but keep it here in case it is needed in the future
// bigint_err bigint_extended_gcd(const bigint *a, const bigint *m, bigint *gcd, bigint *x){
//     if (!a || !m || !gcd || !x) return BIGINT_ERROR_NULLPTR;

//     bigint x0, x1, y0, y1;
//     bigint b;
//     bigint q;
//     bigint r;
//     bigint tmp;

//     CHECK_OKAY(bigint_init(&x0, 1));
//     CHECK_OKAY(bigint_init(&x1, 1));
//     CHECK_OKAY(bigint_init(&y0, 1));
//     CHECK_OKAY(bigint_init(&y1, 1));
//     CHECK_OKAY(bigint_init(&tmp, a->num_of_digit));
//     CHECK_OKAY(bigint_init(&b, m->num_of_digit));
//     CHECK_OKAY(bigint_init(&q, m->num_of_digit));
//     CHECK_OKAY(bigint_init(&r, m->num_of_digit));

//     CHECK_OKAY(bigint_from_small_int(&x0, 1));
//     CHECK_OKAY(bigint_from_small_int(&x1, 0));
//     CHECK_OKAY(bigint_from_small_int(&y0, 0));
//     CHECK_OKAY(bigint_from_small_int(&x0, 1));
//     CHECK_OKAY(bigint_copy(m, &b));
//     CHECK_OKAY(bigint_copy(a, gcd));

//     // while tmp_m != 0
//     while(bigint_cmp_zero(&b)){
//         CHECK_OKAY(bigint_div(gcd, &b, &q, &r));    // q = a // b, r = a % b 
//         CHECK_OKAY(bigint_copy(&b, gcd));           // gcd or a = b 
//         CHECK_OKAY(bigint_copy(&r, &b));            // b = r

//         CHECK_OKAY(bigint_copy(&x1, &x0));          // x0 = x1
//         CHECK_OKAY(bigint_mul(&q, &x1, &tmp));      // tmp = q * x1
//         CHECK_OKAY(bigint_sub(&x0, &tmp, &x1));     // x1 = x0 - q * x1

//         CHECK_OKAY(bigint_copy(&y1, &y0));          // y0 = y1
//         CHECK_OKAY(bigint_mul(&q, &y1, &tmp));      // tmp = q * y1
//         CHECK_OKAY(bigint_sub(&y0, &tmp, &y1));     // y1 = y0 - q * y1

        
//     }       

//     CHECK_OKAY(bigint_copy(&x0, x));

//     bigint_free(&x0);
//     bigint_free(&x1);
//     bigint_free(&y0);
//     bigint_free(&y1);
//     bigint_free(&b);
//     bigint_free(&q);
//     bigint_free(&r);
//     bigint_free(&tmp);

//     return BIGINT_OKAY;
// }

// temporarty struct used to handle modular inverse 
// https://stackoverflow.com/questions/53560302/modular-inverses-and-unsigned-integers
// Maybe I should move this to the header? Perhaps other future functions will need it
typedef struct signed_bigint{
    bigint bi;
    bool sign;    // 1 = negative, 0 = positive
} signed_bigint;

bigint_err bigint_inverse_mod(const bigint *a, const bigint *m, bigint *c){
    if (!a || !m || !c) return BIGINT_ERROR_NULLPTR;

    if (c->num_of_digit != a->num_of_digit) {
        CHECK_OKAY(bigint_expand(c, m->num_of_digit));
    }

    bigint one, gcd;
    CHECK_OKAY(bigint_init(&one, 1));
    CHECK_OKAY(bigint_init(&gcd, 1));
    CHECK_OKAY(bigint_from_small_int(&one, 1));

    CHECK_OKAY(bigint_gcd(a, m, &gcd));
    // If GCD is not 1, modular multiplicative inverse doesn't exist
    if (bigint_cmp(&gcd, &one) != 0) { 
        bigint_free(&gcd);
        bigint_free(&one);
        return BIGINT_ERROR_MUL_INVERSE_DOES_NOT_EXIST;
    } 
    
    bigint_free(&gcd);

    signed_bigint x0;
    signed_bigint x1;
    x0.sign = 0;
    x1.sign = 0;
    CHECK_OKAY(bigint_init(&x0.bi, 1));
    CHECK_OKAY(bigint_init(&x1.bi, 1));
    CHECK_OKAY(bigint_from_small_int(&x0.bi, 0));
    CHECK_OKAY(bigint_from_small_int(&x1.bi, 1));

    bigint tmp_m;   // tmp var for m
    bigint tmp_a;   // tmp var for a
    bigint q;   // q = tmp_a // tmp_m
    bigint r;   // r = tmp_a % tmp_m
    bigint t;   // tmp var to hold intermediate values 
    bigint qx0;   // qx0 = q * x0 

    CHECK_OKAY(bigint_init(&tmp_m, m->num_of_digit));
    CHECK_OKAY(bigint_init(&tmp_a, a->num_of_digit));
    CHECK_OKAY(bigint_init(&q, a->num_of_digit));
    CHECK_OKAY(bigint_init(&r, m->num_of_digit));
    CHECK_OKAY(bigint_init(&t, m->num_of_digit));
    CHECK_OKAY(bigint_init(&qx0, m->num_of_digit));

    CHECK_OKAY(bigint_copy(a, &tmp_a));
    CHECK_OKAY(bigint_copy(m, &tmp_m));

    signed_bigint t2;
    CHECK_OKAY(bigint_init(&t2.bi, x0.bi.num_of_digit));

    while(bigint_cmp(&tmp_a, &one) == 1){
        CHECK_OKAY(bigint_div(&tmp_a, &tmp_m, &q, &r));
        CHECK_OKAY(bigint_copy(&tmp_m, &t));
        CHECK_OKAY(bigint_copy(&r, &tmp_m));
        CHECK_OKAY(bigint_copy(&t, &tmp_a));
        
        CHECK_OKAY(bigint_copy(&x0.bi, &t2.bi));
        t2.sign = x0.sign;

        CHECK_OKAY(bigint_mul(&q, &x0.bi, &qx0));
        if(x0.sign != x1.sign){
            CHECK_OKAY(bigint_add(&x1.bi, &qx0, &x0.bi));
            x0.sign = x1.sign;
        } else{
            if(bigint_cmp(&x1.bi, &qx0) == 1){
                CHECK_OKAY(bigint_sub(&x1.bi, &qx0, &x0.bi));
                x0.sign = x1.sign;
            } else {
                CHECK_OKAY(bigint_sub(&qx0, &x1.bi, &x0.bi));
                x0.sign = !x1.sign;
            }
        }
        CHECK_OKAY(bigint_copy(&t2.bi, &x1.bi));
        x1.sign = t2.sign;
    }
    
    if(x1.sign){
        CHECK_OKAY(bigint_sub(m, &x1.bi, c));
    } else {
        CHECK_OKAY(bigint_copy(&x1.bi, c));
    }

    bigint_free(&tmp_m);
    bigint_free(&tmp_a);
    bigint_free(&q);
    bigint_free(&t);
    bigint_free(&r);
    bigint_free(&qx0);
    bigint_free(&one);
    bigint_free(&x0.bi);
    bigint_free(&x1.bi);
    bigint_free(&t2.bi);
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

