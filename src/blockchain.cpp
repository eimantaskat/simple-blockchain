#include "../include/blockchain.hpp"

// CONSTRUCTOR
Blockchain::Blockchain() {
    Hash hash256;

    CreateDirectory(data_folder.c_str(), NULL);
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

void Blockchain::write_transactions() {
    const std::string file_type = "#blockchain-data:transactions";

    std::stringstream buffer;
    std::string header;

    for (transaction t:cached_transactions) {
        buffer << t.id << "~"
            << t.from << "~"
            << t.to << "~"
            << t.amount << "~"
            << t.time << "~"
            << t.spent << "~";
    }

    std::fstream file (unconfirmed_transaction_file);
    std::getline(file, header);

    if (header.length()) {
        bool correct_file_type = ( header == file_type );

        if (!correct_file_type) {
            throw std::runtime_error("Incorrect file type");
        }
    } else {
        file.close();
        create_file("transactions", unconfirmed_transaction_file);
        file.open(unconfirmed_transaction_file);
    }

    file.seekp(0, std::ios_base::end);
    
    file << buffer.rdbuf();
}

void Blockchain::write_users() {
    const std::string file_type = "#blockchain-data:users";

    std::stringstream buffer;
    std::string header;

    for (user u:cached_users) {
        buffer << u.name << "~"
            << u.public_key << "~"
            << u.time_created << "~";
    }

    std::fstream file (user_data_file);
    std::getline(file, header);

    if (header.length()) {
        bool correct_file_type = ( header == file_type );

        if (!correct_file_type) {
            throw std::runtime_error("Incorrect file type");
        }
    } else {
        file.close();
        create_file("users", user_data_file);
        file.open(user_data_file);
    }

    file.seekp(0, std::ios_base::end);
    
    file << buffer.rdbuf();
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