#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <numeric>
#include <qpp/qpp.hpp>
#include "shor_utils.hpp"

using namespace std;
using namespace qpp;

cmat quantum_fourier_transform(int q) {
    idx dim = 1 << q; // Dimension du système
    cmat QFT = cmat::Zero(dim, dim);

    for (idx i = 0; i < dim; ++i) {
        for (idx j = 0; j < dim; ++j) {
            double angle = 2.0 * pi * static_cast<double>(i * j) / static_cast<double>(dim);
            QFT(i, j) = std::exp(std::complex<double>(0, angle)) / std::sqrt(static_cast<double>(dim));
        }
    }
    return QFT;
}

// Fonction pour trouver la période r quantiquement
int find_period_quantum(int a, int N) {
    // Nombre de qubits nécessaires
    int q = static_cast<int>(std::ceil(std::log2(N * N))); // Nombre de qubits pour représenter Q
    qpp::idx Q = 1 << q; // Taille de l'espace Hilbert total

    // Initialisation de l'état quantique
    ket psi = mket({0, 0}, {static_cast<qpp::idx>(q), static_cast<qpp::idx>(q)});

    // Définir l'opération Uf (a^x mod N) comme matrice unitaire
    cmat U = cmat::Identity(Q, Q); // Matrice unitaire pour Uf
    for (qpp::idx x = 0; x < Q; ++x) {
        U(x, (x * a) % N) = 1;
    }

    // Matrice contrôlée sur q qubits
    cmat Uf = gt.CTRL(U, {0, static_cast<qpp::idx>(q - 1)}, {static_cast<qpp::idx>(q), static_cast<qpp::idx>(2 * q - 1)}, 1, Q);

    // Appliquer la transformée de Fourier quantique
    for (qpp::idx i = 0; i < static_cast<qpp::idx>(q); ++i) {
        psi = apply(psi, gt.H, {i});
    }
    psi = applyCTRL(psi, Uf, {0, static_cast<qpp::idx>(q - 1)}, {static_cast<qpp::idx>(q), static_cast<qpp::idx>(2 * q - 1)});
    psi = apply(psi, quantum_fourier_transform(q), {0, static_cast<qpp::idx>(q - 1)});

    // Mesurer le registre de sortie
    auto result = measure(psi, {0, static_cast<qpp::idx>(q - 1)});
    int y = std::get<RES>(result);

    // Trouver la période r
    int r = 1;
    while (mod_exp(a, r, N) != 1) {
        r++;
    }

    return r;
}

// Fonction principale de l'algorithme de Shor
vector<int> shor_quantum(int N) {
    while (true) {
        int a = rand() % (N - 2) + 2; // Choisir un entier aléatoire a tel que 1 < a < N
        int g = gcd(a, N);
        if (g > 1) {
            return {g, N / g}; // Retourner le facteur non trivial de N
        }

        int r = find_period_quantum(a, N);
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