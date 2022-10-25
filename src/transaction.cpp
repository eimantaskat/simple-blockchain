#include "../include/blockchain.hpp"

// PUBLIC METHODS

void Blockchain::create_transaction(const std::string& from, const std::string& to, const double& amount) {
    long current_time = get_epoch_time();

    // TODO check if users exsist and have enough money to send
    // TODO if transactions with same sender, receiver and amount are created in the same second they get same id
    std::string val_to_hash = from + to + std::to_string(amount) + std::to_string(current_time);
    std::string transaction_id = hash256.hash(val_to_hash);

    transaction new_transaction {transaction_id, from, to, amount, current_time};

    cached_unvalidated_transactions.push_back(new_transaction);
}

void Blockchain::read_transactions() {
    std::stringstream buffer;
    std::string header;

    for (int i = 0; i < 2; i++) {
        std::string file_type;
        std::string file_name;

        if (i == 0) {
            file_type = "#blockchain-data:unvalidated_transactions";
            file_name = unvalidated_transaction_file;
        } else {
            file_type = "#blockchain-data:transactions";
            file_name = transaction_file;
        }

        std::ifstream file (file_name);
        std::getline(file, header);

        // if (header.length()) {
        //     bool correct_file_type = ( header == file_type );

        //     if (!correct_file_type) {
        //         std::cout << file_name << " " << header << " " << file_type << "\n";
        //         throw std::runtime_error("Incorrect file type");
        //     }
        // } else {
        //     throw std::runtime_error("File not found");
        // }

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

            if (i == 0) {
                cached_unvalidated_transactions.push_back(t);
            } else {
                cached_transactions.push_back(t);
            }
        }
    }
}

std::vector<Blockchain::transaction> Blockchain::get_unvalidated_transactions() {
    return cached_unvalidated_transactions;
}

std::vector<Blockchain::transaction> Blockchain::get_transactions() {
    return cached_transactions;
}

std::stringstream Blockchain::generate_transactions_buffer() {
    std::stringstream buffer;

    for (transaction t:cached_unvalidated_transactions) {
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

    return buffer;
}

void Blockchain::trunc_unvalidated_transactions_file(const int& size) {
    cached_unvalidated_transactions.resize(size);
    // std::stringstream buffer;

    // std::fstream file (unvalidated_transaction_file, std::fstream::in);
    // buffer << file.rdbuf();
    // file.close();

    // std::string file_contents;
    // std::string line;
    // std::string prev_line;
    // int tx_no = 0;

    // while (tx_no < size) {
    //     getline(buffer, line, '~');
    //     file_contents += line + "~";

    //     if (prev_line == "|" && line == "|") {
    //         tx_no++;
    //     }

    //     prev_line = line;
    // }

    // file.open(unvalidated_transaction_file, std::fstream::out);
    // file << file_contents;
    // std::cout << "done\n";
}

void Blockchain::complete_transaction(std::string tx_id) {
    
}