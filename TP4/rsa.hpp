#ifndef RSA_HPP
#define RSA_HPP

#include <iostream>
#include <string>
#include <utility>
#include <gmp.h>

#define BITSTRENGTH 128           // Taille du module n en bits
#define PRIMESIZE (BITSTRENGTH / 2)  // Taille des nombres premiers p et q

// Fonction pour générer une clé RSA (p, q, n, e, d)
void generate_rsa_keys(mpz_t &p_out, mpz_t &q_out, mpz_t &n_out, mpz_t &d_out, mpz_t &e_out);

// Exponentiation modulaire avec GMP
void mod_exp(mpz_t result, const mpz_t base, const mpz_t exp, const mpz_t mod);

// Fonction pour calculer l'inverse modulaire avec GMP
void mod_inverse(mpz_t result, const mpz_t a, const mpz_t mod);

// Fonction de calcul du PGCD avec GMP
void gcd(mpz_t result, const mpz_t a, const mpz_t b);

// Fonction de signature avec CRT et possibilité d'injecter une faute
void crt_sign(mpz_t signature, const std::string& message, const mpz_t p, const mpz_t q, const mpz_t d, bool inject_fault = false);

// Attaque de Bellcore pour retrouver p et q
void bellcore_attack(mpz_t p_out, mpz_t q_out, const mpz_t signature1, const mpz_t signature2, const mpz_t n);

#endif // RSA_HPP