#include <iostream>
#include <openssl/dsa.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/rand.h>

using namespace std;

// Fonction utilitaire pour afficher une erreur OpenSSL
void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

// Fonction de hachage SHA-256 du message
BIGNUM* hashMessage(const std::string& message) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)message.c_str(), message.size(), hash);
    BIGNUM* bnHash = BN_new();
    BN_bin2bn(hash, SHA256_DIGEST_LENGTH, bnHash);
    return bnHash;
}

// Fonction principale de génération de clés, signature et vérification
int main() {
    // Initialisation du contexte d'erreurs OpenSSL
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    // Création de la structure DSA et génération des paramètres
    DSA* dsa = DSA_new();
    if (DSA_generate_parameters_ex(dsa, 1024, NULL, 0, NULL, NULL, NULL) != 1) {
        handleErrors();
    }

    // Génération de la paire de clés
    if (DSA_generate_key(dsa) != 1) {
        handleErrors();
    }

    // Message à signer
    std::string message = "Hello, DSA!";
    BIGNUM* hash = hashMessage(message);

    // Signature
    unsigned char* sig = new unsigned char[DSA_size(dsa)];
    unsigned int sig_len;

    if (DSA_sign(0, (const unsigned char*)BN_bn2hex(hash), BN_num_bytes(hash), sig, &sig_len, dsa) != 1) {
        handleErrors();
    }
    cout << "Signature generated successfully." << endl;

    // Vérification de la signature
    int verify_status = DSA_verify(0, (const unsigned char*)BN_bn2hex(hash), BN_num_bytes(hash), sig, sig_len, dsa);
    if (verify_status == 1) {
        cout << "Signature valid: Yes" << endl;
    } else {
        cout << "Signature valid: No" << endl;
    }

    // Nettoyage
    BN_free(hash);
    delete[] sig;
    DSA_free(dsa);
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}
//pour compiler :g++ dsa.cpp -o dsa -lssl -lcrypto
