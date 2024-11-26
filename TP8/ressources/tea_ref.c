#include <stdint.h>
#include <stdio.h>

void encrypt(uint32_t v[2], const uint32_t k[4]) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0, i;
    uint32_t delta = 0x9E3779B9;
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
    for (i = 0; i < 32; i++) {
        sum += delta;
        v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
    }
    v[0] = v0;
    v[1] = v1;
}

void decrypt(uint32_t v[2], const uint32_t k[4]) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0xC6EF3720, i;
    uint32_t delta = 0x9E3779B9;
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
    for (i = 0; i < 32; i++) {
        v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
        v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        sum -= delta;
    }
    v[0] = v0;
    v[1] = v1;
}

void compress(uint32_t iv[2], uint32_t v[2], const uint32_t k[4]) {
    uint32_t tmp[2] = { iv[0], iv[1] };
    encrypt(tmp, k);
    v[0] ^= tmp[0];
    v[1] ^= tmp[1];
}

void hash(uint32_t *message, int message_len, uint32_t iv[2], const uint32_t k[4], uint32_t *output) {
    uint32_t h[2];
    h[0] = iv[0];  // Initialiser le vecteur de hachage avec l'IV
    h[1] = iv[1];

    // Traiter les blocs du message
    for (int i = 0; i < message_len; i += 2) {
        uint32_t v[2] = { message[i], message[i + 1] };
        compress(h, v, k);  // Appliquer la fonction de compression
        h[0] = v[0];
        h[1] = v[1];
    }

    output[0] = h[0];
    output[1] = h[1];
}

// Fonction de calcul du HMAC
void hash_mac(uint32_t *v, int message_len, uint32_t iv[2], const uint32_t k[4], uint32_t *output) {
    // Concaténer la clé k avec le message v
    uint32_t k_and_v[message_len + 4]; // Allocation pour la concaténation de k et v
    for (int i = 0; i < 4; i++) {
        k_and_v[i] = k[i];  // Ajouter la clé k
    }
    for (int i = 0; i < message_len; i++) {
        k_and_v[i + 4] = v[i];  // Ajouter le message v
    }

    // Calculer le hachage de la concaténation (k ∥ v)
    hash(k_and_v, message_len + 4, iv, k, output);
}

// Fonction de vérification du HMAC
int hash_mac_verification(uint32_t *v, int message_len, uint32_t iv[2], const uint32_t k[4], uint32_t *c) {
    uint32_t output[2];
    // Calculer le HMAC pour le message v avec la clé k
    hash_mac(v, message_len, iv, k, output);

    // Comparer le code calculé avec le code reçu c
    if (output[0] == c[0] && output[1] == c[1]) {
        return 1;  // Le code est valide
    } else {
        return 0;  // Le code est invalide
    }
}

void xor_blocks(uint32_t *block1, uint32_t *block2) {
    block1[0] ^= block2[0];
    block1[1] ^= block2[1];
}

// Fonction de chiffrement CBC avec TEA et HMAC pour l'intégrité
void cbc_encrypt_mac(uint32_t *v, int nb_blocks, const uint32_t k[4], uint32_t iv[2], uint32_t ivh[2], uint32_t *output) {
    uint32_t ciphertext[2];  // Pour stocker le texte chiffré d'un bloc
    uint32_t hmac_input[nb_blocks * 2];  // Pour construire le message à hacher
    uint32_t hmac_output[2];  // Pour stocker le HMAC final

    uint32_t previous_block[2] = { iv[0], iv[1] };  // Initialiser avec le vecteur d'initialisation

    // Chiffrement CBC du message
    for (int i = 0; i < nb_blocks; i++) {
        xor_blocks(&v[i * 2], previous_block);  // XOR du bloc avec le bloc précédent (CBC)
        encrypt(&v[i * 2], k);  // Chiffrement du bloc avec TEA
        ciphertext[0] = v[i * 2];
        ciphertext[1] = v[i * 2 + 1];

        // Copier le texte chiffré pour le calcul du HMAC
        hmac_input[i * 2] = ciphertext[0];
        hmac_input[i * 2 + 1] = ciphertext[1];

        // Mettre à jour le bloc précédent (pour CBC)
        previous_block[0] = ciphertext[0];
        previous_block[1] = ciphertext[1];
    }

    // Calcul du HMAC pour le texte chiffré
    hash(hmac_input, nb_blocks * 2, ivh, k, hmac_output);

    // L'output contient le texte chiffré et le HMAC
    for (int i = 0; i < nb_blocks * 2; i++) {
        output[i] = hmac_input[i];  // Texte chiffré
    }
    output[nb_blocks * 2] = hmac_output[0];  // HMAC
    output[nb_blocks * 2 + 1] = hmac_output[1];  // HMAC
}

