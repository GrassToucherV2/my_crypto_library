#!/bin/bash

# List of C source files
FILES=("test/test_util.c" "util/tools.c" \
         "main.c" "util/bigint.c"   "test/bigint_test.c"                    \
         "set1/challenge1.c" "set1/challenge2.c" "set1/challenge3.c" "set1/challenge4.c"
         "set1/challenge5.c")

# Compiler options
CC=gcc
CFLAGS="-Wall -Wextra"

# Compile all files together
echo "Building all files..."
$CC $CFLAGS "${FILES[@]}"

# Check if compilation succeeded
if [ $? -eq 0 ]; then
    echo "All files successfully compiled"
else
    echo "Error compiling files"
    exit 1
fi 
