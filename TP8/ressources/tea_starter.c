#include <stdint.h>
#include <stdio.h>

#define NUM_ROUNDS 32
#define KEY_SIZE 8  // Taille de la clé en octets pour TEA

uint32_t F(uint32_t v, const uint32_t k[4], int i) {
    return ((v << 4) + k[2 * (i % 2)]) ^ (v + (0x9E3779B9 * ((i + 2) / 2))) ^ ((v >> 5) + k[1 + 2 * (i % 2)]);   
}

void feistel_enc(uint32_t v[2], const uint32_t k[4]) {
    uint32_t L = v[0], R = v[1];
    for (int i = 0; i < NUM_ROUNDS; i++) {
        uint32_t temp = R;
        R = L ^ F(R, k, i);
        L = temp;
    }
    v[0] = R;
    v[1] = L;
}

void feistel_dec(uint32_t v[2], const uint32_t k[4]) {
    uint32_t L = v[0], R = v[1];
    for (int i = NUM_ROUNDS - 1; i >= 0; i--) {
        uint32_t temp = R;
        R = L ^ F(R, k, i);
        L = temp;
    }
    v[0] = R;
    v[1] = L;
}

void ecb_encrypt(uint32_t * v, const uint32_t k[4], int nb_blocks) {
    for (int i = 0; i < nb_blocks; i++) {
        printf("ECB Encrypting block %d: %u %u\n", i, v[i * 2], v[i * 2 + 1]);
        feistel_enc(&v[i * 2], k);
        printf("After ECB Encryption block %d: %u %u\n", i, v[i * 2], v[i * 2 + 1]);
    }
}

void ecb_decrypt(uint32_t * v, const uint32_t k[4], int nb_blocks) {
    for (int i = 0; i < nb_blocks; i++) {
        printf("ECB Decrypting block %d: %u %u\n", i, v[i * 2], v[i * 2 + 1]);
        feistel_dec(&v[i * 2], k);
        printf("After ECB Decryption block %d: %u %u\n", i, v[i * 2], v[i * 2 + 1]);
    }
}

void cbc_encrypt(uint32_t *v, uint32_t vect[2], const uint32_t k[4], int nb_blocks) {
    for (int i = 0; i < nb_blocks; i++) {
        printf("CBC Encrypting block %d:\n", i);
        printf("Current IV: %u %u\n", vect[0], vect[1]);
        
        // XOR du texte clair avec l'IV avant de chiffrer
        v[i * 2] ^= vect[0];
        v[i * 2 + 1] ^= vect[1];
        printf("After XOR with IV: %u %u\n", v[i * 2], v[i * 2 + 1]);
        
        // Chiffrement Feistel
        feistel_enc(&v[i * 2], k);
        printf("After Feistel Encryption: %u %u\n", v[i * 2], v[i * 2 + 1]);
        
        // Mettre à jour l'IV avec les résultats du chiffrement
        vect[0] = v[i * 2];
        vect[1] = v[i * 2 + 1];
        printf("Updated IV for next block: %u %u\n", vect[0], vect[1]);
    }
}

void cbc_decrypt(uint32_t *v, uint32_t vect[2], const uint32_t k[4], int nb_blocks) {
    uint32_t prev[2];  // Utilisé pour mémoriser l'IV précédent
    
    for (int i = 0; i < nb_blocks; i++) {
        printf("CBC Decrypting block %d:\n", i);
        printf("Current IV: %u %u\n", vect[0], vect[1]);
        
        // Mémoriser les valeurs du bloc avant de commencer
        prev[0] = v[i * 2];
        prev[1] = v[i * 2 + 1];
        
        // Déchiffrement Feistel
        feistel_dec(&v[i * 2], k);
        printf("After Feistel Decryption: %u %u\n", v[i * 2], v[i * 2 + 1]);
        
        // XOR avec l'IV pour obtenir le texte clair
        v[i * 2] ^= vect[0];
        v[i * 2 + 1] ^= vect[1];
        printf("After XOR with IV: %u %u\n", v[i * 2], v[i * 2 + 1]);
        
        // Mettre à jour l'IV avec les valeurs du bloc actuel
        vect[0] = prev[0];
        vect[1] = prev[1];
        printf("Updated IV for next block: %u %u\n", vect[0], vect[1]);
    }
}


