import secrets
import sys

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

def perform_operation(num1, num2, op):
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
            return num1 // num2  # // => integer division 
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

if __name__ == "__main__":
    bits1 = 0
    bits2 = 0
    op = "add"
    random_number1 = 0
    random_number2 = 0

    if len(sys.argv) == 4:
        bits1 = int(sys.argv[1])
        bits2 = int(sys.argv[2])
        op = sys.argv[3].lower()
    elif len(sys.argv) == 3:
        bits1 = int(sys.argv[1])
        op = sys.argv[2].lower()
        
    random_number1 = generate_random_numbers(bits1)
    random_number2 = generate_random_numbers(bits2)

    if op == "sub":
        while random_number1 < random_number2:
            random_number1 = generate_random_numbers(bits1)
            random_number2 = generate_random_numbers(bits2)

    print("Random Number 1:")
    print_hex_format(random_number1)
    print("Random Number 2:")
    print_hex_format(random_number2)

    result = perform_operation(random_number1, random_number2, op)
    print("Result:")
    if isinstance(result, str):
        print(result)  # Error message
    else:
        print_hex_format(result)

