import sys

def extract(filename):
    with open(filename, "r") as file:
        suppression_lines = []
        in_suppression = False

        for line in file:
            line = line.strip()
            if line.startswith("{"):
                in_suppression = True
                suppression_lines = [line]
            elif line.startswith("}") and in_suppression:
                suppression_lines.append(line)
                suppression_text = '\n'.join(suppression_lines)
                print(suppression_text)
                in_suppression = False
            elif in_suppression:
                suppression_lines.append("    " + line)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Enter input file name after python extract_supp.py")
    else:
        extract(sys.argv[1])