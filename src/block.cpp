#include "../include/blockchain.hpp"


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

void Blockchain::create_block() {
    if (blockchain_height == 0) {
        create_first_block();
        return;
    }

    std::vector<transaction> transactions = get_transactions(true);

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
    
    cached_transactions = tx;
    write_to_disk("transactions");

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
