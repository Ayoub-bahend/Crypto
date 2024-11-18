# Rapport de TP : Implémentation de Mini-Rijndael

## Introduction

Ce projet consiste en l'implémentation de l'algorithme de chiffrement Mini-Rijndael, une version simplifiée de l'algorithme AES (Advanced Encryption Standard). Le but de ce TP est de comprendre les différentes étapes de chiffrement et de déchiffrement, ainsi que de mesurer les performances de ces opérations.

## Structure du Projet

Le projet est structuré comme suit :
```sh
├── Exo_1.cpp : Contient l'implémentation de l'algorithme Mini-Rijndael.
├── Makefile  : Fichier de configuration pour la compilation du projet.
├── README.md
├── secret.enc : Fichier chiffré à déchiffrer en mode CFB.
└── TP.pdf
```

## Compilation et Exécution

Pour compiler le projet, utilisez la commande suivante :
```sh
make
```
Pour exécuter le programme, utilisez la commande suivante :
```sh
./tp
```
## Fonctionnalités Implémentées

### 1. Expansion de Clé

La fonction `KeyExpansion` génère les clés de tour à partir de la clé initiale.
```sh
void KeyExpansion(aes_t K[3][4], aes_t roundKeys[6][3][4]);
```
### 2. Chiffrement

La fonction `Encryption` chiffre un état donné en utilisant les clés de tour.
```sh
void Encryption(aes_t state[3][4], aes_t roundKeys[6][3][4]) {
    AddRoundKey(state, roundKeys[0]);
    for (int round = 1; round <= 4; ++round) {
        SubBytes(state);
        ShiftRow(state);
        MixColumns(state);
        AddRoundKey(state, roundKeys[round]);
    }
    // Dernier tour (sans MixColumns)
    SubBytes(state);
    ShiftRow(state);
    AddRoundKey(state, roundKeys[5]);
}
```
### 3. Déchiffrement

La fonction `Decryption` déchiffre un état donné en utilisant les clés de tour.
```sh
void Decryption(aes_t state[3][4], aes_t roundKeys[6][3][4]) {
    // Ajouter la clé de tour K5
    AddRoundKey(state, roundKeys[5]);
    // Dernier tour (sans InvMixColumns)
    InverseShiftRow(state);
    SubBytes(state);
    AddRoundKey(state, roundKeys[4]);
    // Tours 4 à 1
    for (int round = 4; round >= 1; --round) {
        InvMixColumns(state);
        InverseShiftRow(state);
        SubBytes(state);
        AddRoundKey(state, roundKeys[round - 1]);
    }
}
```
### 4. Mesure du Temps de Chiffrement et Déchiffrement

Les fonctions `MeasureEncryptionTime` et `MeasureDecryptionTime` mesurent le temps nécessaire pour chiffrer et déchiffrer un grand nombre de blocs.
```sh
void MeasureEncryptionTime(aes_t roundKeys[6][3][4]);
void MeasureDecryptionTime(aes_t roundKeys[6][3][4]);
```
### 5. Déchiffrement en Mode CFB

La fonction `decryptCFB` déchiffre un fichier chiffré en mode CFB.
```sh
void decryptCFB(const std::vector<aes_t>& ciphertext, const aes_t iv[3][4], aes_t roundKeys[6][3][4], std::vector<aes_t>& plaintext);
```
## Tests et Résultats

### Test de Chiffrement et Déchiffrement

Le programme teste le chiffrement et le déchiffrement en comparant les résultats obtenus avec les valeurs attendues.

### Mesure des Performances

Les temps de chiffrement et de déchiffrement sont mesurés pour différents nombres de blocs, et les résultats sont affichés.

### Déchiffrement en Mode CFB

Le fichier `secret.enc` est déchiffré en mode CFB, et le résultat est écrit dans `decrypted_output.txt`.

### Extrait de Code
```sh
if (success_mix) {
    cout << "\nLe résultat de la fonction Mix correspond aux valeurs attendues !" << std::endl;
} else {
    cout << "\nLe résultat de la fonction Mix NE correspond PAS aux valeurs attendues." << std::endl;
}

printSeparator();
cout << "Mesure du temps de déchiffrement Mini-Rijndael" << std::endl;
printSeparator();
MeasureDecryptionTime(roundKeys);

printSeparator();
cout << "Mesure du temps de chiffrement Mini-Rijndael" << std::endl;
printSeparator();
MeasureEncryptionTime(roundKeys);

printSeparator();
return 0;
```
## Conclusion

Ce TP a permis de comprendre les différentes étapes de l'algorithme Mini-Rijndael, ainsi que de mesurer les performances de chiffrement et de déchiffrement. Les résultats obtenus montrent que l'implémentation est correcte et efficace.