#include "../include/blockchain.hpp"

// PUBLIC METHODS

void Blockchain::create_user(const std::string& name) {
    long current_time = get_epoch_time();
    // FIXME if users with same username are created in the same second they get same public keys
    std::string val_to_hash = name + std::to_string(current_time);
    std::string public_key = hash256.hash(val_to_hash);

    user new_user {name, public_key, current_time};

    cached_users.push_back(new_user);
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

double Blockchain::get_balance(const std::string& public_key) {
    // TODO test this function
    std::vector<transaction> tx;
    tx = get_user_transactions(public_key);

    double balance = 0;

    for (transaction t:tx) {
        for (txo t_out:t.out) {
            if (t_out.to == public_key && t_out.unspent) {
                balance += t_out.amount;
            }
        }
    }

    return balance;
}

// PRIVATE METHODS
