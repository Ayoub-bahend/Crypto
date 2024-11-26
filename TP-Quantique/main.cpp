#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "shor_utils.hpp"

using namespace std;

// Déclarations des fonctions
vector<int> shor_classical(int N);
vector<int> shor_quantum(int N);

int main() {
    srand(time(NULL)); // Initialiser la graine aléatoire

    int N = 15; // Exemple : N = 15

    cout << "Algorithme de Shor pour la factorisation de " << N << endl;
    cout << "=============================================" << endl;

    // Utilisation de l'algorithme classique
    cout << "Utilisation de l'algorithme classique..." << endl;
    vector<int> factors_classical = shor_classical(N);
    cout << "Les facteurs de " << N << " (classique) sont : " << factors_classical[0] << " et " << factors_classical[1] << endl;

    // Utilisation de l'algorithme quantique
    cout << "Utilisation de l'algorithme quantique..." << endl;
    vector<int> factors_quantum = shor_quantum(N);
    cout << "Les facteurs de " << N << " (quantique) sont : " << factors_quantum[0] << " et " << factors_quantum[1] << endl;

    return 0;
}