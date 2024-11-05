#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cassert>

using namespace std;

typedef unsigned int aes_t;

const int N = 16;

// P16 = X^{16} + X^5 + X^3 + X^2 + 1
const int PAES = (1 << 16) + (1 << 5) + (1 << 3) + (1 << 2) + 1;

// Multiply a by X in K
aes_t mulbyX (aes_t a)
{
    aes_t ret;
    ret = a << 1;
    if (ret & (1 << N))
	ret ^= PAES;
    return ret;
}

// Multiply two elements of K.
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


int main (int argc, char **argv)
{

    aes_t a = 0x2;

    cout << "a: " << a << endl;
    cout << "a*X: " << mulbyX(a) << endl;
    cout << "a*a: " << mul(a,a) << endl;
    cout << "a^2: " << pow(a,2) << endl;

    return 0;
}
