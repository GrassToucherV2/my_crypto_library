import secrets
import sys
import math

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

def print_help_menu():
    print("""
Usage: script_name [BITS1] [BITS2] (BITS3) [OPERATION]

This script performs various arithmetic operations on large, randomly generated numbers.

Arguments:
  BITS1      Number of bits for the first random number - num1
  BITS2      Number of bits for the second random number (optional for certain operations) - num2
  BITS3      Number of bits for the third random number (required only for certain operations) - num3
  OPERATION  The arithmetic operation to perform. Supported operations include:
             - add      Addition of two numbers -- num1 + num2
             - sub      Subtraction of two numbers -- num1 - num2
             - mul      Multiplication of two numbers -- num1 * num2
             - dou      Doubling the first number -- num1 * 2
             - half     Halving the first number -- num1 / 2
             - div      Integer division of two numbers, returns quotient and remainder -- num1 // num2 , num1 % num2
             - mod      Modulus operation of two numbers -- num1 % num2
             - or       Bitwise OR of two numbers -- num1 | num2
             - and      Bitwise AND of two numbers -- num1 & num2
             - xor      Bitwise XOR of two numbers -- num1 ^ num2
             - not      Bitwise NOT of the first number -- ~num1
             - lshift   Left shift the first number by the number of bits specified in BITS2 -- num1 << num2
             - rshift   Right shift the first number by the number of bits specified in BITS2 -- num1 >> num2
             - mul_pow_2 Multiply the first number by 2 raised to BITS2 -- num1 * (2^BITS2)
             - div_pow_2 Integer division of the first number by 2 raised to BITS2 -- num1 / (2^BITE)
             - mod_pow_2 Modulus of the first number by 2 raised to BITS2 -- num1 % (2^BITS2)
             - sqr      Square of the first number -- num1^2
             - expt_mod Modular exponentiation of two numbers with modulus BITS3 -- (num1 * num2) % num3
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
    elif op == "half":
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
    elif len(sys.argv) == 5:
        bits1 = int(sys.argv[1])
        bits2 = int(sys.argv[2])
        bits3 = int(sys.argv[3])
        op = sys.argv[4].lower()
    elif len(sys.argv) < 3 or sys.argv[1] in ('-h', '--help'):
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
    
    
