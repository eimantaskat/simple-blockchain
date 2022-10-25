#include <iostream>
#include <string>
#include <random>

#include "../include/blockchain.hpp"

void generate_users(Blockchain &bc, int n) {
    for (int i = 0; i < n; i++) {
        std::string username = "user" + std::to_string(i);

        bc.create_user(username);
    }
}

void generate_transactions(Blockchain &bc, int n) {
    auto users = bc.get_users();

    std::mt19937 mt;
    long unsigned int seed = static_cast<long unsigned int>(hrClock::now().time_since_epoch().count());
    mt.seed(seed);
    std::uniform_int_distribution<int> users_dist(0, users.size()-1);
    std::uniform_real_distribution<double> value_dist(10, 1000);
    
    for (int i = 0; i < n; i++) {
        int sender_index = users_dist(mt);
        int receiver_index = users_dist(mt);

        std::string sender = users[sender_index].public_key;
        std::string receiver = users[receiver_index].public_key;
        double amount = value_dist(mt);

        bc.create_transaction(sender, receiver, amount);
    }
}

int main() {
    auto start = hrClock::now();

    Blockchain bc;
    generate_users(bc, 1000);
    auto u = bc.get_users();
    std::cout << u.size() << "\n";
    generate_transactions(bc, 10000);

    auto transactions = bc.get_unvalidated_transactions();
    // std::cout << transactions.size() << "\n";

    bc.create_block();
    bc.mine_block();

    transactions = bc.get_unvalidated_transactions();
    std::cout << transactions.size() << "\n";
    
    bc.create_block();
    bc.mine_block();

    transactions = bc.get_unvalidated_transactions();
    std::cout << transactions.size() << "\n";

    // bc.create_block();
    // bc.mine_block();

    // transactions = bc.get_unvalidated_transactions();
    // std::cout << transactions.size() << "\n";
    // transactions = bc.get_transactions();
    // std::cout << transactions.size() << "\n";


    // bc.end();
}