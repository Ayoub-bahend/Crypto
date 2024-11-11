#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>
#include <vector>

uint16_t addF216(uint16_t a, uint16_t b);
uint16_t multiplyByX(uint16_t a);
uint16_t multiplyF216(uint16_t a, uint16_t b);


std::vector<uint16_t> generateLogTable(uint16_t generator, uint16_t polynomial);
std::vector<uint16_t> generateInverseLogTable(const std::vector<uint16_t>& logTable);
std::vector<uint16_t> generateZechLogTable(const std::vector<uint16_t>& logTable, const std::vector<uint16_t>& inverseLogTable);

#endif // COMMON_HPP