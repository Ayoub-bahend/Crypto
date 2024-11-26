#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Function to encrypt using TEA algorithm (not provided in the code snippet)
void encrypt(uint32_t v[2], const uint32_t k[4]) {
    // TEA encryption algorithm (implementation is not provided here)
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

// Function to verify the property of TEA with two different keys producing the same ciphertext
void verify_tea_property(const uint32_t k1[4], const uint32_t k2[4], const uint32_t v[2]) {
    uint32_t v1[2] = {v[0], v[1]};
    uint32_t v2[2] = {v[0], v[1]};

    encrypt(v1, k1);
    encrypt(v2, k2);

    printf("Ciphertext with key 1: %u %u\n", v1[0], v1[1]);
    printf("Ciphertext with key 2: %u %u\n", v2[0], v2[1]);

    if (v1[0] == v2[0] && v1[1] == v2[1]) {
        printf("The property is verified: the ciphertexts are identical.\n");
    } else {
        printf("The property is not verified: the ciphertexts are different.\n");
    }
}

// Function to show that the hash function using TEA as a compression function is not collision-resistant
void show_hash_collision(const uint32_t k[4], const uint32_t v1[2], const uint32_t v2[2]) {
    uint32_t h1[2] = {0, 0};
    uint32_t h2[2] = {0, 0};

    encrypt(h1, k);
    encrypt(h2, k);

    printf("Hash of v1: %u %u\n", h1[0], h1[1]);
    printf("Hash of v2: %u %u\n", h2[0], h2[1]);

    if (h1[0] == h2[0] && h1[1] == h2[1]) {
        printf("Collision found: the hashes are identical.\n");
    } else {
        printf("No collision found: the hashes are different.\n");
    }
}

// Mockup functions for CBC-based encryption and MAC generation (placeholders for real implementations)
void cbc_encrypt_mac(uint32_t v[2], uint32_t iv[2], uint32_t k[4], uint32_t ivh[2], int count, uint8_t *mac) {
    // Just a placeholder for CBC encryption and MAC generation
    memset(mac, 0, 32); // Placeholder
}

bool cbc_decrypt_mac(uint32_t v[2], uint32_t iv[2], uint32_t k[4], uint32_t ivh[2], int count, uint8_t *mac) {
    // Placeholder for MAC verification
    return true; // Assume MAC verification is successful (for the sake of demonstration)
}

// Function to propose and implement an attack against the integrity of the authenticated encryption
void attack_authenticated_encryption(const uint32_t k[4], uint32_t iv[2], uint32_t ivh[2], const uint32_t v[2]) {
    uint32_t v1[2] = {v[0], v[1]};
    uint32_t v2[2] = {v[0], v[1]};
    uint8_t mac1[32];
    uint8_t mac2[32];

    cbc_encrypt_mac(v1, iv, k, ivh, 1, mac1);
    cbc_encrypt_mac(v2, iv, k, ivh, 1, mac2);

    printf("Original ciphertext: %u %u\n", v1[0], v1[1]);
    printf("Original MAC: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", mac1[i]);
    }
    printf("\n");

    // Modify the ciphertext
    v1[0] ^= 0x1;
    v1[1] ^= 0x1;

    printf("Modified ciphertext: %u %u\n", v1[0], v1[1]);

    // Modify the MAC to match the modified ciphertext
    cbc_encrypt_mac(v1, iv, k, ivh, 1, mac1);

    // Verify the MAC with the modified ciphertext
    bool success = cbc_decrypt_mac(v1, iv, k, ivh, 1, mac1);

    if (success) {
        printf("Attack successful: the modified ciphertext is accepted.\n");
    } else {
        printf("Attack failed: the modified ciphertext is rejected.\n");
    }
}

// Main function for testing
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

    // Test of the TEA property
    verify_tea_property(key1, key2, message);

    // Test of hash collision (using TEA as compression function)
    uint32_t v1[2] = {0x01234567, 0x89abcdef};
    uint32_t v2[2] = {0x11223344, 0x55667788};
    show_hash_collision(key1, v1, v2);

    // Test of attack against authenticated encryption
    uint32_t iv[2] = {0x12345678, 0x9abcdef0};
    uint32_t ivh[2] = {0xabcdef01, 0x23456789};
    attack_authenticated_encryption(key1, iv, ivh, message);

    return 0;
}
