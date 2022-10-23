#include "../include/blockchain.hpp"

// CONSTRUCTOR
Blockchain::Blockchain() {
    CreateDirectory(data_folder.c_str(), NULL);
    CreateDirectory(blocks_folder.c_str(), NULL);

    std::string block_file_prefix = blocks_folder + "/block";
    std::string block_file_suffix = ".dat";
    
    std::string file_name = block_file_prefix + std::to_string(blockchain_height) + block_file_suffix;
    while (file_exists(file_name)) {
        blockchain_height++;
        file_name = block_file_prefix + std::to_string(blockchain_height) + block_file_suffix;
    }
}

// PUBLIC METHODS

// TODO make writing blocks private
void Blockchain::write_to_disk(std::string data_type) {
    const std::string file_type = "#blockchain-data:" + data_type;

    std::string header;

    std::string filename;

    if (data_type == "transactions") {
        filename = unconfirmed_transaction_file;
    } else if (data_type == "users") {
        filename = user_data_file;
    } else if (data_type == "block_to_mine") {
        filename = block_to_mine_file;
    } else if (data_type == "block") {
        filename = blocks_folder + "/block" + std::to_string(blockchain_height) + ".dat";
    }

    std::fstream file (filename);
    std::getline(file, header);

    if (header.length()) {
        bool correct_file_type = ( header == file_type );

        if (!correct_file_type) {
            throw std::runtime_error("Incorrect file type");
        }
    } else {
        file.close();
        create_file(data_type, filename);
        file.open(filename);
    }

    file.seekp(0, std::ios_base::end);
    
    std::stringstream buffer;
    if (data_type == "transactions") {
        buffer = generate_transactions_buffer();
        file << buffer.rdbuf();
    } else if (data_type == "users") {
        buffer = generate_users_buffer();
        file << buffer.rdbuf();
    } else if (data_type == "block_to_mine") {
        buffer = generate_block_to_mine_buffer();
        file << buffer.rdbuf();
    } else if (data_type == "block") {
        buffer = generate_block_buffer();
        file << buffer.rdbuf();
    }
}

// PRIVATE METHODS

void Blockchain::create_file(std::string data_type, std::string file_name) {
    std::ofstream file (file_name);
    file << "#blockchain-data:" << data_type << "\n";
}


long Blockchain::get_epoch_time() {
    auto now = std::chrono::system_clock::now();
    auto now_s = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto epoch = now_s.time_since_epoch();
    auto epoch_value = std::chrono::duration_cast<std::chrono::seconds>(epoch);
    long epoch_time = epoch_value.count();

    return epoch_time;
}

std::string Blockchain::get_merkleroot(std::vector<std::string> transactions) {
    // TODO generate merkleroot
    return hash256.hash("");
}

bool Blockchain::file_exists (const std::string& name) {
    struct stat buffer;   
    return (stat (name.c_str(), &buffer) == 0); 
}