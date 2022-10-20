#include <iostream>
#include <fstream>
#include <string>

struct test_struct {
    int a, b, c;
    char str[64];
};

void writeData(std::string fileName, test_struct data) {
    char* char_ptr;
    char_ptr = (char*)&data;

    std::ofstream file(fileName, std::ios::out | std::ios::binary);
    file.write(char_ptr, sizeof(test_struct));
    file.close();
}

test_struct readData(std::string fileName) {
    test_struct data;
    char* char_ptr;
    char_ptr = (char*)&data;

    std::ifstream file(fileName, std::ios::out | std::ios::binary);
    file.read(char_ptr, sizeof(test_struct));
    file.close();

    return data;
}

int main() {
    std::string file = "test.dat";

    // test_struct data {1, 2, 3, "some hash"};
    // std::cout << data.a << " " << data.b << " " << data.c << " " << data.str << "\n";

    // writeData(file, data);

    test_struct data1;
    data1 = readData(file);
    std::cout << data1.a << " " << data1.b << " " << data1.c << " " << data1.str << "\n";
}