#include <iostream>
#include <string>
#include <cstdint> 
#include <cstdio> 
#include <cstdlib> 
#include <ctime> 
using namespace std;

// Définition globale de la boîte S et de son inverse
static const uint8_t S_BOX[16] = { 
 0x3, 0xE, 0x1, 0xA, 0x4, 0x9, 0x5, 0x6, 
 0x8, 0xB, 0xF, 0x2, 0xD, 0xC, 0x0, 0x7
};

static const uint8_t S_BOX_INV[16] = {
 0xE, 0x2, 0xB, 0x0, 0x4, 0x6, 0x7, 0xF, 
 0x8, 0x5, 0x3, 0x9, 0xD, 0xC, 0x1, 0xA
};

/* Classe pour le chiffrement et l'analyse cryptographique */
class Cipher
{
private:
    uint8_t k0; // Première sous-clé
    uint8_t k1; // Deuxième sous-clé

    // Fonction de substitution pour l'opération de ronde
    uint8_t roundFunction(uint8_t input)
    {
        return S_BOX[input];
    }

    // Fonction d'inversion de la substitution pour la décryption
    uint8_t inverseRoundFunction(uint8_t input)
    {
        return S_BOX_INV[input];
    }

public:
    // Constructeur par défaut : initialise les sous-clés aléatoirement
    Cipher()
    { 
        k0 = rand() % 16; // Génère une sous-clé aléatoire k0
        k1 = rand() % 16; // Génère une sous-clé aléatoire k1
        
        printf("Première sous-clé k0 = %x\n", k0);
        printf("Deuxième sous-clé k1 = %x\n\n", k1);
    }

    // Constructeur avec sous-clés spécifiées par l'utilisateur
    Cipher(uint8_t key0, uint8_t key1)
    { 
        k0 = key0;
        k1 = key1;
        
        printf("Première sous-clé k0 = %x\n", k0);
        printf("Deuxième sous-clé k1 = %x\n\n", k1);
    }

    // Fonction de chiffrement de l'entrée
    uint8_t encrypt(uint8_t input)
    {
        uint8_t temp = input ^ k0;       // Étape de mélange avec la sous-clé k0
        temp = roundFunction(temp);      // Applique la fonction de substitution
        temp = temp ^ k1;                // Mélange avec la sous-clé k1
        temp = roundFunction(temp);      // Applique la fonction de substitution finale
        return temp;
    }

    // Fonction de déchiffrement de l'entrée
    uint8_t decrypt(uint8_t input)
    {
        uint8_t temp = inverseRoundFunction(input); // Inversion de la dernière substitution
        temp = temp ^ k1;                           // Mélange inverse avec la sous-clé k1
        temp = inverseRoundFunction(temp);          // Inversion de la première substitution
        temp = temp ^ k0;                           // Mélange inverse avec la sous-clé k0
        return temp; 
    }

    // Accesseurs pour obtenir les sous-clés (utiles pour la cryptanalyse)
    uint8_t getKey0() const { return k0; }
    uint8_t getKey1() const { return k1; }
};

// Structures pour représenter les paires de texte clair et chiffré
struct GoodPair {
    uint8_t P0; // Premier texte clair de la bonne paire
    uint8_t P1; // Deuxième texte clair de la bonne paire
    uint8_t C0; // Premier texte chiffré de la bonne paire
    uint8_t C1; // Deuxième texte chiffré de la bonne paire
};

struct BadPair {
    uint8_t P0; // Premier texte clair de la mauvaise paire
    uint8_t P1; // Deuxième texte clair de la mauvaise paire
    uint8_t C0; // Premier texte chiffré de la mauvaise paire
    uint8_t C1; // Deuxième texte chiffré de la mauvaise paire
};

