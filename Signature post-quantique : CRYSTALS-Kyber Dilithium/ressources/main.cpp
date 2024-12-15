#include <iostream>
#include <vector>
#include <random>
#include <cmath>

using namespace std;

// Paramètres pour Baby Kyber
const int n = 4; // Degré maximum des polynômes
const int k = 2; // Nombre de polynômes par vecteur
const int q = 17; // Modulo
const int eta_1 = 3; // Contrôle des coefficients des petits polynômes

// Structure pour représenter un polynôme
typedef vector<int> Polynomial;

// Fonction pour assurer les coefficients modulo q
int mod(int value, int modulo) {
    return (value % modulo + modulo) % modulo;
}

// Fonction pour générer un polynôme avec des coefficients aléatoires
Polynomial generateRandomPolynomial(int maxCoeff, int degree = n) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(-maxCoeff, maxCoeff);

    Polynomial poly(degree, 0);
    for (int i = 0; i < degree; ++i) {
        poly[i] = mod(dist(gen), q); // Modulo q pour assurer les coefficients dans [0, q)
    }
    return poly;
}

// Fonction pour afficher un polynôme
void printPolynomial(const Polynomial &poly) {
    for (size_t i = 0; i < poly.size(); ++i) {
        cout << poly[i] << "x^" << i;
        if (i != poly.size() - 1) cout << " + ";
    }
    cout << endl;
}

// Addition de deux polynômes
Polynomial addPolynomials(const Polynomial &a, const Polynomial &b) {
    Polynomial result(n, 0);
    for (int i = 0; i < n; ++i) {
        result[i] = mod(a[i] + b[i], q);
    }
    return result;
}

// Multiplication de deux polynômes modulo X^n + 1
Polynomial multiplyPolynomials(const Polynomial &a, const Polynomial &b) {
    Polynomial result(n, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int index = (i + j) % n; // Modulo X^n + 1
            result[index] = mod(result[index] + a[i] * b[j], q);
        }
    }
    return result;
}

// Fonction de génération de clés
pair<vector<Polynomial>, vector<Polynomial>> keyGeneration() {
    // Génération de la matrice A (publique)
    vector<Polynomial> A(k, Polynomial(n));
    for (int i = 0; i < k; ++i) {
        A[i] = generateRandomPolynomial(q - 1);
    }

    // Génération de la clé secrète s
    vector<Polynomial> s(k, Polynomial(n));
    for (int i = 0; i < k; ++i) {
        s[i] = generateRandomPolynomial(eta_1);
    }

    // Génération du vecteur d'erreurs e
    vector<Polynomial> e(k, Polynomial(n));
    for (int i = 0; i < k; ++i) {
        e[i] = generateRandomPolynomial(eta_1);
    }

    // Calcul du vecteur t = A * s + e
    vector<Polynomial> t(k, Polynomial(n));
    for (int i = 0; i < k; ++i) {
        t[i] = e[i];
        for (int j = 0; j < k; ++j) {
            t[i] = addPolynomials(t[i], multiplyPolynomials(A[j], s[j]));
        }
    }

    return {A, t};
}

// Fonction de chiffrement
pair<vector<Polynomial>, Polynomial> encrypt(const vector<Polynomial> &A, const vector<Polynomial> &t, const Polynomial &message) {
    // Génération de r, e1 et e2
    vector<Polynomial> r(k, Polynomial(n));
    for (int i = 0; i < k; ++i) {
        r[i] = generateRandomPolynomial(eta_1);
    }

    vector<Polynomial> e1(k, Polynomial(n));
    for (int i = 0; i < k; ++i) {
        e1[i] = generateRandomPolynomial(eta_1);
    }

    Polynomial e2 = generateRandomPolynomial(eta_1);

    // Calcul de u = A^T * r + e1
    vector<Polynomial> u(k, Polynomial(n));
    for (int i = 0; i < k; ++i) {
        u[i] = e1[i];
        for (int j = 0; j < k; ++j) {
            u[i] = addPolynomials(u[i], multiplyPolynomials(A[j], r[j]));
        }
    }

    // Calcul de v = t^T * r + e2 + message
    Polynomial v = e2;
    for (int i = 0; i < k; ++i) {
        v = addPolynomials(v, multiplyPolynomials(t[i], r[i]));
    }
    v = addPolynomials(v, message);

    return {u, v};
}

