#include "../include/blockchain.hpp"


// PUBLIC METHODS

void Blockchain::mine_block() {
    if (!file_exists(block_to_mine_file)) {
        std::cout << "No blocks found to mine\n";
        return;
    }

    block mineable_block;
    mineable_block = get_block_to_mine();

    if (mineable_block.height != 0) {
        // for (std::string tx_id:mineable_block.tx) {
        auto it = mineable_block.tx.begin();
        while (it != mineable_block.tx.end()) {
            bool success = complete_transaction(*it);

            // erase from block if failed
            if (!success) {
                it = mineable_block.tx.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::string prev_block_hash = mineable_block.prev_block_hash;
    std::string timestamp = std::to_string(mineable_block.time);
    std::string version = mineable_block.version;
    std::string merkleroot = mineable_block.merkleroot;

    int diff_target = mineable_block.difficulity_target;
    std::string difficulity_target = std::to_string(diff_target);

    std::string padding (diff_target, '0');
    
    std::string hash;
    for (unsigned long long nonce = 0; ; ++nonce) {
        std::string str_nonce = std::to_string(nonce);

        std::string value_to_hash = prev_block_hash + timestamp + version + merkleroot + str_nonce + difficulity_target;
        hash = hash256.hash(value_to_hash);
        if (hash.substr(0, diff_target) == padding) {
            break;
        }
        if (nonce == ULLONG_MAX) {
            mineable_block.time = get_epoch_time();
            timestamp = std::to_string(mineable_block.time);
            nonce = 0;
        }
    }

    std::cout << "Block mined! Hash: " << hash << "\n";
    mineable_block.hash = hash;
    mined_block = mineable_block;

    std::vector<transaction> transactions = cached_unvalidated_transactions;

    int tx_amount = max_block_tx;
    if (tx_amount > transactions.size()) {
        tx_amount = transactions.size();
    }


    if (mined_block.height != 0) {
        // for (std::string tx_id:mined_block.tx) {
        //     complete_transaction(tx_id);
        // }

        int file_size = cached_unvalidated_transactions.size() - tx_amount;
        cached_unvalidated_transactions.resize(file_size);

        // trunc_unvalidated_transactions_file(file_size);
    }


    int result = remove(block_to_mine_file.c_str());
    write_to_disk("block");
    write_to_disk("transactions");
    write_to_disk("unvalidated_transactions");
    
    blockchain_height++;
}

void Blockchain::create_block() {
    if (blockchain_height == 0) {
        create_first_block();
        return;
    }

    // get transactions
    std::vector<transaction> transactions = cached_unvalidated_transactions;

    int tx_amount = max_block_tx;
    if (tx_amount > transactions.size()) {
        tx_amount = transactions.size();
    }

    std::vector<transaction> tx(transactions.end() - tx_amount, transactions.end());

    // ---
    block prev_block = get_best_block();

    std::string prev_block_hash = prev_block.hash;
    long time = get_epoch_time();

    std::vector<std::string> tx_str;
    for (transaction t : tx) {
        tx_str.push_back(t.id);
    }

    std::string merkleroot = get_merkleroot(tx_str);

    block new_block;
    new_block.height = blockchain_height;
    new_block.prev_block_hash = prev_block_hash;
    new_block.time = time;
    new_block.version = blockchain_version;
    new_block.merkleroot = merkleroot;
    new_block.difficulity_target = difficulity_target;
    new_block.tx = tx_str;

    if (!file_exists(block_to_mine_file)) {
        block_to_mine = new_block;
        write_to_disk("block_to_mine");
    }
}

void Blockchain::create_first_block() {
    if (blockchain_height != 0) {
        std::cout << "First block has already been created!\n";
        return;
    }

    std::vector<user> users;
    users = cached_users;

    long unsigned int seed = get_epoch_time();
    mt.seed(seed);
    std::uniform_real_distribution<double> amount_dist(100, 10000);

    for (user u:users) {
        std::string from = "Coinbase";
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

        cached_transactions[new_transaction.id] = new_transaction;

        auto receiver = std::find_if(cached_users.begin(),
                                    cached_users.end(),
                                    [&](const user& u) {
                                        return u.public_key == new_transaction.to;
                                    });
        receiver->utx_ids.push_back(transaction_id);
    }

    std::string prev_block_hash = hash256.hash("");
    long time = get_epoch_time();

    std::vector<std::string> tx_ids;
    for (auto t : cached_transactions) {
        transaction tx = t.second;
        tx_ids.push_back(tx.id);
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
    best_block = read_block(blockchain_height - 1);

    return best_block;
}

Blockchain::block Blockchain::read_block(const int& index) {
    std::stringstream buffer;
    std::string header;

    std::string file_type;
    std::string file_name;

    file_type = "#blockchain-data:block";
    file_name = blocks_folder + "/block" + std::to_string(index) + ".dat";

    std::ifstream file (file_name);
    std::getline(file, header);

    if (header.length()) {
        bool correct_file_type = ( header == file_type );

        if (!correct_file_type) {
            throw std::runtime_error("Incorrect file type");
        }
    } else {
        std::cout << "Block file not found\n";
    }

    buffer << file.rdbuf();
    file.close();

    std::string line;
    block b;

    getline(buffer, line, '~');
    b.hash = line;

    getline(buffer, line, '~');
    b.height = std::stoi(line);

    getline(buffer, line, '~');
    b.prev_block_hash = line;

    getline(buffer, line, '~');
    b.time = std::stol(line);

    getline(buffer, line, '~');
    b.version = line;

    getline(buffer, line, '~');
    b.merkleroot = line;

    getline(buffer, line, '~');
    b.nonce = std::stoull(line);

    getline(buffer, line, '~');
    b.difficulity_target = std::stoi(line);

    while (getline(buffer, line, '~')) {
        b.tx.push_back(line);
    }

    return b;
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

void Blockchain::print_block(const int& index) {
    block b = read_block(index);

    std::cout << "Hash: " << b.hash << "\n"
                << "Height: " << b.height << "\n"
                << "Previous block hash: " << b.prev_block_hash << "\n"
                << "Timestamp: " << b.time << "\n"
                << "Version: " << b.version << "\n"
                << "Merkleroot: " << b.merkleroot << "\n"
                << "Nonce: " << b.nonce << "\n"
                << "Difficulity: " << b.difficulity_target << "\n"
                << "Transactions: \n";
    for (auto tx:b.tx) {
        std::cout << "\t" << tx << "\n";
    }

    return;
}

// PRIVATE METHODS
