#include <iostream>
#include <string>

#include "../include/hash.hpp"

int main() {
    Hash hash256;
    std::string out;
    std::string zeros = "0000";
    int len = zeros.length();

    for (int i = 0; i < INT_MAX; i++) {
        out = hash256.hash(std::to_string(i));
        
        if (out.substr(0, len) == zeros) {
            std::cout<< out << " " << i << "\n";
        }
    }

}