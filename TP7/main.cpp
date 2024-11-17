#include <stdio.h>
#include <iostream>
#include <gmp.h>
#include <vector>
#include <time.h>

// Define constants for bit strength and debugging
#define BITSTRENGTH 14
#define DEBUG true
#define MAXRAND 100000000000
#define MAXN 50

/**
 * @brief Compute a share (y_i) given polynomial coefficients
 * 
 * @param coeffs Polynomial coefficients
 * @param Y Array where shares are stored
 * @param S Secret value
 * @param p Prime modulus
 * @param x Index for share evaluation
 * @param k Threshold, minimum shares required
 */
void evaluateYi(mpz_t coeffs[], mpz_t Y[], mpz_t S, mpz_t p, int x, int k)
{
    mpz_t yi;
    mpz_init(yi);
    mpz_add(yi, yi, S);

    for (int j = 1; j < k; j++)
    {
        mpz_t X, temp;
        mpz_init(X);
        mpz_init(temp);
        mpz_set_ui(temp, x);
        mpz_pow_ui(X, temp, j);
        mpz_mul(X, X, coeffs[j]);
        mpz_add(yi, yi, X);
        mpz_mod(yi, yi, p);
        mpz_clear(X);
        mpz_clear(temp);
    }

    mpz_init(Y[x - 1]);
    mpz_set(Y[x - 1], yi);
    mpz_clear(yi);
}

void displayShares(mpz_t Y[], int n)
{
    std::cout << "Shares:\n";
    for (int x = 1; x <= n; x++)
    {
        char x_str[1000];
        sprintf(x_str, "%d", x);
        char y_str[1000];
        mpz_get_str(y_str, 10, Y[x - 1]);
        std::cout << "(x" << x << " = " << x_str << ", y" << x << " = " << y_str << ")\n";
    }
}

// Helper function to regenerate polynomial with a new threshold
void regeneratePolynomial(mpz_t coeffs[], int old_k, int new_k, mpz_t Y[], mpz_t p)
{
    if (new_k < old_k) {
        std::cout << "Using fewer parts than previous threshold. Select " << new_k << " shares to recreate polynomial.\n";
    }

    for (int i = 0; i < new_k; i++) {
        mpz_init(coeffs[i]);
    }

    mpz_set(coeffs[0], Y[0]); // Initialize constant term with original share

    for (int i = 1; i < new_k; i++) {
        mpz_set_ui(coeffs[i], rand() % MAXRAND);
        mpz_mod(coeffs[i], coeffs[i], p);
    }
}

int main()
{
    srand(time(NULL));
    int n = 4;
    int k = 3;

    mpz_t coeffs[k];
    mpz_t Y[MAXN];
    mpz_t alphas[k];
    mpz_t p, S, Sr, temp, neutre;
    mpz_init(p);
    mpz_init(S);
    mpz_init(neutre);
    mpz_set_str(neutre, "0", 10);
    gmp_randstate_t state; 
    gmp_randinit_default(state);
    int r = rand() % MAXRAND + 1;
    gmp_randseed_ui(state, r);

    // Prime generation and secret initialization
    mpz_urandomb(p, state, 100);
    mpz_nextprime(p, p);
    mpz_urandomb(S, state, 100);
    mpz_mod(S, S, p);

    // Polynomial coefficients setup
    mpz_init(coeffs[0]);
    mpz_add(coeffs[0], S, neutre);
    for (int i = 1; i < k; i++) {
        mpz_init(coeffs[i]);
        mpz_urandomb(coeffs[i], state, 100);
        mpz_mod(coeffs[i], coeffs[i], p);
    }

    // Generate shares
    for (int x = 1; x <= n; x++) {
        evaluateYi(coeffs, Y, S, p, x, k);
    }

    if (DEBUG) displayShares(Y, n);

    int choice;
    do {
        std::cout << "\nMenu:\n";
        std::cout << "1. Display polynomial\n";
        std::cout << "2. Increase number of shares\n";
        std::cout << "3. Change threshold\n";
        std::cout << "4. Quit\n";
        std::cin >> choice;

        switch (choice) {
        case 1:
            std::cout << "Polynomial P(X) = ";
            for (int i = 0; i < k; i++) {
                char coeff_str[1000];
                mpz_get_str(coeff_str, 10, coeffs[i]);
                std::cout << coeff_str << "X^" << i << " + ";
            }
            std::cout << "\n";
            displayShares(Y, n);
            break;
        case 2:
            std::cout << "Enter number of new shares: ";
            int extra_shares;
            std::cin >> extra_shares;
            for (int j = 1; j <= extra_shares; j++) {
                evaluateYi(coeffs, Y, S, p, n + j, k);
            }
            n += extra_shares;
            displayShares(Y, n);
            break;
        case 3:
            int new_k;
            std::cout << "Enter new threshold: ";
            std::cin >> new_k;
            if (new_k <= n) {
                regeneratePolynomial(coeffs, k, new_k, Y, p);
                k = new_k;
                std::cout << "Threshold updated. Polynomial regenerated.\n";
            } else {
                std::cout << "Threshold cannot exceed number of shares.\n";
            }
            break;
        case 4:
            break;
        default:
            std::cout << "Invalid choice.\n";
        }
    } while (choice != 4);

    // Cleanup
    for (int i = 0; i < k; i++) {
        mpz_clear(coeffs[i]);
        mpz_clear(alphas[i]);
        mpz_clear(Y[i]);
    }
    mpz_clear(Sr);
    mpz_clear(temp);
    mpz_clear(S);
    mpz_clear(p);
    mpz_clear(neutre);
    gmp_randclear(state);
}
