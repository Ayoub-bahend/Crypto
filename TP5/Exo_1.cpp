#include <iostream>
#include <iomanip>
#include <cstdint>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;
using namespace std::chrono;
typedef uint16_t aes_t;
#define N 16

aes_t table_inverse[65536];
// P16 = X^{16} + X^5 + X^3 + X^2 + 1
const aes_t PAES = (1 << 16) + (1 << 5) + (1 << 3) + (1 << 2) + 1;

// Fonction d'addition dans GF(2^16) (utilise l'opérateur XOR)
aes_t add(aes_t a, aes_t b)
{
    return a^b;
}

// Multiplie a par X dans GF(2^16)
aes_t mulbyX(aes_t a)
{
    aes_t ret;
    ret = a << 1;  // Décalage à gauche pour multiplier par X
    if (ret & (1 << N))  // Si le bit 17 est défini, appliquer la réduction modulo P16
        ret ^= PAES;
    return ret;
}

// Multiplication dans GF(2^16) modulo P16(X)
aes_t multiply(aes_t a, aes_t b) {
    uint32_t produit = 0;
    uint32_t Pa = a;
    uint32_t Pb = b;
    // Multiplication polynomiale
    for (int i = 0; i < 16; ++i) {
        if ((Pb >> i) & 1) {
            produit ^= (Pa << i);  // Si le ième bit de b est 1, ajouter a décalé de i
        }
    }

    // Réduction modulo P16 (X^16 + X^5 + X^3 + X^2 + 1)
    for (int i = 30; i >= 16; --i) {
        if ((produit >> i) & 1) {
            // Soustraction de P16 décalé de (i - 16)
            produit ^= PAES << (i - 16);
        }
    }
    // Retourner les 16 bits de poids faible comme résultat
    return (aes_t)(produit & 0xFFFF);
}

// Initialisation de la table des inverses
void initializeInverseTable() {
    table_inverse[0] = 0;  // 0 mappe sur 0
    for (uint32_t i = 1; i < 65536; ++i) {
        aes_t a = (aes_t)i;
        aes_t inv = 1;  // Initialiser l'inverse
        uint32_t exp = 65535 - 1;  // Puisque a^(2^16 - 1) = 1, l'inverse est a^(2^16 - 2)
        aes_t base = a;
        while (exp) {
            if (exp & 1) {
                inv = multiply(inv, base);
            }
            base = multiply(base, base);
            exp >>= 1;
        }
        table_inverse[i] = inv;
    }
}

// SubBytes : remplace chaque octet par son inverse dans GF(2^16)
void SubBytes(aes_t state[3][4]) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            aes_t value = state[i][j];
            state[i][j] = table_inverse[value];  // Substitution avec l'inverse
        }
    }
}

// ShiftRow : décalage des lignes de l'état (décalage circulaire)
void ShiftRow(aes_t state[3][4]) {
    aes_t temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;

    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;
}

// InverseShiftRow : inverse du décalage des lignes de l'état
void InverseShiftRow(aes_t state[3][4]) {
    // Inverser le décalage de la 2ème ligne
    aes_t temp = state[1][3];
    state[1][3] = state[1][2];
    state[1][2] = state[1][1];
    state[1][1] = state[1][0];
    state[1][0] = temp;

    // Inverser le décalage de la 3ème ligne
    temp = state[2][2];
    state[2][2] = state[2][0];
    state[2][0] = temp;
    temp = state[2][3];
    state[2][3] = state[2][1];
    state[2][1] = temp;
}

// MixColumns : mélange des colonnes de l'état
void MixColumns(aes_t state[3][4]) {
    for (int j = 0; j < 4; ++j) { 
        aes_t a0 = state[0][j];
        aes_t a4 = state[1][j];
        aes_t a8 = state[2][j];

        aes_t new_a0 = multiply(a0, 0x0003) ^ multiply(a8, 0x0001);  // Multiplication par 0x03 et 0x01
        aes_t new_a4 = multiply(a0, 0x0001) ^ multiply(a4, 0x0003);  // Multiplication par 0x01 et 0x03
        aes_t new_a8 = multiply(a4, 0x0001) ^ multiply(a8, 0x0003);  // Multiplication par 0x01 et 0x03         

        state[0][j] = new_a0;
        state[1][j] = new_a4;
        state[2][j] = new_a8;
    }
}

