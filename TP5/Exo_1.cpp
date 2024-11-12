#include <iostream>
using namespace std;

typedef unsigned int aes_t;

const int N = 16;
const aes_t MAX = 0xFFFF; 
aes_t* table_mul_Y;
aes_t* table_mul_3;
aes_t* table_mul_5b65;
aes_t* table_mul_edaf;
aes_t* table_mul_36dc;
// P16 = X^{16} + X^5 + X^3 + X^2 + 1
const aes_t PAES = (1 << 16) + (1 << 5) + (1 << 3) + (1 << 2) + 1;
const aes_t C = (1 << 3)+ 1;
const aes_t MODULO = 0xFFFF; 


// Multiply a by X in K
aes_t mulbyX (aes_t a)
{
    aes_t ret;
    ret = a << 1;
    if (ret & (1 << N))
	    ret ^= PAES;
    return ret;
}


aes_t mul (aes_t a, aes_t b)
{
    aes_t ret = 0;
    aes_t s = a;
    int i;

    for (i = 0; i < N; i++) {
        if (b & (1 << i))
            ret ^= s;
        s = mulbyX (s);
    }
    return ret;
}

aes_t pow (aes_t a, int n)
{
    if (a == 0)
	    return 0;
    if (n == 0)
	    return 1;
    if (n & 1)
	    return mul (a, pow (mul (a, a), n / 2));
    else
	    return pow (mul (a, a), n / 2);
}

aes_t add(aes_t a,aes_t b)
{
    return a^b;
}

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

aes_t* Table_Correspondance(aes_t g) {
    aes_t* table = new aes_t[MAX + 1];
    for (aes_t i = 0; i < MAX-1; i++) {
        table[i] = pow(g, i);
    }
    table[MAX-1] = 0;
    return table;
}

aes_t* Table_Correspondance_inverse(aes_t* table_correspondance) {
    aes_t* table_inverse = new aes_t[MAX + 1];
    for (aes_t i = 0; i <= MAX; i++) {
        if (table_correspondance[i] == 0) {
            table_inverse[i] = 0; 
        } else {
            table_inverse[i] = pow(table_correspondance[i], MAX - 1); 
        }
    }
    return table_inverse;
}

aes_t* Table_Zech(aes_t* table_correspondance) {
    aes_t* table_zech = new aes_t[MAX + 1];

    for (aes_t i = 0; i <= MAX; i++) {
        aes_t sum = add(1, table_correspondance[i]);
        aes_t j = 0;
        while (j <= MAX && table_correspondance[j] != sum) {
            j++;
        }
        if (j <= MAX) {
            table_zech[i] = j;
        } else {
            table_zech[i] = 0; 
        }
    }

    return table_zech;
}


void SubBytes(aes_t state[3][4], aes_t * table_correspondance, aes_t * table_inverse) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            aes_t element = state[i][j];
            aes_t index = 0;
            while (index <= MAX && table_correspondance[index] != element) {
                index++;
            }

            if (index <= MAX) {
                state[i][j] = table_inverse[index];
            } else {
                state[i][j] = element; 
            }
        }
    }
}

void initializeTables() {
    table_mul_Y = new aes_t[65536];
    table_mul_3 = new aes_t[65536];

    for (aes_t i = 0; i < 65536; ++i) {
        aes_t result_Y = i << 1; 

        if (i & 0x8000) { 
            result_Y ^= PAES; 
        }

        table_mul_Y[i] = result_Y & MAX; 
        table_mul_3[i] = table_mul_Y[i] ^ i;
    }
}


void MixColumns(aes_t state[3][4]) {
    for (int j = 0; j < 4; ++j) { 
        aes_t a0 = state[0][j];
        aes_t a4 = state[1][j];
        aes_t a8 = state[2][j];

        aes_t new_a0 = table_mul_3[a0] ^ a8;           
        aes_t new_a4 = table_mul_3[a4] ^ a0;           
        aes_t new_a8 = table_mul_3[a8] ^ a4;           

        state[0][j] = new_a0;
        state[1][j] = new_a4;
        state[2][j] = new_a8;
    }
}
void InverseMixColumns(aes_t state[3][4]) {
    for (int j = 0; j < 4; ++j) { 
        aes_t a0 = state[0][j];
        aes_t a4 = state[1][j];
        aes_t a8 = state[2][j];

        aes_t new_a0 = table_mul_5b65[a0] ^ table_mul_edaf[a4] ^ table_mul_36dc[a8];
        aes_t new_a4 = table_mul_36dc[a0] ^ table_mul_5b65[a4] ^ table_mul_edaf[a8];
        aes_t new_a8 = table_mul_edaf[a0] ^ table_mul_36dc[a4] ^ table_mul_5b65[a8];

        state[2][j] = new_a0; 
        state[0][j] = new_a4; 
        state[1][j] = new_a8; 
    }
}


void initializeInverseTables() {
    table_mul_5b65 = new aes_t[65536];
    table_mul_edaf = new aes_t[65536];
    table_mul_36dc = new aes_t[65536];

    for (aes_t i = 0; i < 65536; ++i) {
        table_mul_5b65[i] = mul(i, 0x5b65);
        table_mul_edaf[i] = mul(i, 0xedaf);
        table_mul_36dc[i] = mul(i, 0x36dc);
    }
}

int main() {
    initializeTables();
    initializeInverseTables();

    aes_t state[3][4] = {
        {0x8c45, 0xc3c5, 0x906f, 0x4601},
        {0xcb2b, 0xf025, 0xaa8a, 0xfba2},
        {0x3c16, 0x8bcd, 0x7fd6, 0xf55b}
    };

    MixColumns(state);
    cout << "\nState After MixColumns:" << endl;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            cout << hex << state[i][j] << " ";
        }
        cout << endl;
    }

    InverseMixColumns(state);
    cout << "\nState After InverseMixColumns:" << endl;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            cout << hex << state[i][j] << " ";
        }
        cout << endl;
    }

    delete[] table_mul_Y;
    delete[] table_mul_3;
    delete[] table_mul_5b65;
    delete[] table_mul_edaf;
    delete[] table_mul_36dc;

    return 0;
}