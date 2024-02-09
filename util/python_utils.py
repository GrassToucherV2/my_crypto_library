import secrets

def print_hex_format(number, line_length=8):
    # Convert the number to a hexadecimal string, removing the '0x' prefix and padding with zeros to ensure byte alignment
    hex_str = format(number, 'x').zfill((number.bit_length() + 7) // 8 * 2)  # Ensure byte alignment
    # Insert spaces and format as bytes
    formatted_str = ', '.join('0x' + hex_str[i:i+2].upper() for i in range(0, len(hex_str), 2))
    # Print with line breaks after every 'line_length' bytes
    for i in range(0, len(formatted_str), line_length * 6):  # 6 accounts for the length of '0xXX, '
        print(formatted_str[i:i + line_length * 6])
    print("\n")


def generate_random_numbers(bits):
    # Generate a random number with the given number of bits
    random_number1 = secrets.randbits(bits)
    random_number2 = secrets.randbits(bits)
    return random_number1, random_number2

# Example usage
bits = 128  # Change this to generate numbers of different sizes
random_number1, random_number2 = generate_random_numbers(bits)
res = random_number1 + random_number2

print_hex_format(random_number1)
print_hex_format(random_number2)
print_hex_format(res)