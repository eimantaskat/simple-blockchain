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

    read_users();
    read_transactions();
    read_unvalidated_transactions();
}

Blockchain::~Blockchain() {
    write();
}

void Blockchain::write() {
    write_to_disk("users");
    write_to_disk("unvalidated_transactions");
    write_to_disk("transactions");
}

// PUBLIC METHODS

// TODO make writing blocks private
void Blockchain::write_to_disk(std::string data_type) {
    const std::string file_type = "#blockchain-data:" + data_type;

    std::string header;

    std::string filename;

    if (data_type == "unvalidated_transactions") {
        filename = unvalidated_transaction_file;
    } else if (data_type == "transactions") {
        filename = transaction_file;
    } else if (data_type == "users") {
        filename = user_data_file;
    } else if (data_type == "block_to_mine") {
        filename = block_to_mine_file;
    } else if (data_type == "block") {
        filename = blocks_folder + "/block" + std::to_string(mined_block.height) + ".dat";
    } else if (data_type == "confirmed_transactions") {
        filename = block_to_mine_file;
    }

    // std::fstream file (filename);
    // std::getline(file, header);
    // std::cout << header << "\n";
    // if (header.length()) {
    //     bool correct_file_type = ( header == file_type );

    //     if (!correct_file_type) {
    //         throw std::runtime_error("Incorrect file type");
    //     }
    // } else {
    //     file.close();
    //     create_file(data_type, filename);
    //     file.open(filename);
    //     std::getline(file, header);
    //     std::cout << header << "\n";
    // }

    create_file(data_type, filename);
    std::fstream file (filename);

    file.seekp(0, std::ios_base::end);
    
    std::stringstream buffer;
    if (data_type == "transactions") {
        buffer = generate_transactions_buffer();
        file << buffer.rdbuf();
    } else if (data_type == "unvalidated_transactions") {
        buffer = generate_unvalidated_transactions_buffer();
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
    for (auto tx_it = transactions.begin(); tx_it != transactions.end(); ++tx_it) {
        *tx_it = hash256.hash(*tx_it);
    }
    
    int size = transactions.size();
    std::vector<std::string> tmp;
    if (size % 2 == 0) {
        tmp.reserve(size / 2);
        for (int tx_index = 0; tx_index < size / 2; tx_index++) {
            std::string hash;
            std::string val_to_hash;

            val_to_hash = transactions[2 * tx_index] + transactions[2 * tx_index + 1];
            hash = hash256.hash(val_to_hash);

            tmp.push_back(hash);
        }
    } else {
        std::string hash;
        std::string val_to_hash;
        tmp.reserve((size + 1) / 2);
        for (int tx_index = 0; tx_index < size / 2; tx_index++) {

            val_to_hash = transactions[2 * tx_index] + transactions[2 * tx_index + 1];
            hash = hash256.hash(val_to_hash);

            tmp.push_back(hash);
        }
        hash = hash256.hash(transactions.back());
        tmp.push_back(hash);
    }

    if (tmp.size() == 1) {
        return tmp[0];
    } else {
        return get_merkleroot(tmp);
    }
}

bool Blockchain::file_exists (const std::string& name) {
    struct stat buffer;   
    return (stat (name.c_str(), &buffer) == 0); 
}