#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

/*  S-box and its inverse */
static const uint8_t S[16] = {0x3, 0xE, 0x1, 0xA, 0x4, 0x9, 0x5, 0x6,
                              0x8, 0xB, 0xF, 0x2, 0xD, 0xC, 0x0, 0x7};

static const uint8_t S_inv[16] = {0xE, 0x2, 0xB, 0x0, 0x4, 0x6, 0x7, 0xF,
                                  0x8, 0x5, 0x3, 0x9, 0xD, 0xC, 0x1, 0xA};

/* Cipher for cryptanalysis */
class Cipher {
private:
    uint8_t k0;
    uint8_t k1;

    uint8_t roundFunc(uint8_t input) {
        uint8_t cypher = XOR(input, k0);
        cypher = evaluateS(cypher);
        cypher = XOR(cypher, k1);
        return cypher;
    }

    uint8_t roundFunc_inv(uint8_t input) {
        uint8_t plain = XOR(input, k1);
        plain = evaluateSinv(plain);
        plain = XOR(plain, k0);
        return plain;
    }

public:
    Cipher() {
        k0 = rand() % 16; // Random subkey0
        k1 = rand() % 16; // Random subkey1
        printf("First sub-key k0 = %x\n", k0);
        printf("Second sub-key k1 = %x\n\n", k1);
    }

    Cipher(uint8_t key0, uint8_t key1) {
        k0 = key0;
        k1 = key1;
    }

    uint8_t encrypt(uint8_t input) {
        uint8_t cypher = input;
        for (int i = 0; i < 16; i++) {
            cypher = roundFunc(cypher);
        }
        return cypher;
    }

    uint8_t decrypt(uint8_t input) {
        uint8_t plain = input;
        for (int i = 0; i < 16; i++) {
            plain = roundFunc_inv(plain);
        }
        return plain;
    }

    static uint8_t XOR(const uint8_t& input, const uint8_t& key) {
        return input ^ key;
    }

    static uint8_t evaluateS(const uint8_t& input) {
        return S[input];
    }

    static uint8_t evaluateSinv(const uint8_t& input) {
        return S_inv[input];
    }
};

class Cryptanalysis {
private:
    uint8_t knownP0[1000];
    uint8_t knownP1[1000];
    uint8_t knownC0[1000];
    uint8_t knownC1[1000];
    uint8_t goodP0, goodP1, goodC0, goodC1;

    int chardatmax;
    int chardat0[16];
    uint8_t T[16][16]; // Table for counting occurrences
    vector<pair<uint8_t, uint8_t>> bestProba;
    vector<pair<uint8_t, uint8_t>> xAndXprime[16];

public:
    Cryptanalysis() {
        chardatmax = 0;
    }

    void findBestDiffs() {
        uint8_t i, j, X, Xp, Y, Yp, DX, DY;
        
        // Initialize the difference table
        for (i = 0; i < 16; ++i) {
            for (j = 0; j < 16; ++j) {
                T[i][j] = 0;
            }
        }

        printf("\nCreating XOR differential table:\n");

        // Fill the difference table
        for (X = 0; X < 16; X++) {
            for (Xp = 0; Xp < 16; Xp++) {
                Y = Cipher::evaluateS(X);
                Yp = Cipher::evaluateS(Xp);

                DX = (X ^ Xp);
                DY = (Y ^ Yp);

                if (X + Xp != 0) {
                    xAndXprime[DX].push_back(make_pair(X, Xp));
                }
                T[DX][DY]++;
            }
        }

        // Display the difference table
        for (i = 0; i < 16; ++i) {
            printf("[");
            for (j = 0; j < 16; ++j) {
                printf(" %u ", T[i][j]);
            }
            printf("]\n");
        }

        // Find the best values (most probable differentials)
        printf("\nDisplaying most probable differentials:\n");

        uint8_t bestValue = 0;
        for (i = 0; i < 16; i++) {
            for (j = 0; j < 16; j++) {
                if (!(i == 0 && j == 0) && bestValue < T[i][j]) {
                    bestValue = T[i][j];
                }
            }
        }

        // Store the pairs corresponding to the best value
        bestProba.clear();
        genPairs(bestValue);
    }