class Cryptanalysis
{
private:
    // Tableaux pour stocker les textes clairs et chiffrés connus
    uint8_t knownP0[1000]; // Texte clair initial de chaque paire
    uint8_t knownP1[1000]; // Texte clair modifié de chaque paire
    uint8_t knownC0[1000]; // Texte chiffré correspondant à P0
    uint8_t knownC1[1000]; // Texte chiffré correspondant à P1

    // Structures pour gérer les bonnes et mauvaises paires
    GoodPair goodPair;           // Stocke la première bonne paire trouvée
    BadPair badPairs[999];        // Stocke les paires restantes
    int badPairCount;             // Compteur de mauvaises paires
    bool goodPairFound;           // Indique si une bonne paire a été trouvée

    int chardatmax;               // Maximum d'occurrences pour un différentiel
    int chardat0[2];              // Tableau de taille 2 pour stocker ΔX et ΔY

public:
    // Constructeur par défaut pour initialiser les valeurs de base
    Cryptanalysis(){
        chardatmax = 0;
        chardat0[0] = 0;
        chardat0[1] = 0;
        badPairCount = 0;
        goodPairFound = false;
    }

    /* Construction de la Table de Distribution Différentielle de la S-box */
    void findBestDiffs(void){
        uint8_t i, j;
        uint8_t X, Xp, Y, Yp, DX, DY; 
        uint8_t T[16][16]; // Table pour les différences ΔX et ΔY

        // Initialisation de la table de distribution à zéro
        for (i = 0; i < 16; ++i){
            for (j = 0; j < 16; ++j){
                T[i][j] = 0;
            }
        }

        printf("\nCréation de la table de distribution différentielle (XOR):\n");

        // Remplissage de la table de distribution en comptant les occurrences des différentiels
        for (X = 0; X < 16; ++X) {
            for (Xp = 0; Xp < 16; ++Xp) {
                DX = X ^ Xp;
                Y = S_BOX[X];
                Yp = S_BOX[Xp];
                DY = Y ^ Yp;
                T[DX][DY]++;
            }
        }

        // Affichage de la table de distribution
        for (i = 0; i < 16; ++i) {
            printf("[");
            for (j = 0; j < 16; ++j) {
                printf(" %u ", T[i][j]);
            }
            printf("]\n");
        }

        printf("\nAffichage des différentiels les plus probables (ΔX ≠ 0):\n");

        // Recherche du différentiel avec le maximum d'occurrences (ΔX ≠ 0)
        chardatmax = 0;
        chardat0[0] = 0;
        chardat0[1] = 0;
        for (i = 1; i < 16; ++i) { // ΔX commence à 1 pour éviter ΔX=0
            for (j = 0; j < 16; ++j) {
                if (T[i][j] > chardatmax) {
                    chardatmax = T[i][j];
                    chardat0[0] = i; // ΔX
                    chardat0[1] = j; // ΔY
                }
            }
        }
        printf("Différentiel le plus probable (ΔX ≠ 0): ΔX = %x, ΔY = %x avec %d occurrences\n", chardat0[0], chardat0[1], chardatmax);
    }

    // Génération des données de caractères intermédiaires en fonction d'un différentiel donné
    void genCharData(int diffIn, int diffOut)
    {
        printf("\nGénération des valeurs intermédiaires possibles basées sur le différentiel (%x --> %x):\n", diffIn, diffOut);

        // Cette fonction permet d'identifier les paires (X, X') respectant le différentiel donné
        // Elle peut être utilisée pour générer des paires conformes pour la récupération de clé
    }

