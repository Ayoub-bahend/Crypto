//
//  TP7_Shamir's Secret Sharing Scheme
//  

#include <stdio.h>
#include <iostream>
#include <gmp.h>
#include <cmath>
#include "main.hpp"

#define BITSTRENGTH  14              /* size of prime number (p) in bits */
#define DEBUG true

gmp_randstate_t state;

void generatePrime(mpz_t &p) {
    mpz_init(p);
    mpz_t p_tmp; mpz_init(p_tmp);
    mpz_urandomb(p_tmp, state, BITSTRENGTH);
    mpz_nextprime(p, p_tmp);
}

void generateRandom(mpz_t &s, mpz_t p) {
    mpz_init(s);
    mpz_t s_tmp; mpz_init(s_tmp);
    mpz_urandomb(s_tmp, state, BITSTRENGTH);
    mpz_mod(s, s_tmp, p);
}

void generateCoefs(mpz_t *tab, mpz_t p, int k) {
    init_tab_mpz(tab, k);
    mpz_t temp; mpz_init(temp);
    for(int i = 0; i < k; i++){
        do {generateRandom(temp, p);} while(check_coefs_different(tab,i,temp));
        mpz_set(tab[i], temp);
    }
}

bool check_coefs_different(mpz_t *tab, int taille, mpz_t e) {
    bool res = false;
    for(int i=0; i < taille; ++i) {
        if(mpz_cmp(tab[i], e) == 0) res = true;
    }
    return res;
}


void compute_image(mpz_t &image, mpz_t *a, mpz_t x, int k, mpz_t s) {
    mpz_set(image, s);
    mpz_t tmp; mpz_init(tmp);
    for(int j=0; j < k; j++) {
        mpz_pow_ui(tmp, x, j+1);
        mpz_mul(tmp, a[j], tmp);
        mpz_add(image, image, tmp);
    }
}

void computeShares(mpz_t *x, mpz_t *y, mpz_t *a, mpz_t s, int k, int n) {
    init_tab_mpz(x, n);
    init_tab_mpz(y, n);
    for(int i = 0; i < n; ++i) {
        mpz_t image; mpz_init(image);
        mpz_set_ui(x[i], i+1);
        compute_image(image, a, x[i], k, s);
        mpz_set(y[i], image);
    }
}


void init_tab_mpz(mpz_t * tab, int t) {
    for(int i = 0; i < t; ++i) {
        mpz_init(tab[i]);
    }
}

void clear_tab_mpz(mpz_t * tab, int t) {
    for(int i = 0; i < t; ++i) {
        mpz_clear(tab[i]);
    }
}

void computeLagrange(mpz_t *alpha, mpz_t * x, int k, int n, mpz_t p) {
    init_tab_mpz(alpha, k);
    mpz_t prod; mpz_init(prod);
    mpz_t tmp; mpz_init(tmp);
    for(int i = 0; i < k; i++) {
        mpz_set_str(prod, "1", 0);
        for(int j = 0; j < k; j++) {
            if(i != j) {
                mpz_sub(tmp, x[j], x[i]);
                mpz_invert(tmp, tmp, p);
                mpz_mul(tmp, x[j], tmp);
                mpz_mul(prod, prod, tmp);
            }
        }
        mpz_set(alpha[i], prod);
    }
}

void reconstructSecret(mpz_t * alpha, mpz_t *y, mpz_t Sr, mpz_t p, int start, int k) {
    mpz_init(Sr); mpz_init_set_str(Sr, "0", 0);
    mpz_t temp; mpz_init(temp);
    for(int i = start; i < k; i++) {
        mpz_mul(temp,alpha[i],y[i]);
        mpz_add(Sr, Sr, temp);
    }
    mpz_mod(Sr, Sr, p );
}

void copyTable(mpz_t * a, mpz_t * b, int n) {
    for(int i = 0; i < n; i++) {
        mpz_set(b[i], a[i]);
    }
}

