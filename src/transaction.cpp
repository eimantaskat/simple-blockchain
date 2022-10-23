#include "../include/blockchain.hpp"

// PUBLIC METHODS

void Blockchain::create_transaction(const std::string& from, const std::string& to, const double& amount) {
    long current_time = get_epoch_time();

    // TODO check if users exsist and have enough money to send
    // TODO if transactions with same sender, receiver and amount are created in the same second they get same id
    std::string val_to_hash = from + to + std::to_string(amount) + std::to_string(current_time);
    std::string transaction_id = hash256.hash(val_to_hash);

    // FIXME inputs and outputs
    // std::vector<transaction> txs = get_user_transactions(from, true);
    // std::vector<std::string> input_ids;
    // double input_amount = 0;
    // for (transaction tx:txs) {
    //     for (txo utxo:tx.out) {
    //         if (utxo.unspent && utxo.to == from) {
    //             txs.push_back(tx);
    //             input_ids.push_back(utxo.transaction_id);
    //             input_amount += utxo.amount;
    //         }
    //     }
    //     if (input_amount >= )
    // }

    transaction new_transaction {transaction_id, from, to, amount, current_time};

    cached_transactions.push_back(new_transaction);
}

std::vector<Blockchain::transaction> Blockchain::get_transactions(bool unvalidated) {
    std::string file_type;
    std::string file_name;

    if (unvalidated) {
        file_type = "#blockchain-data:unvalidated_transactions";
        file_name = unvalidated_transaction_file;
    } else {
        file_type = "#blockchain-data:transactions";
        file_name = transaction_file;
    }

    std::stringstream buffer;
    std::string header;

    std::ifstream file (file_name);
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
        // std::cout << "id " << line << "\n";
        t.id = line;

        getline(buffer, line, '~');
        // std::cout << "from " << line << "\n";
        t.from = line;

        getline(buffer, line, '~');
        // std::cout << "to " << line << "\n";
        t.to = line;

        getline(buffer, line, '~');
        // std::cout << "" << line << "\n";
        t.amount = std::stod(line);

        getline(buffer, line, '~');
        // std::cout << "" << line << "\n";
        t.time = std::stol(line);


        getline(buffer, line, '~');
        while (line != "|") {
            txo t_in;
            t_in.transaction_id = line;

            getline(buffer, line, '~');
            t_in.to = line;

            getline(buffer, line, '~');
            t_in.amount = std::stod(line);

            getline(buffer, line, '~');
            t_in.unspent = (line == "1");

            t.in.push_back(t_in);

            getline(buffer, line, '~');
        }

        getline(buffer, line, '~');
        while (line != "|") {
            txo t_out;
            t_out.transaction_id = line;

            getline(buffer, line, '~');
            t_out.to = line;

            getline(buffer, line, '~');
            t_out.amount = std::stod(line);

            getline(buffer, line, '~');
            t_out.unspent = (line == "1");

            t.out.push_back(t_out);

            getline(buffer, line, '~');
        }

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
        for (txo t_in:t.in) {
            buffer << t_in.transaction_id << "~"
                    << t_in.to << "~"
                    << t_in.amount << "~"
                    << t_in.unspent << "~";
        }
        buffer << "|~";
        for (txo t_out:t.out) {
            buffer << t_out.transaction_id << "~"
                    << t_out.to << "~"
                    << t_out.amount << "~"
                    << t_out.unspent << "~";
        }
        buffer << "|~";
    }

    cached_transactions.clear();

    return buffer;
}