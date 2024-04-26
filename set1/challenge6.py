def compute_hamming_distance(str1, str2):
    hamming_distance = 0

    # assuming str1 and str2 have equal length
    for i in range(len(str1)):
        char1 = ord(str1[i])
        char2 = ord(str2[i])

        xor_res = char1 ^ char2

        while xor_res:
            hamming_distance += xor_res & 1
            xor_res >>= 1
    
    return hamming_distance

string1 = "this is a test"
string2 = "wokka wokka!!!"
distance = compute_hamming_distance(string1, string2)
print("Hamming distance:", distance)  # Output: 37

