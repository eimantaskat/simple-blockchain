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

    std::string file_type;
    std::string file_name;

    file_type = "#blockchain-data:transactions";
    file_name = transaction_file;

    std::ifstream file (file_name);
    std::getline(file, header);

    if (header.length()) {
        bool correct_file_type = ( header == file_type );

        if (!correct_file_type) {
            throw std::runtime_error("Incorrect file type");
        }
    } else {
        std::cout << "Transactions file not found\n";
    }

    buffer << file.rdbuf();
    file.close();

    std::vector<transaction> transactions;
    std::string line;
    while (getline(buffer, line, '~')) {
        transaction t;
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

        cached_transactions.push_back(t);
    }
}

void Blockchain::read_unvalidated_transactions() {
    std::stringstream buffer;
    std::string header;

    std::string file_type;
    std::string file_name;

    file_type = "#blockchain-data:unvalidated_transactions";
    file_name = unvalidated_transaction_file;

    std::ifstream file (file_name);
    std::getline(file, header);

    if (header.length()) {
        bool correct_file_type = ( header == file_type );

        if (!correct_file_type) {
            throw std::runtime_error("Incorrect file type");
        }
    } else {
        std::cout << "Unvalidated transactions file not found\n";
    }

    buffer << file.rdbuf();
    file.close();

    std::vector<transaction> transactions;
    std::string line;
    while (getline(buffer, line, '~')) {
        transaction t;
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

        cached_unvalidated_transactions.push_back(t);
    }
}

std::vector<Blockchain::transaction> Blockchain::get_unvalidated_transactions() {
    return cached_unvalidated_transactions;
}

std::vector<Blockchain::transaction> Blockchain::get_transactions() {
    return cached_transactions;
}

std::stringstream Blockchain::generate_unvalidated_transactions_buffer() {
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
    auto current_tx_it = std::find_if(cached_unvalidated_transactions.begin(),
                                            cached_unvalidated_transactions.end(),
                                            [&](const transaction& t) {
                                                return t.id == tx_id;
                                            });

    std::vector<transaction> senders_tx;
    std::copy_if(cached_transactions.begin(), cached_transactions.end(), std::back_inserter(senders_tx), 
                                            [&](transaction tx) {
                                                for (txo t:tx.out) {
                                                    if ((t.to == current_tx_it->from) && t.unspent) {
                                                        return true;
                                                    }
                                                }
                                                return false;
                                            });

    double spent_tx_amount = 0;
    for (auto it = senders_tx.begin(); it != senders_tx.end(); ++it) {
        if (spent_tx_amount >= current_tx_it->amount) {
            break;
        }

        for (auto txo_it = it->out.begin(); txo_it != it->out.end(); ++txo_it) {
            if ((txo_it->to == current_tx_it->from) && txo_it->unspent) {
                // mark input utxo as spent
                auto input_it = std::find_if(cached_transactions.begin(),
                                            cached_transactions.end(),
                                            [&](const transaction& t) {
                                                return t.id == txo_it->transaction_id;
                                            });
                for (auto input_txo_it = input_it->out.begin(); input_txo_it != input_it->out.end(); ++input_txo_it) {
                    input_txo_it->unspent = false;
                }
                spent_tx_amount += txo_it->amount;

                current_tx_it->in.push_back(*txo_it);
            }
        }
    }

    double change = spent_tx_amount - current_tx_it->amount;
    
    // create utxos
    txo out0 {current_tx_it->id, current_tx_it->from, change};
    txo out1 {current_tx_it->id, current_tx_it->to, current_tx_it->amount};

    current_tx_it->out.push_back(out0);
    current_tx_it->out.push_back(out1);

    cached_transactions.push_back(*current_tx_it);
}

void Blockchain::print_transaction(const std::string& id) {
    for(std::vector<transaction>::iterator it = cached_transactions.begin(); it != cached_transactions.end(); ++it) {
        if (it->id == id) {
            std::cout << "Id: " << it->id << "\n"
                        << "From: " << it->from << "\n"
                        << "To: " << it->to << "\n"
                        << "Amount: " << it->amount << "\n"
                        << "Time: " << it->time << "\n"
                        << "In: " << it->in.size() << "\n"
                        << "Out: " << it->out.size() << "\n";

            for (auto t:it->out) {
                std::cout << t.amount << " ";
            }
            return;
        }
    }
}