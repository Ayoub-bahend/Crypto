// shor_utils.hpp
#ifndef SHOR_UTILS_HPP
#define SHOR_UTILS_HPP

#include <cmath>

// Fonction pour calculer le PGCD
inline int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Fonction pour calculer a^b % n
inline int mod_exp(int a, int b, int n) {
    int result = 1;
    a = a % n;
    while (b > 0) {
        if (b % 2 == 1) {
            result = (result * a) % n;
        }
        b = b >> 1;
        a = (a * a) % n;
    }
    return result;
}

#endif // SHOR_UTILS_HPP