Polynomial decrypt(const vector<Polynomial> &s, const pair<vector<Polynomial>, Polynomial> &ciphertext) {
    const auto &[u, v] = ciphertext;

    // Calcul de mn = v - s^T * u
    Polynomial mn = v;
    for (int i = 0; i < k; ++i) {
        mn = addPolynomials(mn, multiplyPolynomials(s[i], u[i]));
    }

    // Afficher les coefficients de mn pour vérification
    cout << "mn (after subtraction): ";
    printPolynomial(mn);

    // Assurez-vous que les coefficients sont dans [0, q)
    for (int i = 0; i < n; ++i) {
        mn[i] = mod(mn[i], q);
    }

    // Afficher les coefficients de mn après modulation
    cout << "mn (after modulo q): ";
    printPolynomial(mn);

    // Récupération du message avec un seuil ajusté pour plus de tolérance
    Polynomial message(n, 0);
    for (int i = 0; i < n; ++i) {
        // Améliorer le seuil pour plus de tolérance sur les erreurs
        if (mn[i] >= (q / 2)) {
            message[i] = 1;
        } else {
            message[i] = 0;
        }
    }

    // Afficher le message déchiffré avant retour
    cout << "Decrypted message: ";
    printPolynomial(message);

    return message;
}




int main() {
    cout << "Baby Kyber Implementation:\n";

    // Génération des clés
    auto [A, t] = keyGeneration();

    // Message à chiffrer
    Polynomial message = {1, 0, 1, 1}; // Représente 11 en binaire
    cout << "Original message: ";
    printPolynomial(message);

    // Chiffrement
    auto ciphertext = encrypt(A, t, message);
    cout << "Ciphertext generated.\n";

    // Déchiffrement
    Polynomial decryptedMessage = decrypt(A, ciphertext);
    cout << "Decrypted message: ";
    printPolynomial(decryptedMessage);

    return 0;
}

