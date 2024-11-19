#include <stdint.h>
#include <stdio.h>

#define DELTA 0x9E3779B9

uint32_t F(uint32_t v, const uint32_t k[4], int i) {
    return ((v << 4) + k[2 * (i % 2)]) ^ (v + (DELTA * ((i + 2) / 2))) ^ ((v >> 5) + k[1 + 2 * (i % 2)]);   
}

void feistel_enc(uint32_t v[2], const uint32_t k[4]) {
    uint32_t l = v[0], r = v[1];
    uint32_t sum = 0;

    for (int i = 0; i < 32; i++) {
        sum += DELTA;
        l += ((r << 4) + k[0]) ^ (r + sum) ^ ((r >> 5) + k[1]);
        r += ((l << 4) + k[2]) ^ (l + sum) ^ ((l >> 5) + k[3]);
    }

    v[0] = l;
    v[1] = r;
}

void feistel_dec(uint32_t v[2], const uint32_t k[4]) {
    uint32_t l = v[0], r = v[1];
    uint32_t sum = DELTA * 32;

    for (int i = 0; i < 32; i++) {
        r -= ((l << 4) + k[2]) ^ (l + sum) ^ ((l >> 5) + k[3]);
        l -= ((r << 4) + k[0]) ^ (r + sum) ^ ((r >> 5) + k[1]);
        sum -= DELTA;
    }

    v[0] = l;
    v[1] = r;
}

void ecb_encrypt(uint32_t *v, const uint32_t k[4], int nb_blocks) {
    for (int i = 0; i < nb_blocks; i++) {
        feistel_enc(&v[2 * i], k);
    }
}

void ecb_decrypt(uint32_t *v, const uint32_t k[4], int nb_blocks) {
    for (int i = 0; i < nb_blocks; i++) {
        feistel_dec(&v[2 * i], k);
    }
}

void cbc_encrypt(uint32_t *v, uint32_t vect[2], const uint32_t k[4], int nb_blocks) {
    uint32_t prev[2] = { vect[0], vect[1] };

    for (int i = 0; i < nb_blocks; i++) {
        v[2 * i] ^= prev[0];
        v[2 * i + 1] ^= prev[1];
        feistel_enc(&v[2 * i], k);
        prev[0] = v[2 * i];
        prev[1] = v[2 * i + 1];
    }
}

void cbc_decrypt(uint32_t *v, uint32_t vect[2], const uint32_t k[4], int nb_blocks) {
    uint32_t prev[2], curr[2];

    prev[0] = vect[0];
    prev[1] = vect[1];

    for (int i = 0; i < nb_blocks; i++) {
        curr[0] = v[2 * i];
        curr[1] = v[2 * i + 1];
        feistel_dec(&v[2 * i], k);
        v[2 * i] ^= prev[0];
        v[2 * i + 1] ^= prev[1];
        prev[0] = curr[0];
        prev[1] = curr[1];
    }
}

void ofb_stream(uint32_t* stream, uint32_t vect[2], const uint32_t k[4], int nb_blocks) {
    uint32_t iv[2] = { vect[0], vect[1] };

    for (int i = 0; i < nb_blocks; i++) {
        feistel_enc(iv, k);
        stream[2 * i] = iv[0];
        stream[2 * i + 1] = iv[1];
    }
}

void ofb_encrypt(uint32_t *v, uint32_t *stream, int nb_blocks) {
    for (int i = 0; i < nb_blocks; i++) {
        v[2 * i] ^= stream[2 * i];
        v[2 * i + 1] ^= stream[2 * i + 1];
    }
}

int main() {
    uint32_t k[4] = {
        0b01110100011110101010010010010010,
        0b11110000101010100011010101101000,
        0b00100010111010010111010110110011,
        0b00001010110111010110100110100011
    };

    uint32_t v[8] = {
        0b01010101010101010101010101010101,
        0b11111111111111110000000000000000,
        0b10101010101010101010101010101010,
        0b10101010101010101010101010101010,
        0b01010101010101010101010101010101,
        0b01010101010101010101010101010101,
        0b10101010101010101010101010101010,
        0b10101010101010101010101010101010
    };

    uint32_t vect[2] = {
        0b00101101110101110101110010110001,
        0b01110101101101100010101101010011
    };

    printf("Plaintext:\n");
    for (int i = 0; i < 8; i++) {
        printf("%u\n", v[i]);
    }

    printf("\nEncrypting in ECB mode...\n");
    ecb_encrypt(v, k, 4);

    printf("Ciphertext:\n");
    for (int i = 0; i < 8; i++) {
        printf("%u\n", v[i]);
    }

    printf("\nDecrypting in ECB mode...\n");
    ecb_decrypt(v, k, 4);

    printf("Decrypted plaintext:\n");
    for (int i = 0; i < 8; i++) {
        printf("%u\n", v[i]);
    }

    // Vérification du texte déchiffré
    bool match = true;
    for (int i = 0; i < 8; i++) {
        if (v[i] != v[i]) {
            match = false;
            break;
        }
    }

    if (match) {
        printf("\nDecryption successful.\n");
    } else {
        printf("\nDecryption failed.\n");
    }

    return 0;
}