    void genPairs(uint8_t diffIn) {
        printf("\nGenerating known pairs with input differential of %x.\n", diffIn);

        uint8_t DX, DY;
        for (DX = 0; DX < 16; DX++) {
            for (DY = 0; DY < 16; DY++) {
                if (!(DX == 0 && DY == 0) && diffIn == T[DX][DY]) {
                    bestProba.push_back(make_pair(DX, DY));
                }
            }
        }
    }

    void findGoodPair(int diffOut, int nbPairs) {
        printf("\nRecherche de paires valides :\n");

        int count = 0;

        // Parcours des paires possibles pour identifier celles qui correspondent au différentiel de sortie attendu
        for (const auto& pair : xAndXprime[diffOut]) {
            uint8_t X = pair.first;      // Première valeur de la paire
            uint8_t Xp = pair.second;     // Deuxième valeur de la paire
            uint8_t Y = Cipher::evaluateS(X);   // Sortie de la S-box pour X
            uint8_t Yp = Cipher::evaluateS(Xp); // Sortie de la S-box pour X'

            // Calcul de la différence de sortie réelle
            uint8_t actualDiffOut = Y ^ Yp;

            // Vérification de la correspondance avec le différentiel de sortie attendu
            if (actualDiffOut == diffOut) {
                printf("Paire trouvée : (X = %x, X' = %x) avec différence de sortie : %x\n", X, Xp, actualDiffOut);
                count++;

                // Arrête si on atteint le nombre de paires requis
                if (count >= nbPairs) {
                    break;
                }
            }
        }
    }


    void crack(int nbPairs) {
    printf("\nForçage brut sur l'espace réduit de clés :\n");

    for (uint8_t k0_guess = 0; k0_guess < 16; k0_guess++) {
        for (uint8_t k1_guess = 0; k1_guess < 16; k1_guess++) {
            bool validKey = true;

            for (int i = 0; i < nbPairs; i++) {
                uint8_t X = xAndXprime[0][i].first;
                uint8_t Xp = xAndXprime[0][i].second;

                // Chiffrement des paires avec les sous-clés devinées
                uint8_t Y = Cipher::XOR(Cipher::evaluateS(Cipher::XOR(X, k0_guess)), k1_guess);
                uint8_t Yp = Cipher::XOR(Cipher::evaluateS(Cipher::XOR(Xp, k0_guess)), k1_guess);

                // Vérification de la différence de sortie
                if ((Y ^ Yp) != 0) { // 0 représente un différentiel de sortie attendu
                    validKey = false;
                    break;
                }
            }

            // Si toutes les paires correspondent, la clé devinée est probablement correcte
            if (validKey) {
                printf("Clé probable trouvée : k0 = %x, k1 = %x\n", k0_guess, k1_guess);
                return; // Quitter après la première clé probable trouvée
            }
        }
    }

    printf("Aucune clé valide trouvée dans l'espace de recherche.\n");
}

};

//////////////////////////////////////////////////////////////////
//                             MAIN                             //
//////////////////////////////////////////////////////////////////

int main() {
    srand(time(NULL)); // Randomize values per run
    Cipher cipher;
    uint8_t message = rand() % 16;
    printf("Producing a random message: %x\n", message);

    uint8_t ciphertext = cipher.encrypt(message);
    printf("Encrypted message: %x\n", ciphertext);

    uint8_t plaintext = cipher.decrypt(ciphertext);
    printf("Decrypted message: %x\n", plaintext);

    if (message == plaintext) {
        printf(" --> Success\n");
    } else {
        printf(" --> Failure\n");
    }

    // Cryptanalysis phase
    Cryptanalysis cryptanalysis;
    cryptanalysis.findBestDiffs(); // Find some good differentials in the S-box
    
    // Step 2: Use a specific differential to find pairs
    int diffOut = 0x3; // Example differential for testing
    int nbPairs = 5;   // Example number of pairs to find
    cryptanalysis.findGoodPair(diffOut, nbPairs);

    // Step 3: Attempt to crack the key using identified pairs
    cryptanalysis.crack(nbPairs);
    return 0;
}
