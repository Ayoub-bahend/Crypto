# TP3 - Théorie des Codes

**Groupe** :  
- Ayoub BAHEND 
- Abdelmonaim MOUHALHAL

## Objectif du TP

Ce TP consiste à implémenter et tester le chiffrement et déchiffrement RSA en utilisant la librairie GMP dans un premier temps. Ensuite, dans une seconde phase, nous allons remplacer certaines fonctions de GMP par nos propres algorithmes, notamment pour :
- `mpz_powm` : remplacer par l'algorithme d'exponentiation rapide.
- `mpz_nextprime` : remplacer par une fonction qui utilise le test de primalité de Rabin-Miller.
- `mpz_invert` : remplacer par l'algorithme d'Euclide étendu.

---

## 1. Génération des Clés RSA avec GMP

Dans la première partie de ce TP, nous avons utilisé les fonctions fournies par la librairie GMP pour générer des clés RSA. Voici un aperçu des étapes principales :

- **Génération des nombres premiers** : 
  Nous avons utilisé la fonction `mpz_nextprime()` pour générer deux grands nombres premiers **p** et **q**.

- **Calcul du modulo n** :
  Nous avons calculé \( n = p * q \).

- **Calcul de φ(n)** :
  Le calcul de φ(n) se fait à partir de \( p \) et \( q \), avec la formule \( φ(n) = (p - 1) * (q - 1) \).

- **Choix de l'exposant e** :
  Nous avons choisi l'exposant **e** tel que \( 1 < e < φ(n) \) et \( gcd(e, φ(n)) = 1 \).

- **Calcul de l'inverse modulaire (d)** :
  Nous avons utilisé la fonction `mpz_invert()` pour calculer l'inverse modulaire \( d \), qui sert à déchiffrer les messages.

---

## 2. Remplacement des fonctions GMP par des implémentations personnalisées

Dans cette deuxième phase, nous avons implémenté nos propres algorithmes pour remplacer les fonctions GMP mentionnées précédemment.

### 2.1. Remplacement de *mpz_powm()* par l'algorithme d'exponentiation rapide

```cpp
void powm(mpz_t &g, mpz_t &k, mpz_t &p, mpz_t &m);
```



L'algorithme d'exponentiation rapide permet de calculer efficacement la puissance d'un nombre modulo un autre nombre, ce qui est crucial dans le chiffrement RSA. Au lieu de calculer directement \( g^k \mod m \), cet algorithme réduit considérablement le nombre de multiplications nécessaires, en utilisant la décomposition binaire de l'exposant **k**.

**Principe** :  
L'algorithme repose sur le fait que \( g^k \mod m \) peut être exprimé comme une série de carrés successifs, en vérifiant à chaque étape si le bit courant de la décomposition binaire de **k** est à 1 ou non.

### 2.2. Remplacement de *mpz_nextprime()* par le test de primalité de Rabin-Miller

#### Test de primalité de Rabin-Miller :
```cpp
bool PrimalityTest(int accuracy, mpz_t & odd_integer);
void nextprime(mpz_t &p, mpz_t s);
```

Le test de primalité de Rabin-Miller est un test probabiliste qui permet de vérifier si un nombre est probablement premier. Il repose sur des propriétés de la théorie des nombres et garantit qu'un nombre donné est premier avec une très forte probabilité, en fonction du nombre de répétitions du test.

Dans notre implémentation :
- **nextprime()** génère un nombre impair **s** et utilise le test de Rabin-Miller pour vérifier sa primalité. Si le test réussit, **s** est renvoyé comme nombre premier.

**Principe** :  
Le test fonctionne en vérifiant si un certain critère basé sur l'algorithme d'exponentiation modulaire est satisfait. Si un nombre échoue au test, il est composite. Sinon, il est probablement premier.

### 2.3. Remplacement de *mpz_invert()* par l'algorithme d'Euclide étendu
```cpp
void extended_GCD(const mpz_t a, const mpz_t b, mpz_t & k1, mpz_t & k2);
void invert(mpz_t & d, const mpz_t e, const mpz_t x);
```


L'algorithme d'Euclide étendu permet de trouver l'inverse modulaire de **e** modulo **φ(n)**, c'est-à-dire **d**. Cet inverse est utilisé comme clé privée dans RSA.

**Principe** :  
L'algorithme d'Euclide étendu est une généralisation de l'algorithme classique d'Euclide pour calculer le PGCD. Il permet également de trouver des coefficients (appelés coefficients de Bézout) qui expriment le PGCD comme une combinaison linéaire des deux nombres d'entrée, ce qui nous permet de calculer l'inverse modulaire.

---

## 3. Tests sur les données aléatoires

Pour valider nos implémentations, nous avons testé le chiffrement et déchiffrement RSA sur des messages aléatoires.

1. **Chiffrement avec les clés GMP** :
   Nous avons chiffré un message de test en utilisant les clés générées avec GMP.
   
2. **Déchiffrement avec les clés GMP** :
   Le message a été correctement déchiffré, prouvant que l'implémentation avec GMP fonctionne comme attendu.

3. **Chiffrement avec les fonctions personnalisées** :
   Nous avons également testé notre propre implémentation du chiffrement RSA avec les fonctions personnalisées (algorithme d'exponentiation rapide, primalité de Rabin-Miller, etc.).

4. **Déchiffrement avec les fonctions personnalisées** :
   Le message a été déchiffré correctement, prouvant que nos fonctions remplacent bien celles de GMP.

---

## Conclusion

Nous avons réussi à implémenter le chiffrement et déchiffrement RSA en utilisant la librairie GMP, puis à remplacer certaines des fonctions critiques par nos propres algorithmes personnalisés. Le test sur des messages aléatoires a prouvé que notre implémentation fonctionne correctement et respecte les exigences du protocole RSA.

---

### Annexe

- **Annexe 1 : Algorithme d'Euclide étendu**
- **Annexe 2 : Algorithme de Rabin-Miller**
