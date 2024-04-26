import secrets
import sys
import math
import hashlib
import os
from Crypto.Cipher import DES, DES3, AES
from Crypto.Util import Counter
# from Crypto.Random import get_random_bytes

def extended_gcd(a, b):
    x0, x1, y0, y1 = 1, 0, 0, 1
    while b != 0:
        q, a, b = a // b, b, a % b
        x0, x1 = x1, x0 - q * x1
        y0, y1 = y1, y0 - q * y1
    return a, x0, y0

def mod_inverse(a, m):
    gcd, x, _ = extended_gcd(a, m)
    if gcd != 1:
        return 0  # Modular inverse does not exist if a and m are not coprime
    else:
        return x % m
    
def md5_hash(data):
    m = hashlib.md5()
    m.update(data)
    return int(m.hexdigest(), 16)

def print_hex_format(number, line_length=8):
    # Convert the number to a hexadecimal string, removing the '0x' prefix and padding with zeros to ensure byte alignment
    hex_str = format(number, 'x').zfill((number.bit_length() + 7) // 8 * 2)  # Ensure byte alignment
    # Insert spaces and format as bytes
    formatted_str = ', '.join('0x' + hex_str[i:i+2].upper() for i in range(0, len(hex_str), 2))
    # Print with line breaks after every 'line_length' bytes
    for i in range(0, len(formatted_str), line_length * 6):  # 6 accounts for the length of '0xXX, '
        print(formatted_str[i:i + line_length * 6])
    print("\n")

# Generate a random number with the given number of bits
def generate_random_numbers(bits):
    return secrets.randbits(bits)


def bytes_to_int(bytes_value, endian="big"):
    return int.from_bytes(bytes_value, byteorder=endian)


def des_ecb():
    keys = [
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),
        bytes([0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70]),
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),
    ]
    # Generate test vectors
    test_vectors = [
        bytes([0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74]),

        bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 
                0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00]),
        
        bytes([
            0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
            0x68, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20, 
            0x66, 0x6F, 0x72, 0x20, 0x61, 0x6C, 0x6C, 0x00
        ])
    ]
    for key, plaintext in zip(keys, test_vectors):
        cipher = DES.new(key, DES.MODE_ECB)
        ciphertext = cipher.encrypt(plaintext)
        print("key")
        print_hex_format(bytes_to_int(key))
        print("plaintext")
        print_hex_format(bytes_to_int(plaintext))
        print("ciphertext")
        print_hex_format(bytes_to_int(ciphertext))
    
    return ' '


def pkcs7_padding(data, block_size):
    padding_len = block_size - len(data) % block_size
    if padding_len == block_size:
        return data
    padding = bytes([padding_len] * padding_len)
    return data + padding


def des_cbc():
    keys = [
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),
        bytes([0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70]),
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),
    ]
    ivs = [
        bytes([0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]),  # Example IVs
        bytes([0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF]),
        bytes([0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10]),
        bytes([0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10]),
    ]
    # Generate test vectors
    test_vectors = [
        bytes([0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74]),

        bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 
                0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00]),
        
        bytes([
            0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
            0x68, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20, 
            0x66, 0x6F, 0x72, 0x20, 0x61, 0x6C, 0x6C, 0x00
        ]),
        bytes([
            0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
            0x68, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20, 
            0x66, 0x6F, 0x72, 0x20, 0x61, 0x6C, 0x6C, 0x00,
            0x66, 0x6F, 0x72, 0x20, 0x61,
        ]),
    ]
    for iv, key, plaintext in zip(ivs, keys, test_vectors):
        cipher = DES.new(key, DES.MODE_CBC, iv)
        ciphertext = cipher.encrypt(pkcs7_padding(plaintext, 8))
        print("IV")
        print_hex_format(bytes_to_int(iv))
        print("key")
        print_hex_format(bytes_to_int(key))
        print("plaintext")
        print_hex_format(bytes_to_int(pkcs7_padding(plaintext, 8)))
        print("ciphertext")
        print_hex_format(bytes_to_int(ciphertext))
        print("================================================")
    
    return ' '


