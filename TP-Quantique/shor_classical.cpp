#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <numeric>
#include "shor_utils.hpp"

using namespace std;

// Fonction pour trouver la période r classiquement
int find_period_classical(int a, int N) {
    int r = 1;
    int power = a % N;
    while (power != 1) {
        power = (power * a) % N;
        r++;
    }
    return r;
}

// Fonction principale de l'algorithme de Shor
vector<int> shor_classical(int N) {
    while (true) {
        int a = rand() % (N - 2) + 2; // Choisir un entier aléatoire a tel que 1 < a < N
        int g = gcd(a, N);
        if (g > 1) {
            return {g, N / g}; // Retourner le facteur non trivial de N
        }

        int r = find_period_classical(a, N);
        if (r % 2 != 0 || mod_exp(a, r / 2, N) == N - 1) {
            continue; // Retourner à l'étape 1 avec un nouvel entier a
        }

        int factor1 = gcd(mod_exp(a, r / 2, N) - 1, N);
        int factor2 = gcd(mod_exp(a, r / 2, N) + 1, N);
        if (factor1 > 1 && factor2 > 1) {
            return {factor1, factor2}; // Retourner les deux facteurs non triviaux de N
        }
    }
}