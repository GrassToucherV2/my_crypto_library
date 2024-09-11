
# SUMMARY

This library is developed for fun and educational purposes. I am hoping to eventually learn more about cryptanalysis and implement attacks to exploit the cryptographic algorithms implemented here.

Note that this code should not be used in any real software projects due to potential security vulnerabilities in the code, and thorough testing is required to ensure security.

I welcome and appreciate anyone who tests and points out any problem in my code :)

# BUILD THE PROJECT

There is a simple build.sh script located at the root of this repo. It has the following options:

```
./build.sh --help              # Display help information
./build.sh --val               # Compile with debug symbols for Valgrind
./build.sh --clear             # Clean up all compiled binaries
./build.sh --disassemble       # Disassemble the compiled binary
./build.sh --gprof             # Compile with profiling information for gprof
./build.sh --benchmark         # Compile and link benchmark components
./build.sh --webserver         # Compile webserver and client with crypto library --- server not working yet

```

# TESTS
main.c, located at the root of this repo, is the driver program for the tests. To run the tests:

```
./a.out                         # Runs all tests
./a.out u                       # Runs all utility tests
./a.out b                       # Runs all bigint tests
./a.out h                       # Runs all hash tests
./a.out c                       # Runs all cipher tests
./a.out <category> <test_name>  # Runs a specific test (e.g., ./a.out b bigint_add_test)

```