void ofb_stream(uint32_t* stream, uint32_t vect[2], const uint32_t k[4], int nb_blocks) {
    for (int i = 0; i < nb_blocks; i++) {
        printf("OFB generating stream block %d:\n", i);
        printf("Current IV: %u %u\n", vect[0], vect[1]);
        feistel_enc(vect, k);
        printf("After Feistel Encryption: %u %u\n", vect[0], vect[1]);
        stream[i * 2] = vect[0];
        stream[i * 2 + 1] = vect[1];
        printf("Stream block %d: %u %u\n", i, stream[i * 2], stream[i * 2 + 1]);
    }
}

void ofb_encrypt(uint32_t * v, uint32_t * stream, int nb_blocks) {
    for (int i = 0; i < nb_blocks; i++) {
        printf("OFB Encrypting block %d:\n", i);
        printf("Original block: %u %u\n", v[i * 2], v[i * 2 + 1]);
        v[i * 2] ^= stream[i * 2];
        v[i * 2 + 1] ^= stream[i * 2 + 1];
        printf("After XOR with stream: %u %u\n", v[i * 2], v[i * 2 + 1]);
    }
}



int main() {
    uint32_t k[4];
    uint32_t v[8];
    uint32_t vect[2];
    
    // Clé
    k[0] = 0b01110100011110101010010010010010;
    k[1] = 0b11110000101010100011010101101000;
    k[2] = 0b00100010111010010111010110110011;
    k[3] = 0b00001010110111010110100110100011;
    
    // Texte en clair (4 blocs)
    v[0] = 0b01010101010101010101010101010101;
    v[1] = 0b11111111111111110000000000000000;
    v[2] = 0b10101010101010101010101010101010;
    v[3] = 0b10101010101010101010101010101010;
    v[4] = 0b01010101010101010101010101010101;
    v[5] = 0b01010101010101010101010101010101;
    v[6] = 0b10101010101010101010101010101010;
    v[7] = 0b10101010101010101010101010101010;
    
    // Vecteur d'initialisation
    vect[0] = 0b00101101110101110101110010110001;
    vect[1] = 0b01110101101101100010101101010011;
    
    // Affichage du texte en clair
    printf("Plaintext:\n");
    for (int i = 0; i < 8; i++) {
        printf("%u\n", v[i]);
    }
    
    // Chiffrement Feistel
    feistel_enc(v, k);
    
    // Affichage du texte chiffré
    printf("\nCiphertext (Feistel):\n");
    for (int i = 0; i < 8; i++) {
        printf("%u\n", v[i]);
    }
    
    // Déchiffrement Feistel
    feistel_dec(v, k);
    
    // Affichage du texte déchiffré
    printf("\nDecrypted (Feistel):\n");
    for (int i = 0; i < 8; i++) {
        printf("%u\n", v[i]);
    }
    
    // Test ECB
    uint32_t v_ecb[8] = {0};
    for (int i = 0; i < 8; i++) v_ecb[i] = v[i];
    
    printf("\nECB Encryption:\n");
    ecb_encrypt(v_ecb, k, 4); // 4 blocs
    for (int i = 0; i < 8; i++) {
        printf("%u\n", v_ecb[i]);
    }
    
    printf("\nECB Decryption:\n");
    ecb_decrypt(v_ecb, k, 4); // 4 blocs
    for (int i = 0; i < 8; i++) {
        printf("%u\n", v_ecb[i]);
    }
    
    // Test CBC
    uint32_t v_cbc[8] = {0};
    for (int i = 0; i < 8; i++) v_cbc[i] = v[i];
    
    printf("\nCBC Encryption:\n");
    cbc_encrypt(v_cbc, vect, k, 4); // 4 blocs
    for (int i = 0; i < 8; i++) {
        printf("%u\n", v_cbc[i]);
    }
    
    printf("\nCBC Decryption:\n");
    cbc_decrypt(v_cbc, vect, k, 4); // 4 blocs
    for (int i = 0; i < 8; i++) {
        printf("%u\n", v_cbc[i]);
    }
    
    // Test OFB
    uint32_t v_ofb[8] = {0};
    uint32_t stream[8] = {0};
    for (int i = 0; i < 8; i++) v_ofb[i] = v[i];
    
    printf("\nOFB Stream:\n");
    ofb_stream(stream, vect, k, 4); // 4 blocs
    for (int i = 0; i < 8; i++) {
        printf("%u\n", stream[i]);
    }
    
    printf("\nOFB Encryption:\n");
    ofb_encrypt(v_ofb, stream, 4); // 4 blocs
    for (int i = 0; i < 8; i++) {
        printf("%u\n", v_ofb[i]);
    }

    return 0;
}
