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

// 3. Multiplication de deux éléments dans F_{2^{16}}
aes_t mul(aes_t a, aes_t b) {
    aes_t p = 0;
    while (b) {
        if (b & 1) {
            p ^= a;
        }
        if (a & (1 << (N - 1))) {  // Vérification du bit de poids fort pour F_{2^{16}}
            a = (a << 1) ^ P16;    // Réduction modulo P16
        } else {
            a <<= 1;
        }
        b >>= 1;
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

// S-Box de substitution (complétez avec les 256 valeurs AES)
aes_t SubByte(aes_t byte) {
    static const aes_t S_BOX[256] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
    };
    return S_BOX[byte];
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
    state[10] = state[14];
    state[14] = state[6];
    state[6] = temp;

    // Décalage de la 4e ligne (index 3)
    temp = state[3];
    state[3] = state[15];
    state[15] = state[11];
    state[11] = state[7];
    state[7] = temp;
}

// 3. MixColumns : Multiplication des colonnes par une matrice fixe
void MixColumns(vector<aes_t>& state) {
    for (int i = 0; i < 4; i++) {
        aes_t a0 = state[i];
        aes_t a1 = state[i + 4];
        aes_t a2 = state[i + 8];
        aes_t a3 = state[i + 12];
        state[i]      = mul(a0, 2) ^ mul(a1, 3) ^ a2 ^ a3;
        state[i + 4]  = a0 ^ mul(a1, 2) ^ mul(a2, 3) ^ a3;
        state[i + 8]  = a0 ^ a1 ^ mul(a2, 2) ^ mul(a3, 3);
        state[i + 12] = mul(a0, 3) ^ a1 ^ a2 ^ mul(a3, 2);
    }
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

    cout << "État initial :" << endl;
    printState(state);

    SubBytesDebug(state);
    ShiftRows(state);
    cout << "État après ShiftRows :" << endl;
    printState(state);

    MixColumns(state);
    cout << "État après MixColumns :" << endl;
    printState(state);

    return 0;
}
