#!/bin/bash

# compile the suite of test functions
gcc -c fsuite.c

# create a shared library
ld -o fsuite.so -shared fsuite.o

# compile the evaluator (will use the shared library)
g++ -Wall -pedantic evaluator.cc -ldl -o evaluator
