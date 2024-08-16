#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "bignum.h"

#include <string.h>

static uint64_t one_data = { 1 };

static struct bignum ONE = {
    .capacity = 1, .size = 1,
    .negative = false,
    .data = &one_data
};

static struct bignum MINUSONE = {
    .capacity = 1, .size = 1,
    .negative = true,
    .data = &one_data
};

static void resize(bignum_t n) {
    if (n->size > n->capacity) {
        n->capacity *= 2;
        int newsize = n->capacity*sizeof(uint64_t);
        n->data = realloc(n->data, newsize);
    }
}

static void extend(bignum_t n, uint64_t v) {
    n->size++;
    resize(n);
    n->data[n->size - 1] = v;
}

static void mul_int(bignum_t a, uint64_t b) {
    // do the multiplication
    uint64_t ci = 0;
    for (int i = 0; i < a->size; i++) {
        uint64_t ai = a->data[i], ri = ai*b;
        a->data[i] = ci + ri;
        ci = (ai * (unsigned __int128)b) >> 64;
    }

    // add another chunk if needed
    if (ci != 0)
        extend(a, ci);
}

static void add_opp_smal(bignum_t a, bignum_t b) {
    int hnz = 0, carry = 0;
    for (int i = 0; i < b->size; i++) {
        if (i >= a->size)
            extend(a, 0);

        uint64_t ai = a->data[i], bi = b->data[i], ca = carry + ai;
        carry = bi < ai;
        a->data[i] = bi - ca;

        if (a->data[i] != 0)
            hnz = i;
    }
    a->negative = !a->negative;

    // remove leading zeroes
    if (a->data[a->size - 1] == 0)
        a->size = hnz+1;
}

static void add_opp(bignum_t a, bignum_t b) {
    int hnz = 0, carry = 0;
    for (int i = 0; i < b->size; i++) {
        uint64_t ai = a->data[i], bi = b->data[i], cb = carry + bi;
        carry = ai < bi;
        a->data[i] -= cb;

        if (a->data[i] != 0)
            hnz = i;
    }

    // remove leading zeroes
    if (a->data[a->size - 1] == 0)
        a->size = hnz+1;
}

static int compare_abs(bignum_t a, bignum_t b) {
    if (a->size > b->size) return 1;
    if (a->size < b->size) return -1;

    for (int i = a->size - 1; i >= 0; i--) {
        if (a->data[i] > b->data[i]) return 1;
        if (a->data[i] < b->data[i]) return -1;
    }
    return 0;
}

// create bignum from decimal string representation of number
bignum_t bn_create(const char* n) {
    // allocate memory for the number
    bignum_t b = malloc(sizeof(struct bignum));
    b->capacity = 4;
    b->size = 1;
    b->data = malloc(4*sizeof(uint64_t));
    b->data[0] = 0;

    // check if the number is negative
    b->negative = n[0] == '-';
    n += b->negative;

    // convert string to bignum
    for (int i = 0; n[i] != '\0'; i++) {
        mul_int(b, 10);
        b->data[0] += n[i] - '0';
    }

    //bn_display(b);
    return b;
}

void bn_copyto(bignum_t a, bignum_t b) {
    a->size = b->size;
    resize(a);
    a->negative = b->negative;
    memcpy(a->data, b->data, b->size*sizeof(uint64_t));
}

// destroy (deallocate) stuff
void bn_delete(void* p) {
    free(p);
}

// add two big numbers
inline void bn_add(bignum_t a, bignum_t b) {
    bool opp = (!a->negative && b->negative) ||
               (a->negative && !b->negative),
         smal = compare_abs(a, b) == -1;

    if (opp && smal) {
        add_opp_smal(a, b);
        return;
    }
    if (opp) {
        add_opp(a, b);
        return;
    }

    int carry = 0;
    for (int i = 0; i < b->size; i++) {
        if (i >= a->size)
            extend(a, 0);

        uint64_t ai = a->data[i], bi = b->data[i], cb = carry + bi;
        carry = (UINT64_MAX - ai) < bi;
        a->data[i] += cb;
    }
}

void bn_noop(int a, int b) {
    return;
}

// subtract two big numbers
void bn_subtract(bignum_t a, bignum_t b) {
    a->negative = !a->negative;
    bn_add(a, b);
    a->negative = !a->negative;
}

/*
    TODO: implement these

void bn_multiply(bignum_t* a, bignum_t b) {
    printf("stub: %s\n", __FUNCTION__);
}
void bn_divide(bignum_t* a, bignum_t b) {
    printf("stub: %s\n", __FUNCTION__);
}

*/

void bn_increment(bignum_t a) {
    bn_add(a, &ONE);
}

void bn_decrement(bignum_t a) {
    bn_add(a, &MINUSONE);
}

// comparison: returns 1 if a > b, -1 if a < b, and 0 otherwise
int bn_compare(bignum_t a, bignum_t b) {
    if (a->negative && !b->negative)
        return -1;
    if (!a->negative && b->negative)
        return 1;
    if (a->negative)
        return -compare_abs(a, b);
    return compare_abs(a, b);
}

// there's probably a much better way of doing this
char* bn_tohexstr(bignum_t n) {
    size_t s0 = 3;
    if (n->negative) s0++;

    size_t s1 = snprintf(NULL, 0, "%lx", n->data[n->size - 1]);
    size_t s2 = 16*(n->size - 1);

    char* r = malloc(s0 + s1 + s2);
    char* ret = r;
    if (n->negative) {
        *ret = '-';
        ret++;
    }
    ret[0] = '0';
    ret[1] = 'x';
    ret += 2;

    sprintf(ret, "%lx", n->data[n->size - 1]);
    ret += s1;

    for (int i = n->size - 2; i >= 0; i--) {
        sprintf(ret, "%016lx", n->data[i]);
        ret += 16;
    }

    return r;
}