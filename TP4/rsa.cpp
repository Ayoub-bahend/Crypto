#include <openssl/sha.h>
#include "rsa.hpp"
#include <gmp.h>
#include <iostream>
#include <ctime>

// Fonction pour afficher un mpz_t
void print_mpz(const mpz_t num) {
    char *str = mpz_get_str(NULL, 10, num); 
    std::cout << str;                       
    std::cout << std::endl;              
    free(str);                             
}

// Fonction pour générer une clé RSA (p, q, n, e, d)
void generate_rsa_keys(mpz_t &p_out, mpz_t &q_out, mpz_t &n_out, mpz_t &d_out, mpz_t &e_out) {
    mpz_t p, q, phi_n, gcd;
    gmp_randstate_t rand_state;

    // Initialize local variables
    mpz_inits(p, q, phi_n, gcd, NULL);
    gmp_randinit_default(rand_state);
    gmp_randseed_ui(rand_state, time(NULL));

    // Generate prime numbers p and q
    std::cout << "Génération des nombres premiers p et q..." << std::endl;
    mpz_urandomb(p, rand_state, PRIMESIZE);
    mpz_nextprime(p, p);
    std::cout << "Premier p généré : ";
    print_mpz(p); // Utiliser print_mpz au lieu de std::cout

    mpz_urandomb(q, rand_state, PRIMESIZE);
    mpz_nextprime(q, q);
    std::cout << "Premier q généré : ";
    print_mpz(q); // Utiliser print_mpz au lieu de std::cout

    // Calculate n = p * q
    mpz_mul(n_out, p, q);
    std::cout << "Module n = p * q : ";
    print_mpz(n_out); // Utiliser print_mpz au lieu de std::cout

    // Calculate phi(n) = (p - 1) * (q - 1)
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi_n, p, q);

    // Choose e such that 1 < e < phi(n) and gcd(e, phi_n) = 1
    mpz_set_ui(e_out, 65537);  // Utilisation de 65537 comme valeur standard de e
    mpz_gcd(gcd, e_out, phi_n);
    while (mpz_cmp_ui(gcd, 1) != 0) {
        mpz_add_ui(e_out, e_out, 2);
        mpz_gcd(gcd, e_out, phi_n);
    }
    std::cout << "Valeur de e choisie : ";
    print_mpz(e_out); // Utiliser print_mpz au lieu de std::cout

    // Calculate d, the modular inverse of e mod phi_n
    if (mpz_invert(d_out, e_out, phi_n) == 0) {
        std::cerr << "Erreur : impossible de trouver l'inverse modulaire." << std::endl;
    }
    std::cout << "Clé privée d calculée : ";
    print_mpz(d_out); // Utiliser print_mpz au lieu de std::cout

    // Pass p and q back to main
    mpz_add_ui(p, p, 1); // Restore p and q
    mpz_add_ui(q, q, 1);
    mpz_set(p_out, p);
    mpz_set(q_out, q);

    // Clear local variables
    mpz_clears(p, q, phi_n, gcd, NULL);
    gmp_randclear(rand_state);
}

// Exponentiation modulaire avec GMP
void mod_exp(mpz_t result, const mpz_t base, const mpz_t exp, const mpz_t mod) {
    mpz_powm(result, base, exp, mod);  // GMP function for modular exponentiation
}

// Fonction de calcul du PGCD avec GMP
void gcd(mpz_t result, const mpz_t a, const mpz_t b) {
    mpz_gcd(result, a, b);  // GMP function for GCD
}

// Fonction pour calculer l'inverse modulaire avec GMP
void mod_inverse(mpz_t result, const mpz_t a, const mpz_t mod) {
    if (mpz_invert(result, a, mod) == 0) {
        std::cerr << "Erreur : impossible de calculer l'inverse modulaire." << std::endl;
    }
}

// Fonction de signature avec CRT et possibilité d'injecter une faute
void crt_sign(mpz_t signature, const std::string& message, const mpz_t p, const mpz_t q, const mpz_t d, bool inject_fault) {
    mpz_t n, A, B, digest, Mp, Mq, σ1, σ2, temp;
    mpz_inits(n, A, B, digest, Mp, Mq, σ1, σ2, temp, NULL);
    
    // Calcul de n = p * q
    mpz_mul(n, p, q);

    // Calcul de A et B pour le CRT
    mod_inverse(A, q, p);
    mod_inverse(B, p, q);
    mpz_mul(A, A, q);  // A = q * (q^-1 mod p)
    mpz_mul(B, B, p);  // B = p * (p^-1 mod q)

    // Utilisation de SHA-256 pour hacher le message
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(message.c_str()), message.size(), hash);

    // Conversion du hash SHA-256 en un entier GMP
    mpz_import(digest, SHA256_DIGEST_LENGTH, 1, sizeof(hash[0]), 0, 0, hash);

    // Calcul de Mp = digest mod p et Mq = digest mod q
    mpz_mod(Mp, digest, p);
    mpz_mod(Mq, digest, q);

    // Simulation d'une faute dans σ1 si inject_fault est vrai
    mod_exp(σ1, Mp, d, p);
    if (inject_fault) {
        std::cout << "Injection de faute dans σ1..." << std::endl;
        mpz_add_ui(σ1, σ1, 1);  // Injecter une petite erreur dans σ1
    }

    mod_exp(σ2, Mq, d, q);

    // Combinaison des résultats CRT : signature = (A * σ1 + B * σ2) % n
    mpz_mul(temp, A, σ1);
    mpz_addmul(temp, B, σ2);
    mpz_mod(signature, temp, n);

    std::cout << "Signature générée : ";
    print_mpz(signature); // Utiliser print_mpz au lieu de std::cout

    // Libérer la mémoire allouée
    mpz_clears(n, A, B, digest, Mp, Mq, σ1, σ2, temp, NULL);
}

// Attaque de Bellcore pour retrouver p et q
void bellcore_attack(mpz_t p_out, mpz_t q_out, const mpz_t signature1, const mpz_t signature2, const mpz_t n) {
    mpz_t delta, gcd_res;
    mpz_inits(delta, gcd_res, NULL);

    // Calcul de delta = abs(signature1 - signature2)
    mpz_sub(delta, signature1, signature2);
    mpz_abs(delta, delta);

    // Calcul du pgcd de delta et n pour retrouver q
    gcd(gcd_res, delta, n);

    // Calcul de p = n / q
    mpz_div(p_out, n, gcd_res);
    mpz_set(q_out, gcd_res);

    std::cout << "Valeur retrouvée de p : ";
    print_mpz(p_out); // Utiliser print_mpz au lieu de std::cout
    std::cout << "Valeur retrouvée de q : ";
    print_mpz(q_out); // Utiliser print_mpz au lieu de std::cout

    // Libérer la mémoire allouée
    mpz_clears(delta, gcd_res, NULL);
}

