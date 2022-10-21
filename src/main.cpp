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
    bc.get_best_block();
    // generate_users(1000);

    // auto users = bc.get_users();
    // std::cout << users.size() << "\n";

    // generate_transactions(10000);

    // auto transactions = bc.get_transactions();
    // std::cout << transactions.size() << "\n";


    // bc.create_transaction("602ee1e365757877846b8d4db798589decd897a8a7d8e697e5fd8fa79d775a4d", "933303d1be310ee3d2e2187d676c9477a9764f89dd878bc6aafcff655786d7a8", 100);
    // bc.write_transactions();

    // std::vector<Blockchain::transaction> transactions = bc.get_transactions();
    // for (auto t:transactions) {
    //     std::cout << t.id << " " << t.from << " " << t.to << " " << t.amount << " " << t.time << " " << t.spent << "\n";
    // }
}