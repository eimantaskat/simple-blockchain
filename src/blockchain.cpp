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

void Blockchain::mine_block() {
    if (!file_exists(block_to_mine_file)) {
        std::cout << "No blocks found to mine\n";
        return;
    }

    block mineable_block;
    mineable_block = get_block_to_mine();

    std::string prev_block_hash = mineable_block.prev_block_hash;
    std::string timestamp = std::to_string(mineable_block.time);
    std::string version = mineable_block.version;
    std::string merkleroot = mineable_block.merkleroot;

    int diff_target = mineable_block.difficulity_target;
    std::string difficulity_target = std::to_string(diff_target);

    std::uniform_int_distribution<unsigned long long> nonce_dist(0, ULLONG_MAX);

    std::string padding (diff_target, '0');
    
    std::string hash;
    do {
        unsigned long long nonce = nonce_dist(mt);
        std::string str_nonce = std::to_string(nonce);

        std::string value_to_hash = prev_block_hash + timestamp + version + merkleroot + str_nonce + difficulity_target;
        // TODO improve my hash
        // hash = hash256.hash(value_to_hash);
        hash = sha256(value_to_hash);
    } while (hash.substr(0, diff_target) != padding);

    std::cout << "Block mined! Hash: " << hash << "\n";

    block_to_mine = block();
    mined_block = mineable_block;

    int result = remove(block_to_mine_file.c_str());
    write_to_disk("block");
    
    blockchain_height++;
}


void Blockchain::create_transaction(const std::string& from, const std::string& to, const double& amount) {
    long current_time = get_epoch_time();

    // TODO check if users exsist and have enough money to send
    // FIXME if transactions with same sender, receiver and amount are created in the same second they get same id
    std::string val_to_hash = from + to + std::to_string(amount) + std::to_string(current_time);
    std::string transaction_id = hash256.hash(val_to_hash);

    // FIXME inputs and outputs
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
        create_first_block();
        return;
    }

    block prev_block = get_best_block();

    std::string prev_block_hash = prev_block.hash;
    long time = get_epoch_time();

    std::vector<std::string> tx;
    for (transaction t : transactions) {
        tx.push_back(t.id);
    }

    std::string merkleroot = get_merkleroot(tx);

    block new_block;
    new_block.height = blockchain_height;
    new_block.prev_block_hash = prev_block_hash;
    new_block.time = time;
    new_block.version = blockchain_version;
    new_block.merkleroot = merkleroot;
    new_block.difficulity_target = difficulity_target;
    new_block.tx = tx;

    block_to_mine = new_block;
}

void Blockchain::create_first_block() {
    std::cout << blockchain_height << "\n";
    if (blockchain_height != 0) {
        std::cout << "First block has already been created!\n";
        return;
    }

    std::vector<user> users;
    users = get_users();

    std::uniform_real_distribution<double> amount_dist(0, 1000);

    std::vector<transaction> tx;
    for (user u:users) {
        std::string from = "";
        std::string to = u.public_key;
        double amount = amount_dist(mt);
        long current_time = get_epoch_time();

        std::string val_to_hash = from + to + std::to_string(amount) + std::to_string(current_time);
        std::string transaction_id = hash256.hash(val_to_hash);

        std::vector<txo> in;
        std::vector<txo> out;

        txo tx_out;
        tx_out.transaction_id = transaction_id;
        tx_out.to = to;
        tx_out.amount = amount;
        out.push_back(tx_out);

        transaction new_transaction {transaction_id, from, to, amount, current_time, in, out};

        tx.push_back(new_transaction);
    }

    std::string prev_block_hash = hash256.hash("");
    long time = get_epoch_time();

    std::vector<std::string> tx_ids;
    for (transaction t : tx) {
        tx_ids.push_back(t.id);
    }

    std::string merkleroot = get_merkleroot(tx_ids);

    block new_block;
    new_block.height = blockchain_height;
    new_block.prev_block_hash = prev_block_hash;
    new_block.time = time;
    new_block.version = blockchain_version;
    new_block.merkleroot = merkleroot;
    new_block.difficulity_target = difficulity_target;
    new_block.tx = tx_ids;

    if (!file_exists(block_to_mine_file)) {
        block_to_mine = new_block;
        write_to_disk("block_to_mine");
    }
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
    file.close();

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

        // FIXME inputs and outputs

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
    file.close();

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
            << t.time << "~";
            // FIXME inputs and outputs
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

std::stringstream Blockchain::generate_block_to_mine_buffer() {
    std::stringstream buffer;

    buffer << block_to_mine.height << "~"
            << block_to_mine.prev_block_hash << "~"
            << block_to_mine.time << "~"
            << block_to_mine.version << "~"
            << block_to_mine.merkleroot << "~"
            << block_to_mine.difficulity_target << "~";
    for (std::string t:block_to_mine.tx) {
        buffer << t << "~";
    }

    block_to_mine = block();
    return buffer;
}

std::stringstream Blockchain::generate_block_buffer() {
    std::stringstream buffer;

        buffer << mined_block.hash << "~"
                << mined_block.height << "~"
                << mined_block.prev_block_hash << "~"
                << mined_block.time << "~"
                << mined_block.version << "~"
                << mined_block.merkleroot << "~"
                << mined_block.nonce << "~"
                << mined_block.difficulity_target << "~";
        for (std::string t:mined_block.tx) {
            buffer << t << "~";
        }

    mined_block = block();
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

Blockchain::block Blockchain::get_best_block() {
    block best_block;

    // TODO return last block
    return best_block;
}

Blockchain::block Blockchain::get_block_to_mine() {
    const std::string file_type = "#blockchain-data:block_to_mine";

    std::stringstream buffer;
    std::string header;

    std::ifstream file (block_to_mine_file);
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
    file.close();

    std::string line;
    block mineable_block;

    getline(buffer, line, '~');
    mineable_block.height = std::stoi(line);

    getline(buffer, line, '~');
    mineable_block.prev_block_hash = line;

    getline(buffer, line, '~');
    mineable_block.time = std::stol(line);

    getline(buffer, line, '~');
    mineable_block.version = line;

    getline(buffer, line, '~');
    mineable_block.merkleroot = line;

    getline(buffer, line, '~');
    mineable_block.difficulity_target = std::stoi(line);

    while (getline(buffer, line, '~')) {
        mineable_block.tx.push_back(line);
    }

    return mineable_block;
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