    /* 
    Génère des paires jusqu'à trouver une bonne paire.
    Stocke les mauvaises paires générées avant la bonne paire.
    */
    void genPairsUntilGood(Cipher cipher, uint8_t diffIn, uint8_t diffOut)
    {
        printf("\nGénération de paires jusqu'à trouver une bonne paire avec ΔX = %x et ΔY = %x.\n", diffIn, diffOut);

        int count = 0;
        while (!goodPairFound && count < 1000) { // Limite à 1000 pour éviter une boucle infinie
            uint8_t P0 = rand() % 16;
            uint8_t P1 = P0 ^ diffIn;

            // Priorisation des paires avec un différentiel prometteur
            if (isPromisingPair(P0, P1, diffIn)) {
                uint8_t C0 = cipher.encrypt(P0);
                uint8_t C1 = cipher.encrypt(P1);

                knownP0[count] = P0;
                knownP1[count] = P1;
                knownC0[count] = C0;
                knownC1[count] = C1;

                uint8_t delta_c = C0 ^ C1;
                if (delta_c == diffOut) {
                    if (!goodPairFound) {
                        // Stockage de la première bonne paire trouvée
                        goodPair.P0 = P0;
                        goodPair.P1 = P1;
                        goodPair.C0 = C0;
                        goodPair.C1 = C1;
                        goodPairFound = true;
                        printf("Première bonne paire trouvée : P0 = %x, P1 = %x, C0 = %x, C1 = %x, ΔC = %x\n", 
                            goodPair.P0, goodPair.P1, goodPair.C0, goodPair.C1, delta_c);
                    }
                } else {
                    // Stockage des mauvaises paires
                    badPairs[badPairCount].P0 = P0;
                    badPairs[badPairCount].P1 = P1;
                    badPairs[badPairCount].C0 = C0;
                    badPairs[badPairCount].C1 = C1;
                    badPairCount++;
                    printf("Mauvaise paire %d: P0 = %x, P1 = %x, C0 = %x, C1 = %x\n", badPairCount, P0, P1, C0, C1);
                }
            }
            count++;
        }

        if (goodPairFound) {
            printf("Une bonne paire a été trouvée après %d itérations.\n", count);
        } else {
            printf("Aucune bonne paire trouvée après %d itérations.\n", count);
        }
    }

    // Fonction pour vérifier si une paire est prometteuse en fonction des différentiels
    bool isPromisingPair(uint8_t P0, uint8_t P1, uint8_t diffIn)
    {
        return true; 
    }

    /* Méthode pour récupérer la clé en utilisant la bonne paire et la valider avec les mauvaises paires */
    void recoverKeyFromGoodPair()
    {
        if(!goodPairFound){
            printf("Aucune bonne paire disponible pour récupérer la clé.\n");
            return;
        }

        printf("\nRécupération de la clé à partir de la bonne paire:\n");

        // Structure pour stocker les clés candidates
        struct KeyCandidate {
            uint8_t k0;
            uint8_t k1;
        };
        KeyCandidate candidates[256]; // 16 k0 * 16 k1 combinaisons possibles
        int candidateCount = 0;

        // Utilisation de la bonne paire pour générer des clés candidates
        for(uint8_t testK0 = 0; testK0 < 16; ++testK0){
            for(uint8_t testK1 = 0; testK1 < 16; ++testK1){
                uint8_t computedC0 = S_BOX[S_BOX[goodPair.P0 ^ testK0] ^ testK1];
                uint8_t computedC1 = S_BOX[S_BOX[goodPair.P1 ^ testK0] ^ testK1];
                uint8_t deltaC = computedC0 ^ computedC1;
                if(deltaC == (goodPair.C0 ^ goodPair.C1)){
                    candidates[candidateCount].k0 = testK0;
                    candidates[candidateCount].k1 = testK1;
                    candidateCount++;
                    printf("Candidat clé trouvé (à partir de la bonne paire): k0 = %x, k1 = %x\n", testK0, testK1);
                }
            }
        }

        if(candidateCount == 0){
            printf("Aucun candidat clé trouvé à partir de la bonne paire.\n");
            return;
        }

        // Affichage des clés candidates trouvées
        printf("\nClés candidates trouvées à partir de la bonne paire (%d):\n", candidateCount);
        for(int c = 0; c < candidateCount; ++c){
            printf("Clé %d: k0 = %x, k1 = %x\n", c + 1, candidates[c].k0, candidates[c].k1);
        }

        // Validation des candidats avec les mauvaises paires
        printf("\nValidation des clés candidates avec les mauvaises paires (%d):\n", badPairCount);
        bool keyFound = false;
        for(int c = 0; c < candidateCount; ++c){
            uint8_t testK0 = candidates[c].k0;
            uint8_t testK1 = candidates[c].k1;
            bool valid = true;

            for(int p = 0; p < badPairCount; ++p){
                uint8_t computedC0 = S_BOX[S_BOX[badPairs[p].P0 ^ testK0] ^ testK1];
                uint8_t computedC1 = S_BOX[S_BOX[badPairs[p].P1 ^ testK0] ^ testK1];
                uint8_t deltaC = computedC0 ^ computedC1;
                if(deltaC == chardat0[1]){ // Le différentiel pour les mauvaises paires doit être différent
                    valid = false;
                    break;
                }
            }

            if(valid){
                printf("Clé confirmée: k0 = %x, k1 = %x\n", testK0, testK1);
                keyFound = true;
                break;
            }
        }

        if(!keyFound){
            printf("Aucune clé confirmée après validation avec les mauvaises paires.\n");
        }
    }

