#include <iostream>
#include <string>
#include <random>

#include "../include/blockchain.hpp"

void generate_users(int n) {
    Blockchain bc;
    
    for (int i = 0; i < n; i++) {
        std::string username = "user" + std::to_string(i);

        bc.create_user(username);
    }

    bc.write_to_disk("users");
}

void generate_transactions(int n) {
    Blockchain bc;
    auto users = bc.get_users();

    std::mt19937 mt;
    long unsigned int seed = static_cast<long unsigned int>(hrClock::now().time_since_epoch().count());
    mt.seed(seed);
    std::uniform_int_distribution<int> users_dist(0, users.size());
    std::uniform_int_distribution<int> value_dist(0, 100000);
    
    for (int i = 0; i < n; i++) {
        int sender_index = users_dist(mt);
        int receiver_index = users_dist(mt);

        std::string sender = users[sender_index].public_key;
        std::string receiver = users[receiver_index].public_key;
        int amount = value_dist(mt);

        bc.create_transaction(sender, receiver, amount);
    }

    bc.write_to_disk("transactions");
}


int main() {
    Blockchain bc;
    generate_users(1000);
    
    bc.create_first_block();

    bc.mine_block();
}