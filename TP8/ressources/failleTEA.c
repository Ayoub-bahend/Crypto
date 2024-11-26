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

int main() {
    uint32_t key1[4] = {
        0b01110100011110101010010010010010,
        0b11110000101010100011010101101000,
        0b00100010111010010111010110110011,
        0b00001010110111010110100110100011
    };
    
    uint32_t key2[4] = {
        0b01110100011110101010010010010000,  // Modification du dernier bit du premier bloc
        0b11110000101010100011010101101001,  // Modification du dernier bit du deuxième bloc
        0b00100010111010010111010110110011,
        0b00001010110111010110100110100011
    };

    uint32_t message[2] = { 0b01010101010101010101010101010101, 0b11111111111111110000000000000000 };

    // Chiffrement avec la première clé
    encrypt(message, key1);
    printf("Encrypted with key1: %u %u\n", message[0], message[1]);

    uint32_t encrypted1[2] = { message[0], message[1] };  // Sauvegarder le texte chiffré pour key1

    // Réinitialisation du message
    message[0] = 0b01010101010101010101010101010101;
    message[1] = 0b11111111111111110000000000000000;

    // Chiffrement avec la deuxième clé
    encrypt(message, key2);
    printf("Encrypted with key2: %u %u\n", message[0], message[1]);

    uint32_t encrypted2[2] = { message[0], message[1] };  // Sauvegarder le texte chiffré pour key2

    // Comparer les deux textes chiffrés
    if (encrypted1[0] == encrypted2[0] && encrypted1[1] == encrypted2[1]) {
        printf("The two encrypted messages are identical.\n");
    } else {
        printf("The two encrypted messages are different.\n");
    }

    return 0;
}
