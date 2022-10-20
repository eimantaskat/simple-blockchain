#pragma once

#include <string>
#include <random>
#include <sstream>
#include <vector>
#include <algorithm>

class Hash {
    public:
        Hash() {};
        std::string hash(std::string);
        std::string hashWithSalt(std::string, std::string);
    private:
        std::string strInput;
        std::vector<char> vectorInput;

        std::string generateKey(std::vector<char>);
        int generateSeed(std::vector<char>);

        std::string compress(std::vector<char>, int length = 64);

        std::vector<char> toCharVector(std::string);

        std::string toHex(std::string);
        std::string toHex(int);
        int toDec(std::string);
};