#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

typedef unsigned int aes_t;

// Paramètres pour F_{2^{16}}
const int N = 16;
const aes_t PAES = (1 << 16) + (1 << 5) + (1 << 3) + (1 << 2) + 1;

// Fonction d'affichage pour l'état 3x4
void printState(const vector<vector<aes_t>>& state) {
    for (const auto& row : state) {
        for (auto val : row) {
            cout << hex << setw(4) << setfill('0') << val << " ";
        }
        cout << endl;
    }
    cout << dec;
}

// Multiplication par X dans F_{2^{16}}
aes_t mulbyX(aes_t a) {
    aes_t ret = a << 1;
    if (ret & (1 << N))
        ret ^= PAES;
    return ret & ((1 << N) - 1);
}

// Multiplication de deux éléments dans F_{2^{16}}
aes_t mul(aes_t a, aes_t b) {
    aes_t ret = 0;
    aes_t s = a;
    for (int i = 0; i < N; i++) {
        if (b & (1 << i))
            ret ^= s;
        s = mulbyX(s);
    }
    return ret;
}

// Exponentiation rapide pour F_{2^{16}}
aes_t pow(aes_t a, int n) {
    if (a == 0) return 0;
    if (n == 0) return 1;
    if (n & 1) return mul(a, pow(mul(a, a), n / 2));
    else return pow(mul(a, a), n / 2);
}

// Inverse dans F_{2^{16}}
aes_t inv(aes_t a) {
    return pow(a, (1 << N) - 2);
}

// Opération SubBytes (inversion dans F_{2^{16}})
void SubBytes(vector<vector<aes_t>>& state) {
    for (auto& row : state) {
        for (auto& val : row) {
            if (val != 0)
                val = inv(val);
        }
    }
}

// Opération ShiftRow
void ShiftRow(vector<vector<aes_t>>& state) {
    // Décalage de la deuxième ligne d'une position à gauche
    aes_t temp = state[1][0];
    for (int i = 0; i < 3; i++)
        state[1][i] = state[1][i + 1];
    state[1][3] = temp;

    // Décalage de la troisième ligne de deux positions à gauche
    aes_t temp1 = state[2][0];
    aes_t temp2 = state[2][1];
    state[2][0] = state[2][2];
    state[2][1] = state[2][3];
    state[2][2] = temp1;
    state[2][3] = temp2;
}

// Opération MixColumns
void MixColumns(vector<vector<aes_t>>& state) {
    for (int col = 0; col < 4; col++) {
        aes_t a0 = state[0][col];
        aes_t a1 = state[1][col];
        aes_t a2 = state[2][col];

        // Multiplier par G = Y + 3 modulo C = Y^3 + 1
        state[0][col] = mul(a0, 3) ^ a1;
        state[1][col] = mul(a1, 3) ^ a2;
        state[2][col] = mul(a2, 3) ^ a0;
    }
}

// Opération AddRoundKey
void AddRoundKey(vector<vector<aes_t>>& state, const vector<vector<aes_t>>& roundKey) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] ^= roundKey[i][j];
        }
    }
}

// Fonction principale de chiffrement mini-Rijndael
vector<vector<aes_t>> Encrypt(const vector<vector<aes_t>>& plaintext, const vector<vector<vector<aes_t>>>& roundKeys) {
    vector<vector<aes_t>> state = plaintext;

    // Étape initiale : AddRoundKey avec la première sous-clé K0
    AddRoundKey(state, roundKeys[0]);

    // Les 4 tours principaux
    for (int i = 1; i <= 4; i++) {
        SubBytes(state);
        ShiftRow(state);
        MixColumns(state);
        AddRoundKey(state, roundKeys[i]);
    }

    // Dernier tour sans MixColumns
    SubBytes(state);
    ShiftRow(state);
    AddRoundKey(state, roundKeys[5]);

    return state;
}

int main() {
    // Exemple d'état initial (plaintext) et de sous-clés
    vector<vector<aes_t>> plaintext = {
        {0x0001, 0x0203, 0x0405, 0x0607},
        {0x0809, 0x0a0b, 0x0c0d, 0x0e0f},
        {0x1011, 0x1213, 0x1415, 0x1617}
    };

    // Exemple de sous-clés (K0 à K5)
    vector<vector<vector<aes_t>>> roundKeys = {
        {{0x1111, 0x2222, 0x3333, 0x4444}, {0x5555, 0x6666, 0x7777, 0x8888}, {0x9999, 0xaaaa, 0xbbbb, 0xcccc}},
        {{0xdddd, 0xeeee, 0xffff, 0x0001}, {0x0202, 0x0303, 0x0404, 0x0505}, {0x0606, 0x0707, 0x0808, 0x0909}},
        {{0x0a0a, 0x0b0b, 0x0c0c, 0x0d0d}, {0x0e0e, 0x0f0f, 0x1010, 0x1111}, {0x1212, 0x1313, 0x1414, 0x1515}},
        {{0x1616, 0x1717, 0x1818, 0x1919}, {0x1a1a, 0x1b1b, 0x1c1c, 0x1d1d}, {0x1e1e, 0x1f1f, 0x2020, 0x2121}},
        {{0x2222, 0x2323, 0x2424, 0x2525}, {0x2626, 0x2727, 0x2828, 0x2929}, {0x2a2a, 0x2b2b, 0x2c2c, 0x2d2d}},
        {{0x2e2e, 0x2f2f, 0x3030, 0x3131}, {0x3232, 0x3333, 0x3434, 0x3535}, {0x3636, 0x3737, 0x3838, 0x3939}}
    };

    cout << "Plaintext:" << endl;
    printState(plaintext);

    // Chiffrement
    vector<vector<aes_t>> ciphertext = Encrypt(plaintext, roundKeys);

    cout << "Ciphertext:" << endl;
    printState(ciphertext);

    return 0;
}
