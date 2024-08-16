#pragma once

#include <stdint.h>

struct bignum {
    uint32_t size, capacity;
    bool negative;
    uint64_t* data;
};
typedef struct bignum* bignum_t;

// create bignum from decimal string representation of number
bignum_t bn_create(const char* n);

// copy b to a
void bn_copyto(bignum_t a, bignum_t b);

// deallocate things
void bn_delete(void* p);

// in-place arithmetic operations
void bn_add(bignum_t a, bignum_t b);
void bn_subtract(bignum_t a, bignum_t b);

/*
void bn_multiply(bignum_t* a, bignum_t b);
void bn_divide(bignum_t* a, bignum_t b);
*/

void bn_increment(bignum_t a);
void bn_decrement(bignum_t a);

// comparison: returns 1 if a > b, -1 if a < b, and 0 otherwise
int bn_compare(bignum_t a, bignum_t b);

// get hex string from number. your job to free it afterwards
char* bn_tohexstr(bignum_t n);