def tdes_ecb():
    keys = [
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 
               0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),
        
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 
               0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),
    ]
    # Generate test vectors
    test_vectors = [
        bytes([0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74]),

        # bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 
        #         0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00]),
        
        # bytes([
        #     0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
        #     0x68, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20, 
        #     0x66, 0x6F, 0x72, 0x20, 0x61, 0x6C, 0x6C, 0x00
        # ]),
        bytes([
            0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
            0x68, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20, 
            0x66, 0x6F, 0x72, 0x20, 0x61, 0x6C, 0x6C, 0x00
        ]),
    ]

    for key, plaintext in zip(keys, test_vectors):
        cipher = DES3.new(key, DES3.MODE_ECB)
        ciphertext = cipher.encrypt(pkcs7_padding(plaintext, 8))
        print("key")
        print_hex_format(bytes_to_int(key))
        print("plaintext")
        print_hex_format(bytes_to_int(pkcs7_padding(plaintext, 8)))
        print("ciphertext")
        print_hex_format(bytes_to_int(ciphertext))
        print("================================================")
    
    return ' '

def tdes_cbc():
    keys = [
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 
               0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),
        
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 
               0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),
        
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 
               0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),
    ]

    ivs = [
        bytes([0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]),  # Example IVs
        bytes([0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF]),
        bytes([0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF]),
    ]

    # Generate test vectors
    test_vectors = [
        bytes([0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74]),

        # bytes([0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 
        #         0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00]),
        
        # bytes([
        #     0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
        #     0x68, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20, 
        #     0x66, 0x6F, 0x72, 0x20, 0x61, 0x6C, 0x6C, 0x00
        # ]),
        bytes([
            0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
            0x68, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20, 
            0x66, 0x6F, 0x72, 0x20, 0x61, 0x6C, 0x6C, 0x00
        ]),
        bytes([
            0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
            0x68, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20, 
            0x66, 0x6F, 0x72, 0x20, 0x61, 0x6C, 0x6C, 0x00,
            0x66, 0x6F, 0x72, 0x20, 0x61,
        ]),
    ]

    for iv, key, plaintext in zip(ivs, keys, test_vectors):
        cipher = DES3.new(key, DES3.MODE_CBC, iv)
        ciphertext = cipher.encrypt(pkcs7_padding(plaintext, 8))
        print("IV")
        print_hex_format(bytes_to_int(iv))
        print("key")
        print_hex_format(bytes_to_int(key))
        print("plaintext")
        print_hex_format(bytes_to_int(pkcs7_padding(plaintext, 8)))
        print("ciphertext")
        print_hex_format(bytes_to_int(ciphertext))
        print("================================================")
    
    return ' '


def aes_cbc():
    keys = [
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),  # 128-bit key
        
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),  # 128-bit key
        
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70]),  # 192-bit key
        
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),  # 256-bit key

        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
               0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),  # 256-bit key
    ]

    ivs = [
        bytes([0x0a, 0x12, 0x3d, 0x56, 0x01, 0x11, 0xae, 0xff,
               0x0a, 0x12, 0x3d, 0x56, 0x01, 0x11, 0xae, 0xff]),
        bytes([0x0a, 0x12, 0x3d, 0x56, 0x01, 0x11, 0xae, 0xff,
               0x0a, 0x12, 0x3d, 0x56, 0x01, 0x11, 0xae, 0xff]),
        bytes([0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
               0x0a, 0x12, 0x3d, 0x56, 0x01, 0x11, 0xae, 0xff]),
        bytes([0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01,
               0x0a, 0x12, 0x3d, 0x56, 0x01, 0x11, 0xae, 0xff]),
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 
               0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70,])
    ]

    test_vectors = [
        bytes([0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
               0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74]),
        bytes([0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74]),
        bytes([
            0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
            0x68, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20, 
            0x66, 0x6F, 0x72, 0x20, 0x61, 0x6C, 0x6C, 0x00
        ]),
        bytes([
            0x4E, 0x6F, 0x77, 0x20, 0x69, 0x73, 0x20, 0x74,
            0x68, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x20, 
            0x66, 0x6F, 0x72, 0x20, 0x61, 0x6C, 0x6C, 0x00,
            0x66, 0x6F, 0x72, 0x20, 0x61
        ]),
        b'This is AES_CBC_256 test, with totally random key and totalyl random iv. oops that totally is spelt wrong lol... adding a couple more\
            letters to make this test longer',
    ]

    for iv, key, plaintext in zip(ivs, keys, test_vectors):
        cipher = AES.new(key, AES.MODE_CBC, iv)
        ciphertext = cipher.encrypt(pkcs7_padding(plaintext, AES.block_size))
        print("IV")
        print_hex_format(bytes_to_int(iv))
        print("key")
        print_hex_format(bytes_to_int(key))
        print("plaintext")
        print_hex_format(bytes_to_int(pkcs7_padding(plaintext, AES.block_size)))
        print("ciphertext")
        print_hex_format(bytes_to_int(ciphertext))
        print("================================================")
        
    return ' '


