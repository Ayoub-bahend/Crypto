#include <iostream>
#include "common.hpp"

void printSeparator() {
    std::cout << "----------------------------------------" << std::endl;
}

int main() {
    printSeparator();
    std::cout << "Mini-Rijndael Arithmetic in F216 Demonstration" << std::endl;
    printSeparator();

    uint16_t a = 0x1234;
    uint16_t b = 0x5678;

    uint16_t sum = addF216(a, b);
    uint16_t product = multiplyF216(a, b);

    std::cout << "Sum of " << std::hex << a << " and " << b << " in F216: " << sum << std::endl;
    std::cout << "Product of " << std::hex << a << " and " << b << " in F216: " << product << std::endl;

    printSeparator();
    std::cout << "End of Demonstration" << std::endl;
    printSeparator();

    return 0;
}