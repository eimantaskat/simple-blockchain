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

    std::cout << "Loaded " << cached_users.size() << " users\n";
    std::cout << "Loaded " << cached_transactions.size() << " transactions\n";
    std::cout << "Loaded " << cached_unvalidated_transactions.size() << " unvalidated transactions\n";
}

// DESTRUCTOR
Blockchain::~Blockchain() {
    printf("Stopping blockchain...\n");
    write();
    printf("Blockchain stopped!\n");
}

// PUBLIC METHODS
void Blockchain::write() {
    write_to_disk("users");
    write_to_disk("unvalidated_transactions");
    write_to_disk("transactions");
}

void Blockchain::decentralized_mining(const int& max_guesses, const int& miners) {
    int counter = 0;

    for (int miner_no = 0; miner_no < 5; ++miner_no) {
        std::cout << "Miner: " << miner_no << "\n";
        create_block(true);
        write();
    }
}

// PRIVATE METHODS
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
    } else if (data_type == "block") {
        filename = blocks_folder + "/block" + std::to_string(mined_block.height) + ".dat";
    }

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
    } else if (data_type == "block") {
        buffer = generate_block_buffer();
        file << buffer.rdbuf();
    }
}

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
    int size = transactions.size();
    if (size == 0) {
        return hash256.hash("");
    }

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

std::string Blockchain::epoch_to_date(long time) {
    const time_t t = (time_t)time;
    return ctime(&t);
}

std::string Blockchain::time_distance_to_str(long distance) {
    int days = distance / 60 / 60 / 24;
    int hours = (distance / 60 / 60) % 24;
    int minutes = (distance / 60) % 60;
    int seconds = distance % 60;

    std::string str_distance = std::to_string(days) + "d " + std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
    return str_distance;
}