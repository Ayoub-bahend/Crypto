### Partage de Secret de Shamir (TP7)

## Introduction

Ce TP implémente le **schéma de partage de secret de Shamir**. L'objectif est de diviser un secret en plusieurs parts (shares) et de pouvoir le reconstruire en combinant un certain nombre de parts. Ce programme utilise l'arithmétique modulaire et les polynômes pour assurer la confidentialité et la robustesse du secret.

## Fonctionnalités

1. **Initialisation des paramètres** :
   - Génération d'un nombre premier \( p \) (espace de travail \( \mathbb{Z}/p\mathbb{Z} \)).
   - Génération d'un secret aléatoire \( S \).
   - Définition du seuil \( k \) (nombre minimal de parts nécessaires pour reconstruire le secret).
   - Nombre maximal de parts \( n \).

2. **Génération des parts** :
   - Calcul des coefficients d'un polynôme aléatoire de degré \( k-1 \).
   - Calcul des parts \((x_i, y_i)\) pour chaque utilisateur.

3. **Reconstruction du secret** :
   - Utilisation des polynômes de Lagrange pour reconstruire le secret \( S \) à partir d'au moins \( k \) parts.

4. **Ajout de nouvelles parts** :
   - Génération d'une nouvelle part sans modifier les parts existantes.

5. **Modification du seuil \( k \)** :
   - Modification du seuil \( k \) et reconstruction du secret avec le nouveau seuil.

---

## Détails Techniques

### 1. **Algorithme Principal**

Le schéma suit ces étapes :

1. **Initialisation** :
   - Un nombre premier \( p \) est généré.
   - Le secret \( S \) est choisi aléatoirement dans \( [0, p-1] \).
   - Un polynôme \( P(x) \) de degré \( k-1 \) est créé avec \( S \) comme terme constant.

2. **Partage** :
   - Chaque utilisateur reçoit une part \((x_i, y_i)\), où \( x_i \) est unique et \( y_i = P(x_i) \mod p \).

3. **Reconstruction** :
   - Avec \( k \) parts ou plus, l'interpolation de Lagrange est utilisée pour retrouver \( S = P(0) \).

4. **Flexibilité** :
   - Des parts supplémentaires peuvent être générées.
   - Le seuil \( k \) peut être modifié dynamiquement.

---

### 2. **Principales Fonctions**

#### Génération des paramètres
- **`generatePrime(mpz_t &p)`** :
  Génère un nombre premier aléatoire \( p \).
- **`generateRandom(mpz_t &s, mpz_t p)`** :
  Génère un entier aléatoire \( s \) dans \( [0, p-1] \).

#### Polynôme
- **`generateCoefs(mpz_t *tab, mpz_t p, int k)`** :
  Crée les coefficients \( a_1, a_2, \dots, a_{k-1} \) pour le polynôme \( P(x) \).
- **`compute_image(mpz_t &image, mpz_t *a, mpz_t x, int k, mpz_t s)`** :
  Calcule \( P(x) \mod p \) pour un \( x \) donné.

#### Partage
- **`computeShares(mpz_t *x, mpz_t *y, mpz_t *a, mpz_t s, int k, int n)`** :
  Génère \( n \) parts \((x_i, y_i)\).

#### Reconstruction
- **`computeLagrange(mpz_t *alpha, mpz_t *x, int k, int n, mpz_t p)`** :
  Calcule les coefficients de Lagrange \(\lambda_i\).
- **`reconstructSecret(mpz_t *alpha, mpz_t *y, mpz_t Sr, mpz_t p, int start, int k)`** :
  Reconstitue le secret \( S \).

#### Nettoyage
- **`clear_tab_mpz(mpz_t *tab, int t)`** :
  Libère la mémoire associée aux tableaux GMP.

---

### 3. **Exemple d'Exécution**

1. **Paramètres initiaux** :
   - \( n = 4 \), \( k = 3 \), \( p = 13981 \), \( S = 1234 \).
   - Polynôme \( P(x) = a_2 x^2 + a_1 x + S \).

2. **Parts générées** :
   - \((x_1, y_1), (x_2, y_2), \dots, (x_4, y_4)\).

3. **Reconstruction du secret** :
   - Utilisation de \( (x_1, y_1), (x_2, y_2), (x_3, y_3) \) pour retrouver \( S \).

4. **Ajout d'une nouvelle part** :
   - Nouvelle part \((x_5, y_5)\).

5. **Modification du seuil** :
   - Incrémentation de \( k \) à 4 et vérification de la reconstruction.

---

## Instructions de Compilation et d'Exécution

1. **Prérequis** :
   - **Bibliothèque GMP** : La bibliothèque GMP (GNU Multiple Precision) est requise pour manipuler de grands entiers.

   Installer GMP :
   ```sh
   sudo apt-get install libgmp-dev
   ```

2. **Compilation** :
   Compiler le programme avec le `Makefile` fourni :
   ```sh
   make
   ```

3. **Exécution** :
   Lancer le programme compilé :
   ```sh
   ./tp7_shamir
   ```

---

## Exemple de Résultat (Mode DEBUG)

1. **Nombre premier généré** :
   ```
   Random Prime 'p' = 13981
   ```

2. **Secret généré** :
   ```
   Secret number 'S' = 1234
   ```

3. **Polynôme généré** :
   ```
   Polynom 'P(X)' = 981X^2 + 567X + 1234
   ```

4. **Parts calculées** :
   ```
   Login and share of each users : 
   (x1=1 ; y1=2782), (x2=2 ; y2=6427), (x3=3 ; y3=10372), (x4=4 ; y4=5678)
   ```

5. **Reconstruction du secret** :
   ```
   Reconstruction of the secret : S = 1234
   ```

6. **Ajout d'une nouvelle part** :
   ```
   Login and share of each users : 
   (x1=1 ; y1=2782), ..., (x5=5 ; y5=8881)
   ```

7. **Modification du seuil** :
   ```
   Reconstruction of the secret (with the new threshold) : S = 1234
   ```

---

## Limitations et Améliorations

- **Limitations** :
  - Le seuil ne peut pas dépasser \( n \).
  - La taille des nombres premiers est fixe (modifiable via `BITSTRENGTH`).

- **Améliorations possibles** :
  - Interface utilisateur pour configurer \( n, k \).
  - Sauvegarde des parts dans un fichier pour une utilisation ultérieure.

---
