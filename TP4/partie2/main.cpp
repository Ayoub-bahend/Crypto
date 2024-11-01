#include <iostream>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <string>

using namespace std;

// Hash function using SHA-256
BIGNUM* hashMessage(const string& message) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)message.c_str(), message.size(), hash);
    
    BIGNUM* bnHash = BN_new();
    BN_bin2bn(hash, SHA256_DIGEST_LENGTH, bnHash);
    return bnHash;
}

// Key generation function
void keyGen(BIGNUM* p, BIGNUM* q, BIGNUM* g, BIGNUM* &x, BIGNUM* &y, BN_CTX* ctx) {
    x = BN_new();
    y = BN_new();
    
    // Generate private key x: 1 <= x <= q-1
    BN_rand_range(x, q);
    
    // Generate public key y = g^x mod p
    BN_mod_exp(y, g, x, p, ctx);
}

// Signing function
void sign(const string& message, BIGNUM* p, BIGNUM* q, BIGNUM* g, BIGNUM* x, BIGNUM* &r, BIGNUM* &s, BIGNUM* &k_out, BN_CTX* ctx) {
    BIGNUM* k = BN_new();
    BIGNUM* kInv = BN_new();
    BIGNUM* hash = hashMessage(message);
    
    r = BN_new();
    s = BN_new();

    do {
        // Choose k randomly in the range [1, q-1]
        BN_rand_range(k, q);

        // Calculate r = (g^k mod p) mod q
        BIGNUM* temp = BN_new();
        BN_mod_exp(temp, g, k, p, ctx);
        BN_mod(r, temp, q, ctx);
        BN_free(temp);

    } while (BN_is_zero(r));

    // Calculate k^-1 mod q
    BN_mod_inverse(kInv, k, q, ctx);

    // Calculate s = (k^-1 * (H(m) + x * r)) mod q
    BIGNUM* xr = BN_new();
    BN_mod_mul(xr, x, r, q, ctx);

    BIGNUM* hm_plus_xr = BN_new();
    BN_mod_add(hm_plus_xr, hash, xr, q, ctx);
    
    BN_mod_mul(s, kInv, hm_plus_xr, q, ctx);

    k_out = BN_dup(k);  // Save k for the attack

    // Cleanup
    BN_free(k);
    BN_free(kInv);
    BN_free(hash);
    BN_free(xr);
    BN_free(hm_plus_xr);
}

// Verification function
bool verify(const string& message, BIGNUM* r, BIGNUM* s, BIGNUM* p, BIGNUM* q, BIGNUM* g, BIGNUM* y, BN_CTX* ctx) {
    if (BN_is_zero(r) || BN_cmp(r, q) >= 0 || BN_is_zero(s) || BN_cmp(s, q) >= 0)
        return false;
    
    BIGNUM* w = BN_new();
    BN_mod_inverse(w, s, q, ctx);

    BIGNUM* hash = hashMessage(message);
    BIGNUM* u1 = BN_new();
    BIGNUM* u2 = BN_new();

    BN_mod_mul(u1, hash, w, q, ctx);
    BN_mod_mul(u2, r, w, q, ctx);

    BIGNUM* v = BN_new();
    BIGNUM* temp1 = BN_new();
    BIGNUM* temp2 = BN_new();

    BN_mod_exp(temp1, g, u1, p, ctx);
    BN_mod_exp(temp2, y, u2, p, ctx);
    BN_mod_mul(v, temp1, temp2, p, ctx);
    BN_mod(v, v, q, ctx);

    // Cleanup
    BN_free(w);
    BN_free(hash);
    BN_free(u1);
    BN_free(u2);
    BN_free(temp1);
    BN_free(temp2);

    bool isValid = BN_cmp(v, r) == 0;
    BN_free(v);
    return isValid;
}

// Attack to recover private key
BIGNUM* recoverPrivateKey(BIGNUM* r, BIGNUM* s, BIGNUM* k, const string& message, BIGNUM* q, BN_CTX* ctx) {
    BIGNUM* hash = hashMessage(message);
    BIGNUM* xr_inv = BN_new();
    BIGNUM* x = BN_new();
    
    // Compute (s * k - H(m)) * r^-1 mod q
    BIGNUM* sk = BN_new();
    BN_mod_mul(sk, s, k, q, ctx);

    BIGNUM* sk_minus_hash = BN_new();
    BN_mod_sub(sk_minus_hash, sk, hash, q, ctx);

    BN_mod_inverse(xr_inv, r, q, ctx);
    BN_mod_mul(x, sk_minus_hash, xr_inv, q, ctx);

    // Cleanup
    BN_free(hash);
    BN_free(sk);
    BN_free(sk_minus_hash);
    BN_free(xr_inv);

    return x;
}

int main() {
    BN_CTX* ctx = BN_CTX_new();

    // Initialize p, q, and g with example values
    BIGNUM* p = BN_new();
    BIGNUM* q = BN_new();
    BIGNUM* g = BN_new();
    BN_hex2bn(&p, "E95E4A5F737059DC60DF5991D45029409E60FC09AB045DA2D865A95D5E2BFC45");
    BN_hex2bn(&q, "F518AA8781A8DF278ABA4E7D64B7CB9D49462353");
    BN_hex2bn(&g, "4D6477CA6D1A18EED2D2D1BAE6366F5A0AC6244D9377E99F24A9B60D92B27F06");

    // Key generation
    BIGNUM* x;
    BIGNUM* y;
    keyGen(p, q, g, x, y, ctx);

    // Signing
    string message = "Hello, DSA!";
    BIGNUM* r;
    BIGNUM* s;
    BIGNUM* k;  // Used to store the k value for the attack
    sign(message, p, q, g, x, r, s, k, ctx);

    // Verification
    bool isValid = verify(message, r, s, p, q, g, y, ctx);
    cout << "Signature valid: " << (isValid ? "Yes" : "No") << endl;

    // Attack to recover x
    BIGNUM* x_recovered = recoverPrivateKey(r, s, k, message, q, ctx);
    cout << "Private key recovery successful: " << (BN_cmp(x, x_recovered) == 0 ? "Yes" : "No") << endl;

    // Cleanup
    BN_free(p);
    BN_free(q);
    BN_free(g);
    BN_free(x);
    BN_free(y);
    BN_free(r);
    BN_free(s);
    BN_free(k);
    BN_free(x_recovered);
    BN_CTX_free(ctx);

    return 0;
}
