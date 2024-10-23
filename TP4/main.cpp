#include "rsa.hpp"
#include <iostream>


int main() {
    mpz_t p, q, n, e, d;
    mpz_inits(p, q, n, e, d, NULL);

    // Générer les clés RSA
    generate_rsa_keys(p, q, n, d, e);

    // Message à signer
    std::string message = "Hello, World!";

    // Signer le message normalement
    mpz_t signature1, signature2;
    mpz_inits(signature1, signature2, NULL);
    crt_sign(signature1, message, p, q, d, false);

    // Signer le message avec une faute injectée
    crt_sign(signature2, message, p, q, d, true);

    // Attaque de Bellcore pour retrouver p et q
    mpz_t p_found, q_found;
    mpz_inits(p_found, q_found, NULL);
    bellcore_attack(p_found, q_found, signature1, signature2, n);

    // Vérification
    if (mpz_cmp(p_found, p) == 0 && mpz_cmp(q_found, q) == 0) {
        std::cout << "L'attaque de Bellcore a réussi !" << std::endl;
    } else {
        std::cout << "L'attaque de Bellcore a échoué." << std::endl;
    }

    // Libérer la mémoire allouée
    mpz_clears(p, q, n, e, d, signature1, signature2, p_found, q_found, NULL);

    return 0;
}


