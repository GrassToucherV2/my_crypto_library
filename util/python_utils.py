import secrets

def generate_random_numbers(bits):
    # Generate a random number with the given number of bits
    random_number1 = secrets.randbits(bits)
    random_number2 = secrets.randbits(bits)
    return random_number1, random_number2

# Example usage
bits = 128  # Change this to generate numbers of different sizes
random_number1, random_number2 = generate_random_numbers(bits)

print("Random Number 1:", random_number1)
print("Random Number 2:", random_number2)