    // Lance le processus de récupération de clé
    void crack()
    {
        printf("\nRécupération de la clé:\n");
        recoverKeyFromGoodPair();
    }

    // Accesseurs pour chardat0 (ΔX et ΔY les plus probables)
    uint8_t getDiffIn() const { return chardat0[0]; }
    uint8_t getDiffOut() const { return chardat0[1]; }

    // Accesseur public pour `goodPairFound`
    bool hasGoodPair() const { return goodPairFound; }
};


int main() {
    // Initialisation de la graine aléatoire pour des valeurs différentes à chaque exécution
    srand(time(NULL));

    // Définir une clé spécifique pour tester la cryptanalyse
    uint8_t key0 = 0x5; // Exemple de clé pour k0 (0101 en binaire)
    uint8_t key1 = 0x7; // Exemple de clé pour k1 (0111 en binaire)
    Cipher cipher(key0, key1);

    // Générer un message aléatoire pour tester le chiffrement et le déchiffrement
    uint8_t message = rand() % 16;
    printf("Message aléatoire : %x\n", message);

    // Effectuer le chiffrement du message aléatoire
    uint8_t ciphertext = cipher.encrypt(message);
    printf("Message chiffré : %x\n", ciphertext);

    // Déchiffrer le message chiffré
    uint8_t decrypted = cipher.decrypt(ciphertext);
    printf("Message déchiffré : %x\n", decrypted);

    // Vérifier si le déchiffrement est correct
    if (message == decrypted) {
        printf(" --> Succès : Le message a été correctement déchiffré.\n");
    } else {
        printf(" --> Échec : Le message n'a pas été correctement déchiffré.\n");
    }

    // Initialisation de l'instance de cryptanalyse
    Cryptanalysis cryptanalysis;

    // Étape 1 : Trouver les différentiels les plus probables (ΔX ≠ 0)
    cryptanalysis.findBestDiffs();

    // Étape 2 : Définir les différentiels d'entrée et de sortie pour l'attaque
    uint8_t diffIn = cryptanalysis.getDiffIn();   // ΔX
    uint8_t diffOut = cryptanalysis.getDiffOut(); // ΔY
    printf("\nDifférentiel choisi pour l'attaque: ΔX = %x, ΔY = %x\n", diffIn, diffOut);

    // Étape 3 : Générer des paires de texte clair jusqu'à trouver une bonne paire
    cryptanalysis.genPairsUntilGood(cipher, diffIn, diffOut);

    // Vérifier si une bonne paire a été trouvée
    if (!cryptanalysis.hasGoodPair()) {
        printf("Attaque échouée: aucune bonne paire trouvée.\n");
        return 1; // Arrêt du programme en cas d'échec
    }

    // Étape 4 : Effectuer la cryptanalyse pour récupérer la clé
    cryptanalysis.crack();

    return 0;
}