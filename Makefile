# hard-coded file names but it is OK

.PHONY: all clean test

all: libignum.so

clean:
	rm -f libignum.so

test:
	python3 test.py

libignum.so: bignum.c bignum.h
	gcc -std=gnu2x -Ofast -march=native -funroll-loops -funsafe-math-optimizations -fpic -shared bignum.c -o libignum.so