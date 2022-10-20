#include <iostream>
#include <chrono>
#include <sstream>
#include <string>
#include <fstream>
#include <stdexcept>

#include "../include/hash.hpp"

using hrClock = std::chrono::high_resolution_clock;

struct user {
    std::string name;
    std::string public_key;
    long time_created;
};

long get_epoch_time() {
    auto now = std::chrono::system_clock::now();
    auto now_s = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto epoch = now_s.time_since_epoch();
    auto epoch_value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
    long epoch_time = epoch_value.count();

    return epoch_time;
}

user create_user(const std::string& name) {
    Hash hash256;
    
    long current_time = get_epoch_time();
    std::string val_to_hash = name + std::to_string(current_time);
    std::string public_key = hash256.hash(val_to_hash);

    user new_user {name, public_key, current_time};

    return new_user;
}

void create_file(std::string file_type, std::string file_name) {
    std::ofstream file (file_name);
    file << "#blockchain-data:" << file_type << "\n";
}

void write_user(const std::string& file_name, const user& u) {
    const std::string file_type = "#blockchain-data:users"; 

    std::stringstream buffer;
    std::string header;


    buffer << u.name << "~"
            << u.public_key << "~"
            << u.time_created << "~";

    std::fstream file (file_name);
    std::getline(file, header);

    if (header.length()) {
        bool correct_file_type = ( header == file_type );

        if (!correct_file_type) {
            // TODO throw from custom error class
            throw std::runtime_error("Incorrect file type");
        }
    } else {
        file.close();
        create_file("user", file_name);
        file.open(file_name);
    }

    file.seekp(0, std::ios_base::end);
    
    file << buffer.rdbuf();
}

user read_user(const std::string& file_name) {
    const std::string file_type = "#blockchain-data:users";

    std::stringstream buffer;
    std::string header;

    std::ifstream file (file_name);
    std::getline(file, header);

    if (header.length()) {
        bool correct_file_type = ( header == file_type );

        if (!correct_file_type) {
            // TODO throw from custom error class
            throw std::runtime_error("Incorrect file type");
        }
    } else {
        // TODO throw from custom error class
        throw std::runtime_error("File not found");
    }


    buffer << file.rdbuf();

    user u;
    std::string line;
    // while (getline(buffer, line, '~')) {
    getline(buffer, line, '~');
    u.name = line;
    getline(buffer, line, '~');
    u.public_key = line;
    getline(buffer, line, '~');
    u.time_created = std::stoul(line);

    return u;
}

int main() {
    std::string file_name = "C:/Users/Eimantas/Desktop/simple-blockchain/user.dat";
    user u = create_user("namemee");
    std::cout << u.name << " " << u.public_key << " " << u.time_created << "\n";

    write_user(file_name, u);
    user u1 = read_user(file_name);

    std::cout << u1.name << " " << u1.public_key << " " << u1.time_created << "\n";
}

    // auto start = hrClock::now();
    // auto stop = hrClock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    // std::cout << duration.count() * 1e-9;