// Inverse MixColumns : inverse du mélange des colonnes de l'état
void InvMixColumns(aes_t state[3][4]) {
    aes_t a0 = 0x36dc;
    aes_t a4 = 0xedaf;
    aes_t a8 = 0x5b65;
    for (int c = 0; c < 4; ++c) {
        aes_t s0 = state[0][c];
        aes_t s1 = state[1][c];
        aes_t s2 = state[2][c];
        aes_t p0 = multiply(s0, a0) ^ multiply(s1, a8) ^ multiply(s2, a4);  // Calcul de p0
        aes_t p1 = multiply(s0, a4) ^ multiply(s1, a0) ^ multiply(s2, a8);  // Calcul de p1
        aes_t p2 = multiply(s0, a8) ^ multiply(s1, a4) ^ multiply(s2, a0);  // Calcul de p2
        state[0][c] = p0;
        state[1][c] = p1;
        state[2][c] = p2;
    }
}

// Ajoute la clé de tour à l'état
void AddRoundKey(aes_t state[3][4], aes_t key[3][4]) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            state[i][j] ^= key[i][j];  // XOR entre l'état et la clé
        }
    }
}

// Génère Xi pour l'algorithme de clé de tour
std::vector<aes_t> Xi_mod_P16() {
    std::vector<aes_t> Xi(12);
    uint32_t x = 1;  // X^0 mod P16 = 1
    for (int i = 0; i < 12; ++i) {
        Xi[i] = (aes_t)(x & 0xFFFF);  // Ajouter X^i mod P16
        // Multiplier x par X modulo P16
        x <<= 1;
        if (x & 0x10000) {
            x ^= PAES;
        }
    }
    return Xi;
}

// SubByte : applique la substitution par l'inverse de la valeur dans GF(2^16)
aes_t SubByte(aes_t value) {
    return table_inverse[value];
}

// Mélange des tableaux a et b avec le paramètre u
void Mix(aes_t a[12], aes_t b[12], aes_t u) {
    static std::vector<aes_t> Xi = Xi_mod_P16();
    for (int i = 0; i < 12; ++i) {
        aes_t index = (i + 1) % 12;
        aes_t s = add(a[index], Xi[i]);  // s = a_{(i+1)%12} + Xi
        b[i] = add(SubByte(s), u);       // b_i = SubByte(s) + u
    }
}

// Fonction d'expansion de clé
void KeyExpansion(aes_t K[3][4], aes_t roundKeys[6][3][4]) {
    // Aplatir K en un tableau de 12 éléments
    aes_t K_flat[12];
    for (int i = 0; i < 12; ++i) {
        K_flat[i] = K[i / 4][i % 4];
    }
    // Constantes de tour u
    aes_t u[6] = {0x6582, 0xa536, 0x0b14, 0x3885, 0x41b6, 0x5f28};
    aes_t K_prev[12];
    aes_t K_next[12];
    // K0 = Mix(K, u[0])
    Mix(K_flat, K_prev, u[0]);
    // Copier K0 dans roundKeys[0]
    for (int i = 0; i < 12; ++i)
        roundKeys[0][i / 4][i % 4] = K_prev[i];
    // Générer K1 à K5
    for (int r = 1; r < 6; ++r) {
        Mix(K_prev, K_next, u[r]);
        // Copier K_next dans roundKeys[r]
        for (int i = 0; i < 12; ++i)
            roundKeys[r][i / 4][i % 4] = K_next[i];
        // Préparer pour le tour suivant
        std::copy(std::begin(K_next), std::end(K_next), std::begin(K_prev));
    }
}

