#include "../include/blockchain.hpp"


// PUBLIC METHODS

void Blockchain::create_block(bool limit_guesses, const int& max_guesses) {
    if (blockchain_height == 0) {
        create_first_block();
        return;
    }

    block prev_block = get_best_block();

    std::string prev_block_hash = prev_block.hash;
    long time = get_epoch_time();

    std::vector<std::string> tx = select_random_transactions();

    std::string merkleroot = get_merkleroot(tx);

    block new_block;
    new_block.height = blockchain_height;
    new_block.prev_block_hash = prev_block_hash;
    new_block.time = time;
    new_block.version = blockchain_version;
    new_block.difficulity_target = difficulity_target;
    new_block.tx = tx;

    mine_block(new_block, limit_guesses, max_guesses);
}

Blockchain::block Blockchain::get_best_block() {
    block best_block;
    best_block = read_block(blockchain_height - 1);

    return best_block;
}

Blockchain::block Blockchain::get_block(const int& height) {
    return read_block(height);
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

void Blockchain::mine_block(block mineable_block, bool limit_guesses, const int& max_guesses) {
    std::vector<transaction> validated_tx;
    validated_tx.reserve(max_block_tx);

    std::vector<std::string> transactions;
    transactions = mineable_block.tx;

    if (mineable_block.height != 0) {
        for (auto it = mineable_block.tx.begin(); it != mineable_block.tx.end(); ++it) {
            auto current_tx_it = std::find_if(cached_unvalidated_transactions.begin(),
                                                    cached_unvalidated_transactions.end(),
                                                    [&](const transaction& t) {
                                                        return t.id == *it;
                                                    });
            std::string val_to_hash = current_tx_it->from + current_tx_it->to + std::to_string(current_tx_it->amount) + std::to_string(current_tx_it->time);
            std::string hash = hash256.hash(val_to_hash);

            transaction validated = verify_transaction(current_tx_it);
            
            if (validated.id.size() != 0) {
                validated_tx.push_back(validated);
            }
        }
    }

    std::string prev_block_hash = mineable_block.prev_block_hash;
    std::string timestamp = std::to_string(mineable_block.time);
    std::string version = mineable_block.version;

    std::vector<std::string> tmp;
    for (auto tx_it = validated_tx.begin(); tx_it != validated_tx.end(); ++tx_it) {
        // tmp.push_back(hash256.hash(tx_it->id));
        tmp.push_back(tx_it->id);
    }
    std::string merkleroot = get_merkleroot(tmp);
    mineable_block.tx = tmp;

    int diff_target = mineable_block.difficulity_target;
    std::string difficulity_target = std::to_string(diff_target);

    std::string padding (diff_target, '0');
    
    std::string hash;
    for (unsigned long long nonce = 0; ; ++nonce) {
        std::string str_nonce = std::to_string(nonce);

        std::string value_to_hash = prev_block_hash + timestamp + version + merkleroot + str_nonce + difficulity_target;
        hash = hash256.hash(value_to_hash);
        if (hash.substr(0, diff_target) == padding) {
            mineable_block.nonce = nonce;
            break;
        }
        if (limit_guesses && nonce == max_guesses) {
            std::cout << "Failed to mine block\n";
            
            return;
        }
        if (nonce == ULLONG_MAX) {
            mineable_block.time = get_epoch_time();
            timestamp = std::to_string(mineable_block.time);
            nonce = 0;
        }
    }

    std::cout << "Block " << blockchain_height << " containing " << validated_tx.size() << " transactions mined! Hash: " << hash << "\n";
    mineable_block.merkleroot = merkleroot;
    mineable_block.hash = hash;
    mined_block = mineable_block;

    if (mined_block.height != 0) {
        erase_transactions(transactions);
    }

    for (auto tx_it = validated_tx.begin(); tx_it != validated_tx.end(); ++tx_it) {
        auto sender = std::find_if(cached_users.begin(),
                                    cached_users.end(),
                                    [&](const user& u) {
                                        return u.public_key == tx_it->from;
                                    });

        auto receiver = std::find_if(cached_users.begin(),
                                        cached_users.end(),
                                        [&](const user& u) {
                                            return u.public_key == tx_it->to;
                                        });

        std::string current_tx_id;
        current_tx_id = tx_it->id;

        sender->utx_ids.push_back(current_tx_id);
        receiver->utx_ids.push_back(current_tx_id);

        cached_transactions.emplace(current_tx_id, *tx_it);
        cached_transactions.emplace(tx_it->id, *tx_it);
    }

    write_to_disk("block");
    write_to_disk("transactions");
    write_to_disk("unvalidated_transactions");
    
    blockchain_height++;
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
    std::uniform_real_distribution<double> amount_dist(1000, 10000);

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

    mine_block(new_block);
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