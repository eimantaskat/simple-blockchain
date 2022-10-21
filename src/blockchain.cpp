#include "../include/blockchain.hpp"

// CONSTRUCTOR
Blockchain::Blockchain() {
    Hash hash256;

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

void Blockchain::create_transaction(const std::string& from, const std::string& to, const double& amount) {
    long current_time = get_epoch_time();

    // TODO check if users exsist and have enough money to send
    // FIXME if transactions with same sender, receiver and amount are created in the same second they get same id
    std::string val_to_hash = from + to + std::to_string(amount) + std::to_string(current_time);
    std::string transaction_id = hash256.hash(val_to_hash);

    transaction new_transaction {transaction_id, from, to, amount, current_time};

    cached_transactions.push_back(new_transaction);
}

void Blockchain::create_user(const std::string& name) {
    long current_time = get_epoch_time();
    // FIXME if users with same username are created in the same second they get same public keys
    std::string val_to_hash = name + std::to_string(current_time);
    std::string public_key = hash256.hash(val_to_hash);

    user new_user {name, public_key, current_time};

    cached_users.push_back(new_user);
}

void Blockchain::create_block(const std::vector<transaction>& transactions) {
    if (blockchain_height == 0) {
        create_first_block(transactions);
        return;
    }

    block prev_block = get_best_block();

    int height = prev_block.height + 1;
    std::string prev_block_hash = prev_block.hash;
    long time = get_epoch_time();

    std::vector<std::string> tx;
    for (transaction t : transactions) {
        tx.push_back(t.id);
    }

    std::string merkleroot = get_merkleroot(tx);

    // nonce: unsigned long long ULLONG_MAX

    block new_block;
    new_block.height = height;
    new_block.prev_block_hash = prev_block_hash;
    new_block.time = time;
    new_block.version = blockchain_version;
    new_block.merkleroot = merkleroot;
    new_block.difficulity_target = difficulity_target;
    new_block.tx = tx;

    block_to_mine = new_block;
}

std::vector<Blockchain::transaction> Blockchain::get_transactions() {
    const std::string file_type = "#blockchain-data:transactions";

    std::stringstream buffer;
    std::string header;

    std::ifstream file (unconfirmed_transaction_file);
    std::getline(file, header);

    if (header.length()) {
        bool correct_file_type = ( header == file_type );

        if (!correct_file_type) {
            throw std::runtime_error("Incorrect file type");
        }
    } else {
        throw std::runtime_error("File not found");
    }


    buffer << file.rdbuf();

    std::vector<transaction> transactions;
    std::string line;
    transaction t;
    while (getline(buffer, line, '~')) {
        t.id = line;

        getline(buffer, line, '~');
        t.from = line;

        getline(buffer, line, '~');
        t.to = line;

        getline(buffer, line, '~');
        t.amount = std::stod(line);

        getline(buffer, line, '~');
        t.time = std::stoul(line);

        getline(buffer, line, '~');
        t.spent = (line == "1");

        transactions.push_back(t);
    }

    return transactions;
}

std::vector<Blockchain::transaction> Blockchain::get_user_transactions(const std::string& public_key, bool unspent) {
    // TODO get user's transactions
    return cached_transactions;
}

std::vector<Blockchain::user> Blockchain::get_users() {
    const std::string file_type = "#blockchain-data:users";

    std::stringstream buffer;
    std::string header;

    std::ifstream file (user_data_file);
    std::getline(file, header);

    if (header.length()) {
        bool correct_file_type = ( header == file_type );

        if (!correct_file_type) {
            throw std::runtime_error("Incorrect file type");
        }
    } else {
        throw std::runtime_error("File not found");
    }


    buffer << file.rdbuf();

    std::vector<user> users;
    std::string line;
    user u;
    while (getline(buffer, line, '~')) {
        u.name = line;

        getline(buffer, line, '~');
        u.public_key = line;

        getline(buffer, line, '~');
        u.time_created = std::stoul(line);

        users.push_back(u);
    }

    return users;
}

std::stringstream Blockchain::generate_transactions_buffer() {
    std::stringstream buffer;

    for (transaction t:cached_transactions) {
        buffer << t.id << "~"
            << t.from << "~"
            << t.to << "~"
            << t.amount << "~"
            << t.time << "~"
            << t.spent << "~";
    }

    cached_transactions.clear();

    return buffer;
}

std::stringstream Blockchain::generate_users_buffer() {
    std::stringstream buffer;

    for (user u:cached_users) {
        buffer << u.name << "~"
            << u.public_key << "~"
            << u.time_created << "~";
    }

    cached_users.clear();

    return buffer;
}

void Blockchain::write_to_disk(std::string data_type) {
    const std::string file_type = "#blockchain-data:" + data_type;

    std::string header;

    std::string filename;

    if (data_type == "transactions") {
        filename = unconfirmed_transaction_file;
    } else if (data_type == "users") {
        filename = user_data_file;
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
    }
}

Blockchain::block Blockchain::get_best_block() {
    block best_block;

    std::cout << blockchain_height;

    // TODO return last block
    return best_block;
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