// Fonction de chiffrement
void Encryption(aes_t state[3][4], aes_t roundKeys[6][3][4]) {
    AddRoundKey(state, roundKeys[0]);
    for (int round = 1; round <= 4; ++round) {
        SubBytes(state);
        ShiftRow(state);
        MixColumns(state);
        AddRoundKey(state, roundKeys[round]);
    }
    // Dernier tour (sans MixColumns)
    SubBytes(state);
    ShiftRow(state);
    AddRoundKey(state, roundKeys[5]);
}

// Fonction de déchiffrement
void Decryption(aes_t state[3][4], aes_t roundKeys[6][3][4]) {
    // Ajouter la clé de tour K5
    AddRoundKey(state, roundKeys[5]);
    // Dernier tour (sans InvMixColumns)
    InverseShiftRow(state);
    SubBytes(state);
    AddRoundKey(state, roundKeys[4]);
    // Tours 4 à 1
    for (int round = 4; round >= 1; --round) {
        InvMixColumns(state);
        InverseShiftRow(state);
        SubBytes(state);
        AddRoundKey(state, roundKeys[round - 1]);
    }
}

// Fonction auxiliaire pour analyser les chaînes hexadécimales et remplir l'état
void parseState(const std::string input_hex[3][4], aes_t state[3][4]) {
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 4; ++col) {
            std::stringstream ss;
            ss << std::hex << input_hex[row][col];
            ss >> state[row][col];
        }
    }
}

// Fonction auxiliaire pour afficher l'état
void printState(const aes_t state[3][4]) {
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 4; ++col) {
            cout << "{"
                      << std::hex << std::setw(4) << std::setfill('0') << state[row][col]
                      << "} ";
        }
        cout << std::endl;
    }
}

// Fonction pour mesurer le temps de déchiffrement
void MeasureDecryptionTime(aes_t roundKeys[6][3][4]) {
    cout << "\nMesure du temps de déchiffrement :\n";
    cout << "Blocs\tTemps (ms)\n";
    aes_t state[3][4] = {0}; // Initialiser avec zéro ou des données de test
    const int maxBlocks = 1000000;
    for (int numBlocks = 1; numBlocks <= maxBlocks; numBlocks *= 10) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numBlocks; ++i) {
            Decryption(state, roundKeys);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        cout << numBlocks << "\t" << elapsed.count() << std::endl;
    }
}

// Fonction pour mesurer le temps de chiffrement
void MeasureEncryptionTime(aes_t roundKeys[6][3][4]) {
    cout << "\nMesure du temps de chiffrement :\n";
    cout << "Blocs\tTemps (ms)\n";
    aes_t state[3][4] = {0}; // Initialiser avec zéro ou des données de test
    const int maxBlocks = 1000000;
    for (int numBlocks = 1; numBlocks <= maxBlocks; numBlocks *= 10) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numBlocks; ++i) {
            Encryption(state, roundKeys);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        cout << numBlocks << "\t" << elapsed.count() << std::endl;
    }
}

// Fonction pour afficher un séparateur
void printSeparator() {
    cout << "----------------------------------------" << std::endl;
}

// Function to convert a hex string to aes_t
aes_t hexToAesT(const std::string& hex) {
    std::stringstream ss;
    ss << std::hex << hex;
    aes_t value;
    ss >> value;
    return value;
}

// Function to decrypt using CFB mode
void decryptCFB(const std::vector<aes_t>& ciphertext, const aes_t iv[3][4], aes_t roundKeys[6][3][4], std::vector<aes_t>& plaintext) {
    aes_t state[3][4];
    std::copy(&iv[0][0], &iv[0][0] + 12, &state[0][0]);

    for (size_t i = 0; i < ciphertext.size(); i += 12) {
        // Encrypt the IV or previous ciphertext block
        aes_t encryptedState[3][4];
        std::copy(&state[0][0], &state[0][0] + 12, &encryptedState[0][0]);
        Encryption(encryptedState, roundKeys);

        // XOR the encrypted block with the ciphertext to get the plaintext
        for (size_t j = 0; j < 12; ++j) {
            plaintext.push_back(ciphertext[i + j] ^ encryptedState[j / 4][j % 4]);
        }

        // Update the state with the current ciphertext block
        std::copy(&ciphertext[i], &ciphertext[i] + 12, &state[0][0]);
    }
}



