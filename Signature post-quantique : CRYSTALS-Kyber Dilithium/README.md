# Baby Kyber Implementation

## Overview

This project implements **Baby Kyber**, a simplified version of the Kyber cryptographic algorithm. Kyber is a post-quantum public-key encryption system designed to resist attacks by quantum computers. Baby Kyber simplifies the parameters and removes some optimizations (like ciphertext compression) to make the algorithm easier to understand and implement.

The implementation showcases key generation, encryption, and decryption processes for Baby Kyber. It also provides an intuitive understanding of modular arithmetic and polynomial algebra in the context of lattice-based cryptography.

## Features

- **Key Generation:** Generates a public-private key pair.
- **Encryption:** Encrypts a binary message using the public key.
- **Decryption:** Recovers the original message using the private key.
- **Polynomial Operations:** Includes addition, multiplication, and modular reduction of polynomials.

## Mathematical Foundation

Baby Kyber works in the ring:
\[ R_q = \mathbb{Z}_q[X]/(X^n + 1) \]
where:
- \( q \): Modulus for coefficients (e.g., 17 in this implementation).
- \( n \): Maximum degree of polynomials (e.g., 4 in this implementation).
- \( X^n + 1 \): Polynomial modulus.

The algorithm relies on the hardness of certain lattice problems to ensure cryptographic security.

## How It Works

### 1. Key Generation
- Generates:
  - A random matrix \( A \) with coefficients modulo \( q \).
  - A secret vector \( s \) and error vector \( e \), both with small coefficients.
- Computes the public vector \( t \) as:
  \[ t = A \cdot s + e \]

### 2. Encryption
- For a binary message \( m \), generates random polynomials \( r \), \( e_1 \), and \( e_2 \).
- Computes the ciphertext:
  - \( u = A^T \cdot r + e_1 \)
  - \( v = t^T \cdot r + e_2 + m \)

### 3. Decryption
- Recovers the message by:
  \[ m' = v - s^T \cdot u \]
- Applies thresholding to map coefficients back to binary values.

## Parameters

| Parameter | Value | Description                           |
|-----------|-------|---------------------------------------|
| \( n \)    | 4     | Degree of polynomials                |
| \( k \)    | 2     | Number of polynomials per vector     |
| \( q \)    | 17    | Modulus for polynomial coefficients  |
| \( \eta_1 \)| 3     | Bounds for small coefficients        |

## Code Structure

- **Polynomial Operations:** Addition, multiplication, and modular reduction.
- **Key Generation:** Implements the generation of public and private keys.
- **Encryption:** Computes the ciphertext from a given message.
- **Decryption:** Recovers the original message from the ciphertext.

## Usage

### Compilation

To compile the project, use a C++ compiler like `g++`:
```bash
make
```



## Key Points

- **Simplified Parameters:** Baby Kyber uses smaller parameters (\( n=4, q=17 \)) for readability.
- **Polynomial Algebra:** Operates with polynomials modulo \( X^n + 1 \) and \( q \).
- **Post-Quantum Security:** Demonstrates the principles of lattice-based cryptography.

