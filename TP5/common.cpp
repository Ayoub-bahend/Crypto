#include "common.hpp"

// Addition in F216
uint16_t addF216(uint16_t a, uint16_t b) {
    return a ^ b;
}

// Multiplication by X in F216
uint16_t multiplyByX(uint16_t a) {
    uint16_t result = a << 1;
    if (a & 0x8000) { // If the highest bit is set
        result ^= 0x002B; // Reduce by P16(X) = X16 + X5 + X3 + X2 + 1
    }
    return result;
}

// Multiplication in F216
uint16_t multiplyF216(uint16_t a, uint16_t b) {
    uint16_t result = 0;
    for (int i = 0; i < 16; ++i) {
        if (b & 1) {
            result ^= a;
        }
        a = multiplyByX(a);
        b >>= 1;
    }
    return result;
}