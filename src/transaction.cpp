#include "../include/blockchain.hpp"

// PUBLIC METHODS

void Blockchain::create_transaction(const std::string& from, const std::string& to, const int& amount) {
    long current_time = get_epoch_time();

    
    std::string val_to_hash = from + to + std::to_string(amount) + std::to_string(current_time);
    std::string transaction_id = hash256.hash(val_to_hash);

    transaction new_transaction {transaction_id, from, to, amount, current_time};
    cached_unvalidated_transactions.push_back(new_transaction);
}

std::vector<Blockchain::transaction> Blockchain::get_unvalidated_transactions() {
    return cached_unvalidated_transactions;
}

std::unordered_map<std::string, Blockchain::transaction> Blockchain::get_transactions() {
    return cached_transactions;
}

void Blockchain::print_transaction(const std::string& id) {
    auto tx_it = cached_transactions.find(id);
    if (tx_it == cached_transactions.end()) {
        std::cout << "Transaction " << id << " does not exist!\n";
        return;
    }

    std::cout << "Id: " << tx_it->second.id << "\n"
                << "From: " << tx_it->second.from << "\n"
                << "To: " << tx_it->second.to << "\n"
                << "Amount: " << tx_it->second.amount << "\n"
                << "Time: " << tx_it->second.time << "\n\n"
                << "Inputs:\n";

    for (auto t:tx_it->second.in) {
        std::cout << "\ttransaction id: " << t.transaction_id << "\n"
                    << "\tto: " << t.to << "\n"
                    << "\tamount: " << t.amount << "\n"
                    << "\tunspent: " << t.unspent << "\n\n";
    }

    std::cout << "Outputs:\n";
    for (auto t:tx_it->second.out) {
        std::cout << "\ttransaction id: " << t.transaction_id << "\n"
                    << "\tto: " << t.to << "\n"
                    << "\tamount: " << t.amount << "\n"
                    << "\tunspent: " << t.unspent << "\n\n";
    }
    return;
}

void Blockchain::transaction_html(const std::string& id) {
    auto tx_it = cached_transactions.find(id);
    if (tx_it == cached_transactions.end()) {
        std::cout << "Transaction " << id << " does not exist!\n";
        return;
    }

    generate_transaction_html(tx_it->second);
}

// PRIVATE METHODS

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

        cached_transactions[t.id] = t;
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

std::stringstream Blockchain::generate_transactions_buffer() {
    std::stringstream buffer;

    for (auto pair:cached_transactions) {
        transaction t = pair.second;
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

void Blockchain::erase_transactions(const std::vector<std::string>& transactions) {
    int size = cached_unvalidated_transactions.size();
    for (auto tx_it = transactions.begin(); tx_it != transactions.end(); ++tx_it) {
        cached_unvalidated_transactions.erase(std::remove_if(
            cached_unvalidated_transactions.begin(), cached_unvalidated_transactions.end(),
            [&](const transaction& tx) { 
                return tx.id == *tx_it;
            }), cached_unvalidated_transactions.end());
    }
}

Blockchain::transaction Blockchain::verify_transaction(transaction current_tx) {
    // check if transaction has not been changed
    std::string val_to_hash = current_tx.from + current_tx.to + std::to_string(current_tx.amount) + std::to_string(current_tx.time);
    std::string hash = hash256.hash(val_to_hash);

    if (current_tx.id != hash) {
        return transaction{};
    }
    // find sender
    auto sender = std::find_if(cached_users.begin(),
                                    cached_users.end(),
                                    [&](const user& u) {
                                        return u.public_key == current_tx.from;
                                    });

    // find all transactions where sender participated
    auto senders_tx_ids = sender->utx_ids;

    std::vector<transaction> senders_tx;
    for (auto tx_ids_it = senders_tx_ids.begin(); tx_ids_it != senders_tx_ids.end(); ++tx_ids_it) {
        transaction tx;
        auto pair = cached_transactions.find(*tx_ids_it);
        if (pair != cached_transactions.end()) {
            tx = pair->second;
            senders_tx.push_back(tx);
        }
    }
    
    // loop through senders transactions
    int spent_tx_amount = 0;
    std::vector<std::vector<Blockchain::txo>::iterator> spent_transactions;
    for (auto it = senders_tx.begin(); it != senders_tx.end(); ++it) {
        // if he has enough money break the loop
        if (spent_tx_amount >= current_tx.amount) {
            break;
        }

        // loop through tx outputs
        for (auto txo_it = it->out.begin(); txo_it != it->out.end(); ++txo_it) {
            // if sender received money and that txo is unspent
            if ((txo_it->to == current_tx.from) && txo_it->unspent) {

                // find unspent transaction in confirmed tx pool
                auto input = cached_transactions.find(txo_it->transaction_id);

                // loop through its outputs
                for (auto input_txo_it = input->second.out.begin(); input_txo_it != input->second.out.end(); ++input_txo_it) {
                    // mark output that sender has received as spent
                    if (input_txo_it->to == current_tx.from) {
                        spent_transactions.push_back(input_txo_it);
                        input_txo_it->unspent = false;
                    }
                }
                
                // add spent txo to new transaction's input
                txo_it->unspent = false;
                current_tx.in.push_back(*txo_it);
                spent_tx_amount += txo_it->amount;
            }
        }
    }

    // if sender doesn't have enough money to complete this transaction
    if (spent_tx_amount < current_tx.amount) {
        // mark spent txo for this transaction as unspent
        for (auto it = spent_transactions.begin(); it != spent_transactions.end(); ++it) {
            (*it)->unspent = true;
        }
        // cancel transaction
        return transaction{};
    }

    int change = spent_tx_amount - current_tx.amount;
    
    // create utxos
    txo out0 {current_tx.id, current_tx.from, change, true};
    txo out1 {current_tx.id, current_tx.to, current_tx.amount, true};

    current_tx.out.push_back(out0);
    current_tx.out.push_back(out1);

    return current_tx;
}

void Blockchain::unspend_transactions(std::vector<transaction> validated_tx) {
    for (auto tx_it = validated_tx.begin(); tx_it != validated_tx.end(); ++tx_it) {
        for (auto in_tx_it = tx_it->in.begin(); in_tx_it != tx_it->in.end(); ++in_tx_it) {
            std::string tx_id = in_tx_it->transaction_id;
            std::string to = in_tx_it->to;

            auto pair = cached_transactions.find(tx_id);
            for (auto out_tx_it = pair->second.out.begin(); out_tx_it != pair->second.out.end(); ++out_tx_it) {
                if (out_tx_it->to == to) {
                    out_tx_it->unspent = true;
                }
            }
        }
    }
}

std::vector<std::string> Blockchain::select_random_transactions() {
    auto tx = cached_unvalidated_transactions;
    int size = tx.size();

    int tx_amount = max_block_tx;
    if (tx_amount > size) {
        tx_amount = size;
    }

    // Fisher-Yates shuffle
    std::vector<std::string> rand_tx;
    
    std::uniform_int_distribution<int> dist(0, INT_MAX);
    long unsigned int seed = static_cast<long unsigned int>(hrClock::now().time_since_epoch().count());
    mt.seed(seed);

    auto begin = tx.begin();
    auto end = tx.end();

    while (tx_amount > 0) {
        auto rand_iter = begin;
        std::advance(rand_iter, dist(mt) % size);
        rand_tx.push_back(rand_iter->id);
        begin++;
        --size;
        --tx_amount;
    }

    return rand_tx;
}