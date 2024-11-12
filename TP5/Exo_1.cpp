#include <iostream>
#include <vector>
#include <iomanip>
#include <cassert>

using namespace std;

// Représentation des éléments de F_{2^{16}} comme des entiers de 32 bits
typedef unsigned int aes_t;

const int N = 16;
const int FIELD_SIZE = (1 << N) - 1;
const aes_t P16 = (1 << 16) | (1 << 5) | (1 << 3) | (1 << 2) | 1; // Polynôme irréductible P16

// 1. Addition de deux éléments dans F_{2^{16}}.
aes_t add(aes_t a, aes_t b) {
    return a ^ b;  // XOR bit-à-bit
}

// 2. Multiplication par X dans F_{2^{16}}.
aes_t mulByX(aes_t a) {
    a <<= 1;  // Décalage à gauche de 1 bit (équivalent à multiplication par X)
    if (a & (1 << N)) {  // Si le bit de poids 16 est 1, réduction modulo P16
        a ^= P16;
    }
    return a;
}

// 3. Multiplication de deux éléments dans F_{2^{16}}.
// Fonction de multiplication dans F_{2^8} (modulo x^8 + x^4 + x^3 + x + 1)
aes_t mul(aes_t a, aes_t b) {
    aes_t p = 0;
    aes_t high_bit_mask = 0x80;

    for (int i = 0; i < 8; i++) {
        if (b & 1) {
            p ^= a;  // Ajout de a au produit si le bit b[i] est 1
        }
        bool high_bit_set = a & high_bit_mask;
        a <<= 1;  // Décalage de a vers la gauche (multiplication par 2)
        if (high_bit_set) {
            a ^= 0x11B;  // Réduction modulo x^8 + x^4 + x^3 + x + 1 si le bit haut était 1
        }
        b >>= 1;  // Décalage de b vers la droite
    }
    return p;
}


// 4. Exponentiation rapide pour calculer l'inverse.
aes_t pow(aes_t a, int n) {
    aes_t result = 1;
    aes_t base = a;
    while (n > 0) {
        if (n & 1) {
            result = mul(result, base);  // Multiplier par base si le bit est 1
        }
        base = mul(base, base);  // Exponentier base au carré
        n >>= 1;  // Décalage de n vers la droite
    }
    return result;
}

// 5. Calcul de l'inverse.
aes_t inverse(aes_t a) {
    return pow(a, (1 << 16) - 2);  // Utilisation de l'exposant 2^16 - 2
}

// 1.2. Tables de puissances et logarithmes
vector<aes_t> power_table(FIELD_SIZE);
vector<int> log_table(FIELD_SIZE + 1, -1);  // +1 pour l'élément 0
vector<aes_t> inverse_table(FIELD_SIZE);
vector<int> zech_table(FIELD_SIZE);

// 6. Construction des tables
void buildTables() {
    aes_t value = 1;
    for (int i = 0; i < FIELD_SIZE; i++) {
        power_table[i] = value;
        log_table[value] = i;
        value = mulByX(value);  // Multiplier par le générateur G (ici X)
    }
    log_table[0] = FIELD_SIZE;  // Convention : log(0) = 2^16 - 1

    for (int i = 1; i < FIELD_SIZE; i++) {
        inverse_table[i] = inverse(power_table[i]);  // Inverse de chaque élément
    }

    for (int i = 0; i < FIELD_SIZE; i++) {
        aes_t gi = power_table[i];    // G^i
        aes_t target = gi ^ 1;        // 1 + G^i
        if (target == 0) {
            zech_table[i] = FIELD_SIZE;  // Convention pour l'infini
        } else {
            zech_table[i] = log_table[target];  // Trouver j tel que G^j = 1 + G^i
        }
    }
}

// Fonction de test pour vérifier les opérations de base
void testOperations() {
    aes_t a = 0xa3a6;
    aes_t b = 0x3df5;

    cout << "Addition de a et b dans F_{2^{16}} : " << hex << add(a, b) << endl;
    cout << "Multiplication de a par X dans F_{2^{16}} : " << hex << mulByX(a) << endl;
    cout << "Multiplication de a et b dans F_{2^{16}} : " << hex << mul(a, b) << endl;
    cout << "Inverse de a dans F_{2^{16}} : " << hex << inverse(a) << endl;

    cout << "Table de puissances (extrait) : ";
    for (int i = 0; i < 10; ++i) cout << hex << power_table[i] << " ";
    cout << endl;

    cout << "Table des logarithmes (extrait) : ";
    for (int i = 1; i <= 10; ++i) cout << dec << log_table[i] << " ";
    cout << endl;

    cout << "Table de Zech (extrait) : ";
    for (int i = 0; i < 10; ++i) cout << dec << zech_table[i] << " ";
    cout << endl;
}

