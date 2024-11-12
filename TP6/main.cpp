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

// Cipher to cryptanalyse
class Cipher
{
private:
  uint8_t k0;
  uint8_t k1;

  uint8_t roundFunc(uint8_t input)
  {
    // Apply S-box transformation to input
    return S[input];
  }

  uint8_t roundFunc_inv(uint8_t input)
  {
    // Apply inverse S-box transformation to input
    return S_inv[input];
  }

public:
  Cipher()
  {   
    k0 = rand() % 16;                  // Create random subkey k0
    k1 = rand() % 16;                  // Create random subkey k1
    
    printf("First sub-key k0 = %x\n", k0);
    printf("Second sub-key k1 = %x\n\n", k1);
  }

  Cipher(uint8_t key0, uint8_t key1)
  {   
    k0 = key0;
    k1 = key1;
  }

  uint8_t encrypt(uint8_t input)
  {
    uint8_t state = input ^ k0;         // XOR with k0
    state = roundFunc(state);           // Apply S-box
    state = state ^ k1;                 // XOR with k1
    state = roundFunc(state);           // Apply S-box again
    return state;
  }

  uint8_t decrypt(uint8_t input)
  {
    uint8_t state = roundFunc_inv(input); // Inverse S-box transformation
    state = state ^ k1;                 // XOR with k1
    state = roundFunc_inv(state);       // Inverse S-box transformation
    state = state ^ k0;                 // XOR with k0
    return state;
  }
};


class Cryptanalysis
{
private:
  uint8_t knownP0[1000];
  uint8_t knownP1[1000];
  uint8_t knownC0[1000];
  uint8_t knownC1[1000];
  
  uint8_t goodP0, goodP1, goodC0, goodC1;
  int chardatmax;
  int chardat0[16];

public:
  Cryptanalysis(){
    chardatmax = 0;
  }

  /* Difference Distribution Table of the S-box */
  void findBestDiffs(void){
    uint8_t i, j;
    uint8_t X, Xp, Y, Yp, DX, DY; 
    uint8_t T[16][16] = {0}; // Table to store occurrences
    for (i = 0; i < 16; ++i) {
      for (j = 0; j < 16; ++j) {
        T[i][j] = 0;
      }
    }

    printf("\nCreating XOR differential table:\n");
    
    /* Question 1 : Completer le code pour afficher la matrice T des différences */
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

    /* Display differences in table */
    for (i = 0; i < 16; ++i) {
      printf("[");
      for (j = 0; j < 16; ++j) {
        printf(" %u ", T[i][j]);
      }
      printf("]\n");
    }

    printf("\nDisplaying most probable differentials:\n");

    /* TODO: Identify the most probable differentials that will be exploited in the attack */
  }

  void genCharData(int diffIn, int diffOut)
  {
    printf("\nGenerating possible intermediate values based on differential (%x --> %x):\n", diffIn, diffOut);

    // TODO: Implement the code to generate candidate intermediate values based on the differential
  }

  void genPairs(Cipher cipher, uint8_t diffIn, int nbPairs)
  {
    printf("\nGenerating %i known pairs with input differential of %x.\n", nbPairs, diffIn);

    /* Question 2 : Generate pairs of ciphertexts with the correct difference */
    for (int i = 0; i < nbPairs; ++i) {
      uint8_t P0 = rand() % 16;        // Random plaintext
      uint8_t P1 = P0 ^ diffIn;        // Compute P1 based on input differential
      uint8_t C0 = cipher.encrypt(P0); // Encrypt P0
      uint8_t C1 = cipher.encrypt(P1); // Encrypt P1

      knownP0[i] = P0;
      knownP1[i] = P1;
      knownC0[i] = C0;
      knownC1[i] = C1;
    }
  }

  void findGoodPair(int diffOut, int nbPairs)
  {
    printf("\nSearching for good pair:\n");

    /* Question 4: Implement the code to find a good pair based on the differential characteristic */
    for (int i = 0; i < nbPairs; ++i) {
      if ((knownC0[i] ^ knownC1[i]) == diffOut) {
        goodP0 = knownP0[i];
        goodP1 = knownP1[i];
        goodC0 = knownC0[i];
        goodC1 = knownC1[i];
        printf("Good pair found: P0=%x, P1=%x, C0=%x, C1=%x\n", goodP0, goodP1, goodC0, goodC1);
        return;
      }
    }

    printf("No good pair found!\n");
  }

  int testKey(int testK0, int testK1, int nbPairs)
  {
    // TODO: Implement the function to test if the given key candidates are correct
    return 0;
  }

  void crack(int nbPairs)
  {
    printf("\nBrute forcing reduced keyspace:\n");

    // TODO: Implement the brute force key guessing based on the good pair
  }
};


//////////////////////////////////////////////////////////////////
//                             MAIN                             //
//////////////////////////////////////////////////////////////////

int main()
{
  srand(time(NULL));                                                      // Randomize values per run
  Cipher cipher;
  uint8_t message = rand() % 16;
  printf("Producing a random message : %x\n", message);
  uint8_t ciphertext = cipher.encrypt(message);
  printf("Encrypted message : %x\n", ciphertext);
  uint8_t plaintext = cipher.decrypt(ciphertext);
  printf("Decrypted message : %x\n", plaintext);

  if (message == plaintext) {
    printf(" --> Success\n");
  } else {
    printf(" --> Failure\n");
  }

  int nbPairs = 0; // Define number of known pairs
  uint8_t diffIn = 0;
  uint8_t diffOut = 0;

  Cryptanalysis analysis;
  analysis.findBestDiffs();
  analysis.genPairs(cipher, diffIn, nbPairs);
  analysis.findGoodPair(diffOut, nbPairs);
  analysis.crack(nbPairs);

  return 0;
}
