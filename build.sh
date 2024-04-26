#!/bin/bash

# List of C source files
FILES=(
    
    "test/test_util.c"       
    "util/tools.c"          
    "main.c"  
    "lib/crypto_api.c"              
    "lib/bigint.c"    
    "lib/md5.c"        
    "lib/sha1.c"        
    "lib/sha256.c"        
    "lib/sha512.c"     
    "lib/chacha20.c"   
    "lib/poly1305.c"  
    "lib/chacha20_poly1305.c" 
    "lib/des.c"
    "lib/aes.c"
    "test/bigint_test.c"  
    "test/hash_test.c"                     
    "test/cipher_test.c"                         
)

# Compiler options
CC=gcc
CFLAGS="-Wall -Wextra"
RUSTC=rustc
RUSTFLAGS="--crate-type=bin"

# Function to compile C library
compile_c_library() {
    echo "Compiling C library..."
    # Filter out the files that should be part of the library
    LIB_FILES=()
    for file in "${FILES[@]}"; do
        if [[ "$file" == lib/*.c ]]; then
            LIB_FILES+=("$file")
        fi
    done
    $CC $CFLAGS -shared -fPIC "${LIB_FILES[@]}" -o libcrypto_api.so
    if [ $? -eq 0 ]; then
        echo "C library compiled successfully"
    else
        echo "Error compiling C library"
        exit 1
    fi
}

# Function to compile Rust code
compile_rust() {
    echo "Compiling Rust $1..."
    $RUSTC $RUSTFLAGS -L. -lcrypto_api util/$1.rs -o $1
    if [ $? -eq 0 ]; then
        echo "Rust $1 compiled successfully"
    else
        echo "Error compiling Rust $1"
        exit 1
    fi
}

# I am beginning to forget the options...
function print_help() {
    echo "Usage: $0 [OPTION]..."
    echo "Options:"
    echo "  -h, --help          Display this help and exit."
    echo "  -v, --val           Add debugging information with -g flag for use with valgrind."
    echo "  -c, --clear         Clean up all compiled binaries."
    echo "  -d, --disassemble   Compile and disassemble the binary, saving the output to disassembly_with_source."
    echo "  -g, --gprof         Compile with profiling information for use with gprof."
    echo "  -b, --benchmark     Compile and link benchmark components, creating a benchmark binary."
    echo "  -w, --webserver     Compile webserver and client written in rust with crypto library"
    echo ""
    exit 0
}

# --val and --disassemble argument add -g flag when compiling, which lets valgrind to pinpoint the line number 
# of the problematic code, and objdump to print the source code corresponding to the asm block
for arg in "$@"
do  

    if [[ "$arg" == "--help" || "$arg" == "-h" ]]; then
        print_help
    fi

    # adding -g flag to compile to facilitate debugging with valgrind
    if [[ "$arg" == "--val" || "$arg" == "-v" ]]; then
        CFLAGS="$CFLAGS -g"
    fi

    # deletes all the built binaries 
    if [[ "$arg" == "--clear" || "$arg" == "-c" ]]; then
        echo "Cleaning up..."
        find . -type f \( -name "*.o" -o -name "*.out" \) -exec rm {} +
        echo "Cleanup complete."
        exit 0
    fi

    # objdump the binary 
    if [[ "$arg" == "--disassemble" || "$arg" == "-d" ]]; then
        CFLAGS="$CFLAGS -g"
        if $CC $CFLAGS "${FILES[@]}"; then
            echo "All files successfully compiled"
        else
            echo "Error compiling files"
            exit 1 
        fi
        if objdump -d -l -S a.out > disassembly_with_source; then
            echo "objdump succeeded in disassembly_with_source"
        else
            echo "objdump failed"
            exit 2 
        fi
        exit 0

    fi

    # build project for gprof 
    if [[ "$arg" == "--gprof" || "$arg" == "-g" ]]; then
        CFLAGS="$CFLAGS -pg"
    fi

    # build the project for benchmarking 
    if [[ "$arg" == "--benchmark" || "$arg" == "-b" ]]; then
        echo "Compiling and linking for benchmark..."

        # Compile the C library
        gcc -c util/bigint.c -o bigint.o 

        # Compile the C++ benchmark file
        g++ -c util/benchmark.cpp -o benchmark.o 

        # Compile the timer file 
        g++ -c util/benchmark_timer.cpp -o timer.o

        # Link them together
        g++ bigint.o benchmark.o timer.o -o benchmark_bigint.o 

        # Check if benchmark compilation and linking succeeded
        if [ $? -eq 0 ]; then
            echo "Benchmark binary 'test_bigint' successfully created."
        else
            echo "Error compiling or linking benchmark components."
            exit 1
        fi

        rm bigint.o benchmark.o timer.o
        echo "Cleaned up intermediate object files."

        exit 0
    fi

    if [[ "$arg" == "--webserver" || "$arg" == "-w" ]]; then
        echo "compileing for rust webserver and crypto library"
        compile_c_library
        compile_rust "server"
        compile_rust "client"
        # had to copy libcrypto_api.so to the following location for it to work
        cp libcrypto_api.so /usr/local/lib/
        ldconfig
        exit 0
    fi
done

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