// S-Box de substitution (assurez-vous que la S-Box complète est incluse ici)
aes_t SubByte(aes_t byte) {
    static const aes_t S_BOX[256] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa8, 0x51, 0xa3,
        0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c,
        0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, 0x60, 0x81,
        0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0x2f, 0x03, 0x01,
        0x75, 0xdf, 0x10, 0x10, 0x76, 0x79, 0x9f, 0x57, 0x55, 0x5b, 0x12, 0x88, 0x56, 0x48, 0x31, 0x3a,
        0x0e, 0x28, 0x38, 0x4d, 0x73, 0x0a, 0x0f, 0x78, 0x54, 0x5b, 0x65, 0x99, 0x1f, 0x76, 0xf6, 0xe7
    };

    return S_BOX[byte];  // Retourne la substitution correspondante dans la S-Box
}

// 2. ShiftRows : Décalage circulaire des lignes de l'état
void ShiftRows(vector<aes_t>& state) {
    aes_t temp;

    // Décalage de la 2e ligne (index 1)
    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;

    // Décalage de la 3e ligne (index 2)
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;

    // Décalage de la 4e ligne (index 3)
    temp = state[3];
    state[3] = state[15];
    state[15] = state[7];
    state[7] = state[11];
    state[11] = temp;
}


// 3. MixColumns : Multiplication des colonnes par une matrice fixe
void MixColumns(vector<aes_t>& state) {
    aes_t temp[4];
    for (int i = 0; i < 4; i++) {
        temp[i] = state[i];  // Sauvegarde des éléments d'origine
    }

    // Affichage des colonnes avant MixColumns
    cout << "Colonnes avant MixColumns : ";
    for (int i = 0; i < 4; i++) {
        cout << hex << setw(2) << setfill('0') << temp[i] << " ";
    }
    cout << endl;

    // Calcul pour chaque colonne
    for (int i = 0; i < 4; i++) {
        state[i]     = mul(temp[0], 2) ^ mul(temp[1], 3) ^ temp[2] ^ temp[3];
        state[i+4]   = temp[0] ^ mul(temp[1], 2) ^ mul(temp[2], 3) ^ temp[3];
        state[i+8]   = temp[0] ^ temp[1] ^ mul(temp[2], 2) ^ mul(temp[3], 3);
        state[i+12]  = mul(temp[0], 3) ^ temp[1] ^ temp[2] ^ mul(temp[3], 2);
    }

    // Affichage des colonnes après MixColumns
    cout << "Colonnes après MixColumns : ";
    for (int i = 0; i < 4; i++) {
        cout << hex << setw(2) << setfill('0') << state[i] << " ";
    }
    cout << endl;
}



// Affichage de l'état
void printState(const vector<aes_t>& state) {
    for (int i = 0; i < 16; i++) {
        cout << hex << setw(2) << setfill('0') << state[i] << " ";
        if ((i + 1) % 4 == 0) cout << endl;
    }
}

// Affichage détaillé de la substitution
void SubBytesDebug(vector<aes_t>& state) {
    cout << "État avant SubBytes : ";
    for (int i = 0; i < 16; i++) {
        cout << hex << setw(2) << setfill('0') << state[i] << " ";
    }
    cout << endl;

    for (int i = 0; i < 16; i++) {
        aes_t original = state[i];
        state[i] = SubByte(state[i]);
        cout << "SubByte(" << hex << +original << ") = " << hex << +state[i] << endl;
    }

    cout << "État après SubBytes : ";
    for (int i = 0; i < 16; i++) {
        cout << hex << setw(2) << setfill('0') << state[i] << " ";
    }
    cout << endl;
}


// Fonction principale pour tester les opérations et transformations AES
int main() {
    buildTables();
    testOperations();

    vector<aes_t> state = {
        0x32, 0x88, 0x31, 0xE0, 0x43, 0x5A, 0x31, 0x37, 0xF6, 0x30, 0x98, 0x07, 0xA8, 0x8D, 0xA2, 0x34
    };

    cout << "État initial : " << endl;
    printState(state);

    SubBytesDebug(state);  // Substitution des bytes
    ShiftRows(state);      // Décalage des lignes
    MixColumns(state);     // Mélange des colonnes

    cout << "État après transformations : " << endl;
    printState(state);

    return 0;
}