def aes_ctr():
    keys = [    
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),  # 128-bit key
        
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),  # 128-bit key
        
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70]),  # 192-bit key
        
        bytes([0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
               0x75, 0x28, 0x78, 0x39, 0x74, 0x93, 0xCB, 0x70,
               0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF]),  # 256-bit key
    ]

    nonces = [
        bytes([0x0a, 0x12, 0x3d, 0x56, 0x01, 0x11, 0xae, 0xff]),
        bytes([0x0a, 0x12, 0x3d, 0x56, 0x01, 0x11, 0xae, 0xff]),
        bytes([0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF]),
        bytes([0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01]),
    ]

    test_vectors = [
        b'Now is the time for all good men to come to the aid of their country.',
        b'The quick brown fox jumps over the lazy dog',
        b'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.',
        b'Sample message for encryption',
    ]
    for nonce, key, plaintext in zip(nonces, keys, test_vectors):
        # Combine the nonce and initial counter value into a single 128-bit block
        nonce_int = int.from_bytes(nonce, byteorder='big')
        # Assuming the counter starts at 1 and is 64 bits
        initial_counter_value = generate_random_numbers(64)
        iv = (nonce_int << 64) | initial_counter_value

        counter = Counter.new(128, initial_value=iv)
        cipher = AES.new(key, AES.MODE_CTR, counter=counter)
        ciphertext = cipher.encrypt(plaintext)
        print("Counter")
        print_hex_format(counter.get('initial_value'))
        print("Nonce")
        print_hex_format(bytes_to_int(nonce))
        print("Key")
        print_hex_format(bytes_to_int(key))
        print("Plaintext")
        print_hex_format(bytes_to_int(plaintext))
        print("Ciphertext")
        print_hex_format(bytes_to_int(ciphertext))
        print("================================================")
        # counter = Counter.new(128, initial_value=1)

    return ' '


def print_help_menu():
    print("""
Usage: script_name [BITS1] [BITS2] (BITS3) [OPERATION]

This script generates random test data for my crypto library.

============================================ bigint operations =================================================
Arguments:
    BITS1      Number of bits for the first random number                                        - num1
    BITS2      Number of bits for the second random number (optional for certain operations)     - num2
    BITS3      Number of bits for the third random number (required only for certain operations) - num3
    OPERATION  The operation to perform. 
            - add       Addition of two numbers                                                    -- num1 + num2
            - sub       Subtraction of two numbers                                                 -- num1 - num2
            - mul       Multiplication of two numbers                                              -- num1 * num2
            - dou       Doubling the first number                                                  -- num1 * 2
            - hal      Halving the first number                                                    -- num1 / 2
            - div       Integer division of two numbers, returns quotient and remainder            -- num1 // num2 , num1 % num2
            - mod       Modulus operation of two numbers                                           -- num1 % num2
            - or        Bitwise OR of two numbers                                                  -- num1 | num2
            - and       Bitwise AND of two numbers                                                 -- num1 & num2
            - xor       Bitwise XOR of two numbers                                                 -- num1 ^ num2
            - not       Bitwise NOT of the first number                                            -- ~num1
            - lshift    Left shift the first number by the number of bits specified in BITS2       -- num1 << num2
            - rshift    Right shift the first number by the number of bits specified in BITS2      -- num1 >> num2
            - mul_pow_2 Multiply the first number by 2 raised to BITS2                             -- num1 * (2^BITS2)
            - div_pow_2 Integer division of the first number by 2 raised to BITS2                  -- num1 / (2^BITE)
            - mod_pow_2 Modulus of the first number by 2 raised to BITS2                           -- num1 % (2^BITS2)
            - sqr       Square of the first number                                                 -- num1^2
            - expt_mod  Modular exponentiation of two numbers with modulus BITS3                   -- (num1 * num2) % num3
            - mod_inv   Modular multiplicative inverse                                             -- num1^-1 (mod num2)
    
    ============================================ hashing operations =================================================
    Arguments:
        BITS1      Number of bits for the first random string                                        - num1
        BITS2      Number of bits for the second random string (optional for certain operations)     - num2
        BITS3      Number of bits for the third random string (required only for certain operations) - num3  
        Operation  The hash algorithm  
            - md5       compute MD5 hash
    
    ============================================ encrypt operations =================================================
    Arguments:
            - des_ecb      Generate DES_ECB test vectors
            - des_cbc      Generate DES_CBC test vectors
            - 3des_ecb     Generate 3DES_ECB test vectors
            - 3des_cbc     Generate 3DES_CBC test vectors
            - aes_cbc      Generate AES_CBC test vectors for all three key lengths
            - aes_ctr      Generate AES_CTR test vectors for all three key lengths
          
""")


