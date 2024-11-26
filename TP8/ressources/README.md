# Projet Cryptographie Avancée - TP 8 : TEA et Fonctions Associées

## Description

Ce projet a pour objectif de travailler sur l'implémentation et l'étude du chiffrement **Tiny Encryption Algorithm (TEA)**, qui est un algorithme de chiffrement par bloc. Nous allons explorer son fonctionnement en profondeur à travers plusieurs exercices, notamment l'implémentation des modes de chiffrement **ECB**, **CBC** et **OFB**, ainsi que des fonctions de hachage et des codes d'authentification des messages (MAC).

### Objectifs principaux :
- Implémentation du chiffrement TEA et des fonctions associées.
- Utilisation des modes de chiffrement **ECB**, **CBC** et **OFB**.
- Création d'une fonction de hachage basée sur la structure de Merkle-Damgård avec TEA.
- Implémentation de l'authentification des messages via **HMAC**.
- Analyse de failles de sécurité dans TEA.

## Structure du projet

Le projet est divisé en plusieurs exercices :

### Exercice 1 : Réseau de Feistel de TEA
- Étude du réseau de Feistel utilisé par TEA.
- Expression des relations entre les blocs de texte chiffré.
- Vérification de la nécessité d’inverser la fonction `F` pour le déchiffrement.

### Exercice 2 : Implémentation de TEA
- Implémentation des fonctions de chiffrement (`feistel_enc`) et de déchiffrement (`feistel_dec`).
- Comparaison de l’implémentation avec la référence fournie.

### Exercice 3 : Mode ECB (Electronic Codebook)
- Implémentation du mode de chiffrement **ECB** avec TEA.
- Fonction `ecb_encrypt` pour chiffrer un message.
- Fonction `ecb_decrypt` pour déchiffrer un message.

### Exercice 4 : Mode CBC (Cipher Block Chaining)
- Implémentation du mode **CBC** avec TEA.
- Chiffrement et déchiffrement avec l’utilisation d’un vecteur d’initialisation (IV).
- Analyse du chaînage entre les blocs et des parallélisations possibles.

### Exercice 5 : Mode OFB (Output Feedback)
- Implémentation du mode **OFB** avec TEA.
- Transformation du chiffrement par blocs en chiffrement par flot.
- Étude des opérations parallélisables et pré-calculables.

### Exercice 6 : Fonction de Hachage avec Merkle-Damgård
- Implémentation d'une fonction de hachage en utilisant la structure de Merkle-Damgård et le chiffrement TEA comme fonction de compression.

### Exercice 7 : HMAC (Hash-based Message Authentication Code)
- Implémentation du **HMAC** en utilisant TEA comme fonction de hachage.
- Fonction `hash_mac` pour générer un HMAC et fonction `hash_mac_verification` pour vérifier l'intégrité du message.

### Exercice 8 : Chiffrement Authentifié (Encrypt-Then-MAC)
- Utilisation du paradigme **Encrypt-Then-MAC** pour garantir l'intégrité des messages.
- Implémentation de la fonction `cbc_encrypt_mac` et `cbc_decrypt_mac` avec TEA en mode CBC et HMAC.

### Exercice 9 : Exploitation d'une faille de sécurité de TEA
- Vérification de la vulnérabilité dans TEA qui permet d'altérer la clé sans affecter le chiffrement.
- Analyse de la fonction de hachage basée sur TEA et démonstration des collisions.
- Proposer une attaque contre l'intégrité du chiffrement authentifié avec TEA.

## Prérequis

- C Compiler (par exemple, GCC).


## Compilation

Pour compiler le projet, il vous suffit d'utiliser la commande suivante dans le terminal :

```bash
gcc tea_ref.c -o  prog1
gcc tea_starter.c -o  prog2
gcc failleTEA.c -o  prog3
```

## Utilisation

    Chiffrement avec TEA :
        Vous pouvez utiliser la fonction feistel_enc pour chiffrer un bloc de données avec TEA.

    Déchiffrement avec TEA :
        Utilisez la fonction feistel_dec pour déchiffrer un bloc de données avec TEA.

    Mode ECB :
        Chiffrez un message avec ecb_encrypt et déchiffrez-le avec ecb_decrypt.

    Mode CBC :
        Utilisez cbc_encrypt pour chiffrer avec un vecteur d'initialisation (IV) et cbc_decrypt pour déchiffrer.

    Mode OFB :
        Générez un flot de chiffrement avec ofb_stream et chiffrez un message avec ofb_encrypt.

    HMAC et Fonction de Hachage :
        Utilisez hash_mac pour calculer le HMAC d'un message et hash_mac_verification pour vérifier l'intégrité d'un message.

## Sécurité

Bien que TEA soit un algorithme simple et efficace, il présente des failles de sécurité connues. Ce projet met en évidence certaines de ces vulnérabilités et démontre comment elles peuvent être exploitées, en particulier dans l’utilisation de TEA comme fonction de hachage ou dans les mécanismes d'authentification des messages.

##Conclusion

Ce projet vise à comprendre et à implémenter l'algorithme Tiny Encryption Algorithm (TEA) et à explorer ses différents modes de chiffrement, ainsi que la création de fonctions de hachage et de codes d'authentification. L’étude de ses failles de sécurité permet de mieux comprendre ses limites et son adéquation dans des environnements sécurisés.
