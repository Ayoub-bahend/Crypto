#pragma once

#include <gmp.h>

void generatePrime(mpz_t &p);
void generateRandom(mpz_t &s, mpz_t p);
void generateCoefs(mpz_t *tab, mpz_t p, int k);
void init_tab_mpz(mpz_t * tab, int t);
void computeShares(mpz_t *x, mpz_t *y, mpz_t *a, mpz_t s, int k, int n);
void compute_image(mpz_t &image, mpz_t *a, mpz_t x, int k, mpz_t s);
void computeLagrange(mpz_t *alpha, mpz_t * x, int k, int n, mpz_t p);
void clear_tab_mpz(mpz_t * tab, int t) ;
void reconstructSecret(mpz_t * alpha, mpz_t *y, mpz_t Sr, mpz_t p, int k);
bool check_coefs_different(mpz_t *tab, int taille, mpz_t e);
void copyTable(mpz_t * a, mpz_t * b, int n);