def perform_operation(num1, num2, num3, op):
    if op == "add":
        return num1 + num2
    elif op == "sub":
        return num1 - num2
    elif op == "mul":
        return num1 * num2
    elif op == "dou":
        return num1 * 2
    elif op == "hal":
        return num1 >> 1
    elif op == "div":
        if num2 == 0:
            return "ERORR, num2 = 0"
        else: 
            return (num1 // num2, num1 % num2)  # // => integer division, returns quotient and remainder
    elif op == "mod":
        if num2 == 0:
            return "ERORR, num2 = 0"
        else: 
            return num1 % num2
    elif op == "or":
        return num1 | num2
    elif op == "and":
        return num1 & num2
    elif op == "xor":
        return num1 ^ num2
    elif op == "not":
        return ~num1
    elif op == "lshift":
        return num1 << num2
    elif op == "rshift":
        return num1 >> num2
    elif op == "mul_pow_2":
        return num1 * (2 ** num2)
    elif op == "div_pow_2":
        return num1 // (2 ** num2)
    elif op == "mod_pow_2":
        return num1 % (2 ** num2)
    elif op == "sqr":
        return num1 * num1
    elif op == "expt_mod":
        return pow(num1, num2, num3)
    elif op == "mul_mod":
        return (num1 * num2) % num3
    elif op == "square_mod":
        return (num1 ** 2) % num2
    elif op == "gcd":
        return math.gcd(num1, num2)
    elif op == "lcm":
        return math.lcm(num1, num2)
    elif op == "mod_inv":
        return mod_inverse(num1, num2)
    elif op == "md5":
        byte_data = num1.to_bytes((num1.bit_length() + 7) // 8, byteorder='big') or b'\x00'
        return md5_hash(byte_data)
    elif op == "des" or op == "des_ecb":
        return des_ecb()
    elif op == "des_cbc":
        return des_cbc()
    elif op == "3des_ecb":
        return tdes_ecb()
    elif op == "3des_cbc":
        return tdes_cbc()
    elif op == "aes_cbc":
        return aes_cbc()
    elif op == "aes_ctr":
        return aes_ctr()

if __name__ == "__main__":
    bits1 = 0
    bits2 = 0
    bits3 = 0
    op = "add"
    random_number1 = 0
    random_number2 = 0
    random_number3 = 0

    if len(sys.argv) == 4:
        bits1 = int(sys.argv[1])
        bits2 = int(sys.argv[2])
        op = sys.argv[3].lower()
    elif len(sys.argv) == 3:
        bits1 = int(sys.argv[1])
        op = sys.argv[2].lower()
    elif len(sys.argv) == 2:
        op = sys.argv[1].lower()
    elif len(sys.argv) == 5:
        bits1 = int(sys.argv[1])
        bits2 = int(sys.argv[2])
        bits3 = int(sys.argv[3])
        op = sys.argv[4].lower()
    elif len(sys.argv) < 2 or sys.argv[1] in ('-h', '--help'):
        print_help_menu()
        sys.exit(0)
        
    random_number1 = generate_random_numbers(bits1)
    random_number2 = generate_random_numbers(bits2)
    random_number3 = generate_random_numbers(bits3)

    if op == "sub":
        while random_number1 < random_number2:
            random_number1 = generate_random_numbers(bits1)
            random_number2 = generate_random_numbers(bits2)
    
    if op == "lshift" or op == "rshift":
        random_number1 = generate_random_numbers(bits1)
        random_number2 = bits2
    
    if op == "mul_pow_2" or op == "div_pow_2" or op == "mod_pow_2":
        random_number1 = generate_random_numbers(bits1)
        random_number2 = bits2
    
    print("Random Number 1:")
    print_hex_format(random_number1)
    print("Random Number 2:")
    print_hex_format(random_number2)
    if bits3 != 0:
        print("Random Number 3:")
        print_hex_format(random_number3)

    result = perform_operation(random_number1, random_number2, random_number3, op)
    print("Result:")
    if isinstance(result, str):
        print(result)  # Error message
    else:
        if op != "div":
            print_hex_format(result)
        else:
            print("quotient:")
            print_hex_format(result[0])
            print("remainder:")
            print_hex_format(result[1])
    
    
