# TP : Algorithme de Shor

## Introduction

En arithmétique modulaire et en informatique quantique, l’algorithme de Shor est un algorithme quantique pour factoriser un entier naturel `N` en temps `O((logN)^3)` et en espace `O(logN)`, nommé en l’honneur de Peter Shor. Peter Williston Shor, né le 14 août 1959, est un mathématicien américain. Il est connu pour son travail sur le calcul quantique et il est professeur au MIT.

Beaucoup de cryptosystèmes à clé publique, tels que le RSA, deviendraient vulnérables si l’algorithme de Shor était un jour implémenté dans un calculateur quantique pratique. Un message chiffré avec RSA peut être déchiffré par factorisation de sa clé publique `N`, qui est le produit de deux nombres premiers. En l’état actuel des connaissances, il n’existe pas d’algorithme classique capable de faire cela en temps `O((logN)^k)` pour n’importe quel `k`, donc, les algorithmes classiques connus deviennent rapidement impraticables quand `N` augmente, à la différence de l’algorithme de Shor qui peut casser le RSA en temps polynomial. Il a été aussi étendu pour attaquer beaucoup d’autres cryptosystèmes à clé publique.

Comme tous les algorithmes pour calculateur quantique, l’algorithme de Shor est probabiliste : il donne la réponse correcte avec une haute probabilité et la probabilité d’échec peut être diminuée en répétant l’algorithme. L’algorithme de Shor fut utilisé en 2001 par un groupe d’IBM, qui factorisa 15 en 3 et 5, en utilisant un calculateur quantique de 7 qubits.

## Objectif

Durant cette séance, nous allons implémenter l’algorithme de Shor afin de retrouver ce résultat.

1. Dans un premier temps, vous allez mettre en place l’algorithme de Shor sans la fonction quantique. Au vu des tests que nous allons réaliser (`N = 15`), une implémentation classique est largement suffisante. Cette fonction de recherche de la période `r` se fera donc avec une transformée de Fourier discrète classique.
2. Dans un second temps, nous allons remplacer la fonction de recherche de période par une version quantique. Pour cela, vous utiliserez la description de la transformée de Fourier Quantique et la librairie [Quantum++](https://github.com/softwareQinc/qpp).

## Algorithme de Shor

### Étapes de l'algorithme

1. Choisir un entier aléatoire `a` tel que `1 < a < N`.
2. Si `gcd(a, N) > 1`, retourner `gcd(a, N)` comme facteur non trivial de `N`.
3. Trouver le plus petit entier `r` tel que `a^r ≡ 1 (mod N)` (c’est-à-dire l’ordre de `a` modulo `N`).
4. Si `r` est impair ou `a^(r/2) ≡ -1 (mod N)`, retourner à l’étape 1 avec un nouvel entier `a`.
5. Calculer `gcd(a^(r/2) - 1, N)` et `gcd(a^(r/2) + 1, N)`.
6. Si `gcd(a^(r/2) - 1, N) > 1` et `gcd(a^(r/2) + 1, N) > 1`, retourner les deux facteurs non triviaux de `N`. Sinon, retourner à l’étape 1 avec un nouveau choix de `a`.

### Exemple

Pour `N = 15`, `a = 7`, `r = 4` :
- `gcd(7^2 - 1, 15) = gcd(48, 15) = 3`
- `gcd(7^2 + 1, 15) = gcd(50, 15) = 5`

## Structure du projet

Le projet est structuré comme suit :

```bash
├── build
├── CMakeLists.txt
├── main.cpp
├── Makefile
├── qpp/
├── README.md
├── shor_classical.cpp
├── shor_quantum.cpp
├── shor_utils.hpp
└── TP1-Quantum.pdf
```

### Fichiers

- `main.cpp` : Contient le point d'entrée du programme et teste les fonctionnalités des implémentations classiques et quantiques de l'algorithme de Shor.
- `shor_classical.cpp` : Implémentation classique de l'algorithme de Shor.
- `shor_quantum.cpp` : Implémentation quantique de l'algorithme de Shor utilisant la librairie Quantum++.
- `shor_utils.hpp` : Contient les fonctions utilitaires communes (`gcd` et `mod_exp`).

## Compilation et exécution

### Prérequis

- CMake
- [Quantum++](https://github.com/softwareQinc/qpp)

### Instructions

1. Cloner le dépôt Quantum++ et suivre les instructions d'installation.
2. Créer un répertoire de build et naviguer dedans :
```sh
   mkdir build
   cd build
```
3. Configurer le projet avec CMake :
```sh
cmake ..
```
4. Construire le projet :
```sh
cmake --build .
```
5. Exécuter le programme :
```sh
./shor
```
### Résultats attendus
Le programme doit afficher les facteurs de N trouvés par les implémentations classiques et quantiques de l'algorithme de Shor.
```sh
Algorithme de Shor pour la factorisation de 15
=============================================
Utilisation de l'algorithme classique...
Les facteurs de 15 (classique) sont : 3 et 5
Utilisation de l'algorithme quantique...
Les facteurs de 15 (quantique) sont : 3 et 5
```
### References
- [Quantum++](https://github.com/softwareQinc/qpp)
- [Algorithme de Shor](https://fr.wikipedia.org/wiki/Algorithme_de_Shor)
