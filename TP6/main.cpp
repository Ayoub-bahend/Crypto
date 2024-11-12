#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <stdint.h>

using namespace std;

static const uint8_t S[16] = { 
  0x3, 0xE, 0x1, 0xA, 0x4, 0x9, 0x5, 0x6, 0x8, 0xB, 0xF, 0x2, 0xD, 0xC, 0x0, 0x7
};

static const uint8_t S_inv[16] = {
  0xE, 0x2, 0xB, 0x0, 0x4, 0x6, 0x7, 0xF, 0x8, 0x5, 0x3, 0x9, 0xD, 0xC, 0x1, 0xA
};

class Cipher
{
private:
  uint8_t k0;
  uint8_t k1;

  uint8_t roundFunc(uint8_t input)
  {
    return S[input];
  }

  uint8_t roundFunc_inv(uint8_t input)
  {
    return S_inv[input];
  }

public:
  Cipher()
  {   
    k0 = rand() % 16;
    k1 = rand() % 16;
    printf("First sub-key k0 = %x\n", k0);
    printf("Second sub-key k1 = %x\n\n", k1);
  }

  uint8_t encrypt(uint8_t input)
  {
    uint8_t state = input ^ k0;
    state = roundFunc(state);
    state = state ^ k1;
    state = roundFunc(state);
    return state;
  }

  uint8_t decrypt(uint8_t input)
  {
    uint8_t state = roundFunc_inv(input);
    state = state ^ k1;
    state = roundFunc_inv(state);
    state = state ^ k0;
    return state;
  }
};

class Cryptanalysis
{
public:
  uint8_t knownP0[1000];
  uint8_t knownP1[1000];
  uint8_t knownC0[1000];
  uint8_t knownC1[1000];

  void test_roundFunc()
  {
    for (uint8_t i = 0; i < 16; ++i) {
      uint8_t result = S[i];
      printf("roundFunc(%x) = %x\n", i, result);
    }
  }

  void test_roundFunc_inv()
  {
    for (uint8_t i = 0; i < 16; ++i) {
      uint8_t result = S_inv[i];
      printf("roundFunc_inv(%x) = %x\n", i, result);
    }
  }

  void test_encryption_decryption(Cipher &cipher)
  {
    for (uint8_t i = 0; i < 16; ++i) {
      uint8_t ciphertext = cipher.encrypt(i);
      uint8_t decrypted = cipher.decrypt(ciphertext);
      printf("Message: %x, Chiffre: %x, Déchiffre: %x\n", i, ciphertext, decrypted);
      if (i == decrypted) {
        printf(" --> Success\n");
      } else {
        printf(" --> Failure\n");
      }
    }
  }

  void test_findBestDiffs()
  {
    uint8_t i, j;
    uint8_t X, Xp, Y, Yp, DX, DY; 
    uint8_t T[16][16] = {0};

    printf("\nCreating XOR differential table:\n");

    for (i = 0; i < 16; ++i) {
      for (j = 0; j < 16; ++j) {
        X = i;
        Xp = j;
        Y = S[X];
        Yp = S[Xp];
        DX = X ^ Xp;
        DY = Y ^ Yp;
        T[DX][DY]++;
      }
    }

    for (i = 0; i < 16; ++i) {
      printf("[");
      for (j = 0; j < 16; ++j) {
        printf(" %u ", T[i][j]);
      }
      printf("]\n");
    }
  }

  void genPairs(Cipher &cipher, uint8_t diffIn, int nbPairs)
  {
    for (int i = 0; i < nbPairs; ++i) {
      uint8_t P0 = rand() % 16;
      uint8_t P1 = P0 ^ diffIn;
      uint8_t C0 = cipher.encrypt(P0);
      uint8_t C1 = cipher.encrypt(P1);

      knownP0[i] = P0;
      knownP1[i] = P1;
      knownC0[i] = C0;
      knownC1[i] = C1;
    }
  }

  void findGoodPair(uint8_t diffOut, int nbPairs)
  {
    for (int i = 0; i < nbPairs; ++i) {
      if ((knownC0[i] ^ knownC1[i]) == diffOut) {
        printf("Good pair found: P0=%x, P1=%x, C0=%x, C1=%x\n", knownP0[i], knownP1[i], knownC0[i], knownC1[i]);
        return;
      }
    }
    printf("No good pair found!\n");
  }

  void crack(int nbPairs)
  {
    printf("\nBrute forcing reduced keyspace:\n");

    for (int k0 = 0; k0 < 16; ++k0) {
      for (int k1 = 0; k1 < 16; ++k1) {
        // Here you'd use the known pairs to test the keys
        printf("Trying key: k0=%x, k1=%x\n", k0, k1);
      }
    }
  }
};

int main()
{
    srand(time(NULL));

    Cipher cipher;
    Cryptanalysis analysis;

    // Test de la fonction roundFunc
    printf("\n-- Test de roundFunc --\n");
    analysis.test_roundFunc();

    // Test de la fonction roundFunc_inv
    printf("\n-- Test de roundFunc_inv --\n");
    analysis.test_roundFunc_inv();

    // Test de l'encryptage et du décryptage
    printf("\n-- Test de l'encryptage et du décryptage --\n");
    analysis.test_encryption_decryption(cipher);

    // Test de la création de la table des différences
    printf("\n-- Test de la table des différences --\n");
    analysis.test_findBestDiffs();

    // Test de la génération des paires
    int nbPairs = 10;
    uint8_t diffIn = 3; // Différence d'entrée
    printf("\n-- Test de la génération des paires --\n");
    analysis.genPairs(cipher, diffIn, nbPairs);

    // Test de la recherche d'une bonne paire
    uint8_t diffOut = 0; // Différence de sortie
    printf("\n-- Test de la recherche d'une bonne paire --\n");
    analysis.findGoodPair(diffOut, nbPairs);

    // Test de l'attaque par force brute
    printf("\n-- Test de l'attaque par force brute --\n");
    analysis.crack(nbPairs);

    return 0;
}
