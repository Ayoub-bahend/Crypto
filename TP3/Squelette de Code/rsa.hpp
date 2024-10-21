// rsa.h
#ifndef RSA_H
#define RSA_H

#include <gmp.h>

#define BITSTRENGTH 14              // Taille du module n en bits
#define PRIMESIZE (BITSTRENGTH / 2)  // Taille des nombres premiers p et q

// Déclaration des variables globales
extern mpz_t d, e, n, M, c;


void generate_rsa_keys();
void encrypt(const char* plaintext, char* ciphertext);
void decrypt(const char* ciphertext);
void rsa_encrypt(mpz_t ciphertext, const mpz_t message, const mpz_t e, const mpz_t n);
void rsa_decrypt(mpz_t decrypted_message, const mpz_t ciphertext, const mpz_t d, const mpz_t n);

void customized_generate_rsa_keys();
void nextprime(mpz_t &p, mpz_t s);
bool PrimalityTest(int accuracy, mpz_t & odd_integer);
void powm(mpz_t &g, mpz_t &k, mpz_t &p, mpz_t &m);
void extended_GCD(const mpz_t a, const mpz_t b, mpz_t & k1, mpz_t & k2);
void invert(mpz_t & d, const mpz_t e, const mpz_t x);
void customized_encrypt(char * message, char * chiffr_str);
void customized_decrypt(char * chiffr_str);

void clear_gmp_integers();

#endif
