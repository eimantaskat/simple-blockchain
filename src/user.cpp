#include "../include/blockchain.hpp"

// PUBLIC METHODS

void Blockchain::create_user(const std::string& name) {
    long current_time = get_epoch_time();
    std::string val_to_hash = name + std::to_string(current_time);
    std::string public_key = hash256.hash(val_to_hash);

    user new_user {name, public_key, current_time};

    cached_users.push_back(new_user);
    get_users();
}

std::vector<Blockchain::user> Blockchain::get_users() {
    return cached_users;
}

int Blockchain::get_balance(const std::string& public_key) {
    std::vector<transaction> tx;
    tx = get_user_transactions(public_key);

    int balance = 0;

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

std::vector<Blockchain::transaction> Blockchain::get_user_transactions(const std::string& public_key) {
    std::vector<transaction> user_transactions;
    
    auto tx = get_transactions();

    for (auto t:tx) {
        if (t.second.from == public_key || t.second.to == public_key) {
            user_transactions.push_back(t.second);
        }
    }

    return user_transactions;
}


void Blockchain::read_users() {
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
        std::cout << "Users file not found\n";
    }


    buffer << file.rdbuf();
    file.close();

    std::vector<user> users;
    std::string line;
    while (getline(buffer, line, '~')) {
        user u;
        u.name = line;

        getline(buffer, line, '~');
        u.public_key = line;

        getline(buffer, line, '~');
        u.time_created = std::stoul(line);

        getline(buffer, line, '~');
        while (line != "|") {
            u.utx_ids.push_back(line);
            getline(buffer, line, '~');
        }
        
        cached_users.push_back(u);
    }
}

std::stringstream Blockchain::generate_users_buffer() {
    std::stringstream buffer;

    for (user u:cached_users) {
        buffer << u.name << "~"
            << u.public_key << "~"
            << u.time_created << "~";

        for (std::string t_id:u.utx_ids) {
            buffer << t_id << "~";
        }
        buffer << "|~";
    }

    return buffer;
}