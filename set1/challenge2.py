from binascii import unhexlify, hexlify
from base64 import b64encode

def bxor(a, b):
    "bitwise XOR of bytestrings"
    xor_res = b""
    for a_i, b_i in zip(a, b):
        xor_res += bytes([a_i ^ b_i])
    
    return bytes(xor_res)

str1 = "1c0111001f010100061a024b53535009181c"
str2 = "686974207468652062756c6c277320657965"

hex_str1 = unhexlify(str1)
hex_str2 = unhexlify(str2)

res = bxor(hex_str1, hex_str2)

print(res)
