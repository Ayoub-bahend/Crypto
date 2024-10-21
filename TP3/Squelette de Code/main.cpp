#include "rsa.hpp"
#include <iostream>

int main() {
    char plain[7] = "44";  
    char cipher[1000];  

    std::cout << "------------------ using GMP Library ------------------------" << std::endl << std::endl;
    
    generate_rsa_keys();
    encrypt(plain, cipher);
    decrypt(cipher);


    std::cout << "--------------- using crafted functions ------------------------" << std::endl << std::endl;
    customized_generate_rsa_keys();
    customized_encrypt(plain, cipher);
    customized_decrypt(cipher);

    // Clearing gmp integers
    clear_gmp_integers();
    return 0;
}
