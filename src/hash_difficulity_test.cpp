#include <iostream>
#include <string>

#include "../include/hash.hpp"

int main() {
    Hash hash256;
    std::string out;
    std::string zeros = "0000";
    int len = zeros.length();

    for (int i = 0; i < INT_MAX; i++) {
        for (int j = 0; j < INT_MAX; j++) {
            out = hash256.hash(std::to_string(i) + std::to_string(j));
            
            if (out.substr(0, len) == zeros)
                std::cout<< out << " " << i << " " << j << "\n";
        }
    }
}