int main() {
    // Initialiser la table des inverses
    initializeInverseTable();
    
    // Analyser la clé fournie et la mettre dans K[3][4]
    std::string key_hex[3][4] = {
        {"6d39", "45e3", "01b9", "15a4"},
        {"a811", "d173", "d1fb", "66c2"},
        {"5de3", "b33d", "bd8d", "2b02"}};
    aes_t K[3][4];
    parseState(key_hex, K);
    
    // Expansion de la clé
    aes_t roundKeys[6][3][4];
    KeyExpansion(K, roundKeys);
    
    // Test de déchiffrement
    aes_t state_dec[3][4];
    printSeparator();
    cout << "Test de déchiffrement Mini-Rijndael" << std::endl;
    printSeparator();
    
    // Texte chiffré en entrée
    std::string ciphertext_hex[3][4] = {
        {"8c07", "101e", "f24f", "e20d"},
        {"391c", "d98e", "b0b7", "ed7f"},
        {"f74c", "7a37", "078f", "a1f6"}};
    // Texte clair attendu après déchiffrement
    std::string expected_plaintext_hex[3][4] = {
        {"b6e3", "60bd", "3519", "a1c9"},
        {"6184", "f9c4", "8c05", "ffaa"},
        {"45c9", "0a97", "1d02", "15eb"}};
    
    // Analyser le texte chiffré
    parseState(ciphertext_hex, state_dec);
    // Déchiffrer l'état
    Decryption(state_dec, roundKeys);
    
    // Afficher le résultat et comparer avec le texte clair attendu
    cout << "\nRésultat du test de déchiffrement :" << std::endl;
    bool success_dec = true;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 4; ++col) {
            aes_t value = state_dec[row][col];
            cout << "{"
                      << std::hex << std::setw(4) << std::setfill('0') << value
                      << "} ";
            // Comparer avec la valeur attendue
            std::stringstream ss;
            ss << std::hex << expected_plaintext_hex[row][col];
            aes_t expected_value;
            ss >> expected_value;
            if (value != expected_value) {
                success_dec = false;
            }
        }
        cout << std::endl;
    }
    
    if (success_dec) {
        cout << "\nLe résultat du déchiffrement correspond aux valeurs attendues !" << std::endl;
    } else {
        cout << "\nLe résultat du déchiffrement NE correspond PAS aux valeurs attendues." << std::endl;
    }
    
    printSeparator();
    cout << "Test de chiffrement Mini-Rijndael" << std::endl;
    printSeparator();
    
    // Test de chiffrement
    aes_t state_enc[3][4];
    // Texte clair en entrée
    std::string expected_ciphertext_hex[3][4] = {
        {"3a2b", "2807", "ee3a", "2c8e"},
        {"0060", "4389", "db92", "a064"},
        {"e4fa", "340c", "cce9", "1f3b"}};
    // Texte chiffré attendu après chiffrement
    std::string plaintext_hex[3][4] = {
        {"4e23", "fd5a", "25ce", "02c8"},
        {"d0d8", "2c9c", "b4de", "d3ed"},
        {"df51", "07b1", "fef6", "e2ac"}};
    
    // Analyser le texte clair
    parseState(plaintext_hex, state_enc);
    // Chiffrer l'état
    Encryption(state_enc, roundKeys);
    
    // Afficher le résultat et comparer avec le texte chiffré attendu
    cout << "\nRésultat du test de chiffrement :" << std::endl;
    bool success_enc = true;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 4; ++col) {
            aes_t value = state_enc[row][col];
            cout << "{"
                      << std::hex << std::setw(4) << std::setfill('0') << value
                      << "} ";
            // Comparer avec la valeur attendue
            std::stringstream ss;
            ss << std::hex << expected_ciphertext_hex[row][col];
            aes_t expected_value;
            ss >> expected_value;
            if (value != expected_value) {
                success_enc = false;
            }
        }
        cout << std::endl;
    }
    if (success_enc) {
        cout << "\nLe résultat du chiffrement correspond aux valeurs attendues !" << std::endl;
    } else {
        cout << "\nLe résultat du chiffrement NE correspond PAS aux valeurs attendues." << std::endl;
    }

    printSeparator();
    cout << "Test du déchiffrement en mode CFB du fichier secret.enc" << std::endl;
    printSeparator();
    // Clé pour le déchiffrement
    std::string key_hex1[3][4] = {
        {"6edc", "2fbe", "41b6", "e8ca"},
        {"1a10", "b710", "5509", "e711"},
        {"40f5", "8f3b", "79a7", "d1b6"}};
    aes_t K1[3][4];
    parseState(key_hex1, K1);

    // Expansion de la clé
    aes_t roundKeys1[6][3][4];
    KeyExpansion(K1, roundKeys1);
    // Lire le fichier secret.enc
    std::ifstream file("secret.enc", std::ios::binary);
    if (!file) {
        std::cerr << "Erreur lors de l'ouverture du fichier secret.enc" << std::endl;
        return 1;
    }

    // Lire l'IV (premier bloc)
    aes_t iv[3][4];
    for (int i = 0; i < 12; ++i) {
        uint16_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        iv[i / 4][i % 4] = value;
    }

    // Lire le reste du fichier comme ciphertext
    std::vector<aes_t> ciphertext;
    while (file) {
        uint16_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        if (file) {
            ciphertext.push_back(value);
        }
    }
    file.close();

    // Déchiffrer le ciphertext en mode CFB
    std::vector<aes_t> plaintext;
    decryptCFB(ciphertext, iv, roundKeys, plaintext);

    // Écrire le résultat du déchiffrement dans un nouveau fichier
    std::ofstream output_file("decrypted_output.txt");
    if (!output_file) {
        std::cerr << "Erreur lors de la création du fichier decrypted_output.txt" << std::endl;
        return 1;
    }

    for (size_t i = 0; i < plaintext.size(); ++i) {
        output_file << "{"
                    << std::hex << std::setw(4) << std::setfill('0') << plaintext[i]
                    << "} ";
        if ((i + 1) % 4 == 0) {
            output_file << std::endl;
        }
    }
    output_file.close();

    std::cout << "Déchiffrement terminé. Résultat écrit dans decrypted_output.txt" << std::endl;

    printSeparator();
    cout << "Test de la fonction Mix de Mini-Rijndael" << std::endl;
    printSeparator();
    // Test de la fonction Mix
    aes_t a[12] = {0xd437, 0xe27c, 0xb551, 0x13d3, 0xab9f, 0x7670, 0xc7b5, 0x4016, 0x15ea, 0xa1fd, 0x40d2, 0x709e};
    aes_t u = 0x4242;
    aes_t b[12];
    aes_t expected_b[12] = {0xb8b2, 0x2cd4, 0x928c, 0x3f1a, 0x2408, 0xd122, 0x939f, 0x67bc, 0xd9dc, 0x826c, 0xc1a2, 0xdf6b};
    Mix(a, b, u);
    cout << "\nRésultat du test de la fonction Mix :" << std::endl;
    bool success_mix = true;
    for (int i = 0; i < 12; ++i) {
        cout << "{"
                  << std::hex << std::setw(4) << std::setfill('0') << b[i]
                  << "} ";
        if (b[i] != expected_b[i]) {
            success_mix = false;
        }
    }
    cout << std::endl;

    if (success_mix) {
        cout << "\nLe résultat de la fonction Mix correspond aux valeurs attendues !" << std::endl;
    } else {
        cout << "\nLe résultat de la fonction Mix NE correspond PAS aux valeurs attendues." << std::endl;
    }
    
    printSeparator();
    cout << "Mesure du temps de déchiffrement Mini-Rijndael" << std::endl;
    printSeparator();
    MeasureDecryptionTime(roundKeys);
    
    printSeparator();
    cout << "Mesure du temps de chiffrement Mini-Rijndael" << std::endl;
    printSeparator();
    MeasureEncryptionTime(roundKeys);

    printSeparator();
    return 0;
}