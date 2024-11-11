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

// Function to generate the logarithm table for a given generator
std::vector<uint16_t> generateLogTable(uint16_t generator, uint16_t polynomial) {
    std::vector<uint16_t> logTable(65536, 0);
    uint16_t value = 1;
    for (uint16_t i = 0; i < 65535; ++i) {
        logTable[value] = i;
        value = multiplyByX(value);
    }
    logTable[0] = 65535; // Convention for log(0)
    return logTable;
}

// Function to generate the inverse logarithm table
std::vector<uint16_t> generateInverseLogTable(const std::vector<uint16_t>& logTable) {
    std::vector<uint16_t> inverseLogTable(65536, 0);
    for (uint16_t i = 0; i < 65535; ++i) {
        inverseLogTable[logTable[i]] = i;
    }
    inverseLogTable[65535] = 0; // Convention for inverse log(0)
    return inverseLogTable;
}

// Function to generate the Zech's logarithm table
std::vector<uint16_t> generateZechLogTable(const std::vector<uint16_t>& logTable, const std::vector<uint16_t>& inverseLogTable) {
    std::vector<uint16_t> zechLogTable(65536, 0);
    for (uint16_t i = 0; i < 65535; ++i) {
        uint16_t gi = inverseLogTable[i];
        uint16_t onePlusGi = gi ^ 1;
        zechLogTable[i] = logTable[onePlusGi];
    }
    return zechLogTable;
}