// Fonction de déchiffrement CBC avec vérification du HMAC
int cbc_decrypt_mac(uint32_t *v, int nb_blocks, const uint32_t k[4], uint32_t iv[2], uint32_t ivh[2], uint32_t *encrypted_message) {
    uint32_t ciphertext[2];  // Pour stocker chaque bloc du message chiffré
    uint32_t decrypted_message[nb_blocks * 2];  // Pour stocker le message déchiffré
    uint32_t hmac_input[nb_blocks * 2];  // Pour construire le message à hacher
    uint32_t hmac_output[2];  // Pour stocker le HMAC final

    uint32_t previous_block[2] = { iv[0], iv[1] };  // Initialiser avec le vecteur d'initialisation

    // Extraire les blocs chiffrés et vérifier le HMAC
    for (int i = 0; i < nb_blocks; i++) {
        ciphertext[0] = encrypted_message[i * 2];
        ciphertext[1] = encrypted_message[i * 2 + 1];
        xor_blocks(&ciphertext[0], previous_block);  // XOR avec le bloc précédent (CBC)
        decrypt(ciphertext, k);  // Déchiffrer avec TEA

        // Copier le texte déchiffré
        decrypted_message[i * 2] = ciphertext[0];
        decrypted_message[i * 2 + 1] = ciphertext[1];

        // Mettre à jour le bloc précédent (pour CBC)
        previous_block[0] = encrypted_message[i * 2];
        previous_block[1] = encrypted_message[i * 2 + 1];
        
        // Construire le message pour HMAC
        hmac_input[i * 2] = encrypted_message[i * 2];
        hmac_input[i * 2 + 1] = encrypted_message[i * 2 + 1];
    }

    // Calculer le HMAC pour vérifier l'intégrité
    hash(hmac_input, nb_blocks * 2, ivh, k, hmac_output);

    // Vérifier le HMAC
    if (encrypted_message[nb_blocks * 2] != hmac_output[0] || encrypted_message[nb_blocks * 2 + 1] != hmac_output[1]) {
        return 0;  // Code HMAC invalide, donc déchiffrement échoue
    }

    // Si le HMAC est valide, retourner le message déchiffré
    for (int i = 0; i < nb_blocks * 2; i++) {
        v[i] = decrypted_message[i];  // Retourner le message déchiffré
    }

    return 1;  // Déchiffrement réussi
}

int main() {
    uint32_t k[4] = {
        0b01110100011110101010010010010010,
        0b11110000101010100011010101101000,
        0b00100010111010010111010110110011,
        0b00001010110111010110100110100011
    };

    uint32_t iv[2] = { 0b00101101110101110101110010110001, 0b01110101101101100010101101010011 };
    uint32_t ivh[2] = { 0x12345678, 0x87654321 }; // Initialiser IV pour HMAC
    
    uint32_t message[8] = {
        0b01010101010101010101010101010101,
        0b11111111111111110000000000000000,
        0b10101010101010101010101010101010,
        0b10101010101010101010101010101010,
        0b01010101010101010101010101010101,
        0b01010101010101010101010101010101,
        0b10101010101010101010101010101010,
        0b10101010101010101010101010101010
    };

    uint32_t encrypted_message[16];  // Sortie du chiffrement et du HMAC
    cbc_encrypt_mac(message, 8, k, iv, ivh, encrypted_message);

    // Affichage du texte chiffré et du HMAC
    printf("Encrypted message with HMAC:\n");
    for (int i = 0; i < 16; i++) {
        printf("%u ", encrypted_message[i]);
    }
    printf("\n");

    uint32_t decrypted_message[8];  // Pour stocker le message déchiffré
    int result = cbc_decrypt_mac(decrypted_message, 8, k, iv, ivh, encrypted_message);

    if (result == 1) {
        printf("Déchiffrement réussi: ");
        for (int i = 0; i < 8; i++) {
            printf("%u ", decrypted_message[i]);
        }
    } else {
        printf("Échec du déchiffrement (HMAC invalide)");
    }

    return 0;
}
