
# Summary and Disclaimer

This library is developed for **educational purposes** and serves as a "cryptography playground."

**Disclaimer:** This code is strictly for learning and experimentation. It **must not be used in any real-world software projects**, as custom-implemented cryptographic libraries are highly prone to vulnerabilities. However, it does make the library an excellent target for cryptanalysis exercises :)

I welcome and appreciate anyone who tests the code and reports any issues.

---

# Build the Project

A simple `build.sh` script is located at the root of the repository and provides the following options:

```
./build.sh --help              # Display help information
./build.sh --val               # Compile with debug symbols for Valgrind (for memory/leak checking)
./build.sh --clear             # Clean up all compiled binaries and build artifacts
./build.sh --disassemble       # Disassemble the compiled binary (for low-level code review)
./build.sh --gprof             # Compile with profiling information for gprof (for performance analysis)
./build.sh --benchmark         # Compile and link the benchmarking components
./build.sh --webserver         # Compile the webserver and client using the crypto library (Note: server feature is under development)

```

# TESTS
`main.c`, located at the root of this repo, is the driver program for the tests. To run the tests:

```
./a.out                         # Runs all tests
./a.out u                       # Runs all utility tests
./a.out b                       # Runs all bigint tests
./a.out h                       # Runs all hash tests
./a.out c                       # Runs all cipher tests
./a.out <category> <test_name>  # Runs a specific test (e.g., ./a.out b bigint_add_test)

```
