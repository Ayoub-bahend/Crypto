cmake_minimum_required(VERSION 3.15)
project(shor_quantum)

# Spécifiez le standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ajouter les chemins des en-têtes Quantum++ et ses dépendances
include_directories(
    "/home/ayoub/Documents/Crypto/TP-Quantique/qpp/include"
    "/home/ayoub/Documents/Crypto/TP-Quantique/qpp/qasmtools/include"
    "/usr/include/eigen3"
)

# Ajouter les fichiers source
add_executable(shor main.cpp shor_classical.cpp shor_quantum.cpp)