/* Main subroutine */
int main()
{
    /* Declare variables */
    int n = 4;          // Numbers of users (max)
    int k = 3;          // Threshold : minimal number of users => secret

    mpz_t p;            // Prime number
    mpz_t S;            // Secret
    mpz_t Sr;           // Reconstruction of the Secret

    mpz_t  a[k-1];       // Coefficients of polynom
    mpz_t alpha[k];  // Lagrangian polynomials in zero

    mpz_t x[n];  // Login users
    mpz_t y[n];  // Shares of users
    //initialize seed
    gmp_randinit_mt(state); gmp_randseed_ui(state, time(NULL));

    /* This function creates the shares computation. The basic algorithm is...
    *
    *  1. Initialize Prime Number : we work into Z/pZ
    *  2. Initialize Secret Number : S
    *  3. Compute a random polynom of order k-1
    *  4. Shares computation for each users (xi, yi) for i in [1,n]
    *  5. Reconstruct the secret with k users or more
    *
    */
    
    /*
     *  Step 1: Initialize Prime Number : we work into Z/pZ
     */
    generatePrime(p);
    
    if (DEBUG)
    {
        char p_str[1000]; mpz_get_str(p_str,10,p);
        std::cout << "Random Prime 'p' = " << p_str <<  std::endl;
    }

    /*
     *  Step 2: Initialize Secret Number
     */
    generateRandom(S, p);
    
    if (DEBUG)
    {
        char S_str[1000]; mpz_get_str(S_str,10,S);
        std::cout << "Secret number 'S' = " << S_str <<  std::endl;
    }

    /*
     *  Step 3: Initialize Coefficient of polynom
     */
    generateCoefs(a, p, k-1);
    
    if (DEBUG)
    {
        char a1_str[1000]; mpz_get_str(a1_str,10,a[0]);
        char a2_str[1000]; mpz_get_str(a2_str,10,a[1]);
        char S_str[1000];  mpz_get_str(S_str,10,S);
        std::cout << "Polynom 'P(X)' = " << a2_str << "X^2 + " << a1_str << "X + " << S_str << std::endl;
    }
    
    /*
     *  Step 4: Shares computation for each users (xi, yi)
     */
    computeShares(x, y, a, S, k-1, n);

    
    if (DEBUG)
    {
        char x1_str[1000]; mpz_get_str(x1_str,10,x[0]);
        char x2_str[1000]; mpz_get_str(x2_str,10,x[1]);
        char x3_str[1000]; mpz_get_str(x3_str,10,x[2]);
        char x4_str[1000]; mpz_get_str(x4_str,10,x[3]);

        char y1_str[1000]; mpz_get_str(y1_str,10,y[0]);
        char y2_str[1000]; mpz_get_str(y2_str,10,y[1]);
        char y3_str[1000]; mpz_get_str(y3_str,10,y[2]);
        char y4_str[1000]; mpz_get_str(y4_str,10,y[3]);
        
        std::cout << "Login and share of each users : " << "( x1="<< x1_str << " ; y1=" << y1_str << " ) , "  << "( x2="<< x2_str << " ; y2=" << y2_str << " ) , "  << "( x3="<< x3_str << " ; y3=" << y3_str << " ) , "  << "( x4="<< x4_str << " , y4=" << y4_str << " )" << std::endl;
    }

    /*
     *  Step 5: Sample for reconstruct the secret with 3 users (x1, x2, x3)
     */
    
    computeLagrange(alpha, x, k, n, p);
    // Compute Secret = sum_{i=1}^{k} alpha_i x y_i

    reconstructSecret(alpha, y, Sr, p, 0, k);
    if (DEBUG)
    {
        char Sr_str[1000]; mpz_get_str(Sr_str,10,Sr);
        std::cout << "Reconstruction of the secret : S = " << Sr_str << std::endl;
    }
    
    //Compute another share
    mpz_t x_n[n+1];  // Login users
    mpz_t y_n[n+1];  // Shares of users
    std::cout << "************* Adding another share *************" << std::endl;
    computeShares(x_n, y_n, a, S, k, n+1);

    if (DEBUG)
    {
        char x1_str[1000]; mpz_get_str(x1_str,10,x_n[0]);
        char x2_str[1000]; mpz_get_str(x2_str,10,x_n[1]);
        char x3_str[1000]; mpz_get_str(x3_str,10,x_n[2]);
        char x4_str[1000]; mpz_get_str(x4_str,10,x_n[3]);
        char x5_str[1000]; mpz_get_str(x5_str,10,x_n[4]);

        char y1_str[1000]; mpz_get_str(y1_str,10,y_n[0]);
        char y2_str[1000]; mpz_get_str(y2_str,10,y_n[1]);
        char y3_str[1000]; mpz_get_str(y3_str,10,y_n[2]);
        char y4_str[1000]; mpz_get_str(y4_str,10,y_n[3]);
        char y5_str[1000]; mpz_get_str(y5_str,10,y_n[4]);
        
        std::cout << "Login and share of each users : " << "( x1="<< x1_str << " ; y1=" << y1_str << " ) , "  << "( x2="<< x2_str << " ; y2=" << y2_str << " ) , "  << "( x3="<< x3_str << " ; y3=" << y3_str << " ) , "  << "( x4="<< x4_str << " , y4=" << y4_str << " ) , " << "( x5="<< x5_str << " , y5=" << y5_str << " )" << std::endl;
    }

    //Change threshold k
    std::cout << "************* Incrementing threshold *************" << std::endl;
    //mpz_t  a_n[k];       // Coefficients of polynom
    mpz_t alpha_n[k+1];  // Lagrangian polynomials in zero
    mpz_t Sr_n;

    computeLagrange(alpha_n, x_n, k+1, n+1, p);

    reconstructSecret(alpha_n, y_n, Sr_n, p, 1, k+1);

    if (DEBUG)
    {
        char Sr_str[1000]; mpz_get_str(Sr_str,10,Sr_n);
        std::cout << "Reconstruction of the secret(with the new threshold) : S = " << Sr_str << std::endl;
    }

    /* Clean up the GMP integers */

    clear_tab_mpz(x, n);
    clear_tab_mpz(y, n);
    clear_tab_mpz(alpha, k);
    clear_tab_mpz(a, k-1);
    mpz_clear(Sr);
    mpz_clear(S);
    mpz_clear(p);
}

