#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 


typedef struct int24{
    int data : 24;
}int24;

/* number utilities */
#define IS_ODD(a) (a & 1)

int init_int24(uint8_t *a, int24 *num);


/* Statistics related stuff */
double score_text(char *text, int len);

/* String manipulations */
void print_hex(char *hex_str, int len);
int hex_to_bytes_conv(const char *input, int input_size, char *output);
int three_bytes_to_base64(int24 *num, char *output);
int bytes_to_base64(const char *input, int input_len, char *output);

int read_b64(const char *bytes, int len, char *output); // read bytes array into output buffer

#endif /* TOOLS_H */