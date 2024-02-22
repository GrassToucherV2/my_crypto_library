#include <stdio.h>
#include "util/bigint.h"
#include "util/tools.h"
void printBinary(int number) {
    unsigned int mask = 1 << (sizeof(int) * 8 - 1); // Create a mask with a 1 in the leftmost position

    for (int i = 0; i < sizeof(int) * 8; ++i) {
        putchar((number & mask) ? '1' : '0'); // Check if the current bit is 1 or 0
        mask >>= 1; // Move the mask one position to the right
    }
    printf("\n");
}



int main(){
    uint64_t a_digit = 12; 
    uint64_t b_digit = 23;
    int amsd = 10;
    int bmsd = 6;
    int max_asd = 10;
    int i = 8;
    if(i <= amsd){
        a_digit = 12;
    } else {
        a_digit = 0;
    }

    printf("1. a_digit = %ld\n", a_digit);
    a_digit &= ~( -(i > amsd) );
    printf("1. a_digit = %ld\n", a_digit);
    amsd = 0;
    if(i <= amsd){
        a_digit = 12;
    } else {
        a_digit = 0;
    }
    printf("2. a_digit = %lu\n", a_digit);
    a_digit &= ~( -(i > amsd) );
    printf("2. a_digit = %lu\n", a_digit);
    // int r = -1;
    // int a = r >> (sizeof(int) * 8 - 1) & 1;
    // printf("a = %d\n", a);
    // printBinary(r);

    // r = -0;
    // a = r >> (sizeof(int) * 8 - 1) & 1;
    // printf("a = %d\n", a);
    // printBinary(r);

    // r = 0;
    // a = r >> (sizeof(int) * 8 - 1) & 1;
    // printf("a = %d\n", a);

    int a = 0;
    a = BASE - 1;
    int64_t b = 2 - 10 + BASE;
    printf("%08X\n",a);
    printf("%08lX\n", b);

}