#include "../include/blockchain.hpp"

// PUBLIC METHODS

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