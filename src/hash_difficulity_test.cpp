#include <iostream>
#include <string>

#include "../include/hash.hpp"

int main() {
    Hash hash256;
    std::string out;

    for (int i = 0; i < INT_MAX; i++) {
        for (int j = 0; j < INT_MAX; j++) {
            out = hash256.hash(std::to_string(i) + std::to_string(j));
            
            if (out.substr(0, 5) == "0000")
                std::cout<< out << " " << i << " " << j << "\n";
        }
    }
}