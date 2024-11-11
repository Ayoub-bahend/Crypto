#include <iostream>
#include <fstream>
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



    printSeparator();
    std::cout << "Mini-Rijndael Logarithm Tables Demonstration" << std::endl;
    printSeparator();
    uint16_t generator = 0x0002; // Example generator
    uint16_t polynomial = 0x002B; // P16(X) = X16 + X5 + X3 + X2 + 1
    auto logTable = generateLogTable(generator, polynomial);
    auto inverseLogTable = generateInverseLogTable(logTable);
    auto zechLogTable = generateZechLogTable(logTable, inverseLogTable);
    // Write the tables to a file
    std::ofstream outFile("logarithm_tables.txt");
    if (outFile.is_open()) {
        outFile << "Logarithm Table:" << std::endl;
        for (size_t i = 0; i < logTable.size(); ++i) {
            outFile << "log(" << i << ") = " << logTable[i] << std::endl;
        }
        outFile << std::endl << "Inverse Logarithm Table:" << std::endl;
        for (size_t i = 0; i < inverseLogTable.size(); ++i) {
            outFile << "antilog(" << i << ") = " << inverseLogTable[i] << std::endl;
        }
        outFile << std::endl << "Zech's Logarithm Table:" << std::endl;
        for (size_t i = 0; i < zechLogTable.size(); ++i) {
            outFile << "Zech(" << i << ") = " << zechLogTable[i] << std::endl;
        }
        outFile.close();
    } else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }
    std::cout << "Logarithm tables have been written to 'logarithm_tables.txt'." << std::endl;
    printSeparator();
    std::cout << "End of Demonstration" << std::endl;
    printSeparator();

    return 0;
}