/*#include <iostream>
#include <vector>
#include <random>
#include <cmath>

using namespace std;

// Paramètres pour Kyber1024
const int n = 256;  // Degré maximum des polynômes
const int k = 4;    // Nombre de polynômes par vecteur
const int q = 3329; // Modulo
const int eta_1 = 2; // Contrôle des coefficients des petits polynômes

// Structure pour représenter un polynôme
typedef vector<int> Polynomial;

// Fonction pour assurer les coefficients modulo q
int mod(int value, int modulo) {
    return (value % modulo + modulo) % modulo;
}

// Fonction pour générer un polynôme avec des coefficients aléatoires
Polynomial generateRandomPolynomial(int maxCoeff, int degree = n) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(-maxCoeff, maxCoeff);

    Polynomial poly(degree, 0);
    for (int i = 0; i < degree; ++i) {
        poly[i] = mod(dist(gen), q); // Modulo q pour assurer les coefficients dans [0, q)
    }
    return poly;
}

// Fonction pour afficher un polynôme
void printPolynomial(const Polynomial &poly) {
    for (size_t i = 0; i < poly.size(); ++i) {
        cout << poly[i] << "x^" << i;
        if (i != poly.size() - 1) cout << " + ";
    }
    cout << endl;
}

// Addition de deux polynômes
Polynomial addPolynomials(const Polynomial &a, const Polynomial &b) {
    Polynomial result(n, 0);
    for (int i = 0; i < n; ++i) {
        result[i] = mod(a[i] + b[i], q);
    }
    return result;
}

// Multiplication de deux polynômes modulo X^n + 1
Polynomial multiplyPolynomials(const Polynomial &a, const Polynomial &b) {
    Polynomial result(n, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int index = (i + j) % n; // Modulo X^n + 1
            result[index] = mod(result[index] + a[i] * b[j], q);
        }
    }
    return result;
}

// Fonction de génération de clés pour Kyber1024
pair<vector<Polynomial>, vector<Polynomial>> keyGeneration() {
    // Génération de la matrice A (publique)
    vector<Polynomial> A(k, Polynomial(n));
    for (int i = 0; i < k; ++i) {
        A[i] = generateRandomPolynomial(q - 1);
    }

    // Génération de la clé secrète s
    vector<Polynomial> s(k, Polynomial(n));
    for (int i = 0; i < k; ++i) {
        s[i] = generateRandomPolynomial(eta_1);
    }

    // Génération du vecteur d'erreurs e
    vector<Polynomial> e(k, Polynomial(n));
    for (int i = 0; i < k; ++i) {
        e[i] = generateRandomPolynomial(eta_1);
    }

    // Calcul du vecteur t = A * s + e
    vector<Polynomial> t(k, Polynomial(n));
    for (int i = 0; i < k; ++i) {
        t[i] = e[i];
        for (int j = 0; j < k; ++j) {
            t[i] = addPolynomials(t[i], multiplyPolynomials(A[j], s[j]));
        }
    }

    return {A, t};
}

// Fonction de chiffrement avec des messages de longueur variable
pair<vector<vector<Polynomial>>, Polynomial> encrypt(const vector<Polynomial> &A, const vector<Polynomial> &t, const vector<int> &message) {
    // Diviser le message en blocs de taille appropriée pour le chiffrement
    vector<Polynomial> message_polynomials;
    for (size_t i = 0; i < message.size(); i += n) {
        Polynomial block(n, 0);
        for (int j = 0; j < n && i + j < message.size(); ++j) {
            block[j] = message[i + j];
        }
        message_polynomials.push_back(block);
    }

    vector<vector<Polynomial>> ciphertext_u;  // Correction ici
    Polynomial ciphertext_v;

    for (const auto& block : message_polynomials) {
        vector<Polynomial> r(k, Polynomial(n));
        for (int i = 0; i < k; ++i) {
            r[i] = generateRandomPolynomial(eta_1);
        }

        vector<Polynomial> e1(k, Polynomial(n));
        for (int i = 0; i < k; ++i) {
            e1[i] = generateRandomPolynomial(eta_1);
        }

        Polynomial e2 = generateRandomPolynomial(eta_1);

        // Calcul de u = A^T * r + e1
        vector<Polynomial> u(k, Polynomial(n));
        for (int i = 0; i < k; ++i) {
            u[i] = e1[i];
            for (int j = 0; j < k; ++j) {
                u[i] = addPolynomials(u[i], multiplyPolynomials(A[j], r[j]));
            }
        }

        // Calcul de v = t^T * r + e2 + message
        Polynomial v = e2;
        for (int i = 0; i < k; ++i) {
            v = addPolynomials(v, multiplyPolynomials(t[i], r[i]));
        }
        v = addPolynomials(v, block);

        ciphertext_u.push_back(u);  // Correction ici
        ciphertext_v = v;
    }

    return {ciphertext_u, ciphertext_v};
}

// Fonction de déchiffrement avec des messages de longueur variable
vector<int> decrypt(const vector<Polynomial> &s, const pair<vector<vector<Polynomial>>, Polynomial> &ciphertext) {
    const auto &[u, v] = ciphertext;  // Avertissement si vous ne compilez pas en C++17 ou supérieur

    vector<int> message;

    for (size_t i = 0; i < u.size(); ++i) {
        Polynomial mn = v;
        for (int j = 0; j < k; ++j) {
            mn = addPolynomials(mn, multiplyPolynomials(s[j], u[i][j]));  // Correction ici
        }

        // Modulo pour assurer que les coefficients sont dans [0, q)
        for (int i = 0; i < n; ++i) {
            mn[i] = mod(mn[i], q);
        }

        // Convertir le polynôme déchiffré en un vecteur de bits (0 ou 1)
        for (int i = 0; i < n; ++i) {
            if (mn[i] >= (q / 2)) {
                message.push_back(1);
            } else {
                message.push_back(0);
            }
        }
    }

    return message;
}

// Calcul du facteur d'expansion
double calculateExpansionFactor(int message_size, int ciphertext_size) {
    return double(ciphertext_size) / double(message_size);
}

int main() {
    cout << "Kyber1024 Implementation:\n";

    // Génération des clés
    auto [A, t] = keyGeneration();

    // Message à chiffrer (représente un message binaire)
    vector<int> message = {1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1}; // Représente 10101010101 en binaire
    cout << "Original message: ";
    for (int bit : message) {
        cout << bit;
    }
    cout << endl;

    // Chiffrement
    auto ciphertext = encrypt(A, t, message);
    cout << "Ciphertext generated.\n";

    // Déchiffrement
    vector<int> decryptedMessage = decrypt(A, ciphertext);
    cout << "Decrypted message: ";
    for (int bit : decryptedMessage) {
        cout << bit;
    }
    cout << endl;

    // Calcul du facteur d'expansion
    int message_size = message.size();
    int ciphertext_size = k * n * 2;  // Deux vecteurs de taille k * n pour le ciphertext
    double expansion_factor = calculateExpansionFactor(message_size, ciphertext_size);
    cout << "Expansion Factor: " << expansion_factor << endl;

    return 0;
}
*/