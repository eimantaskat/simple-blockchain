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
    Blockchain bc;

    // bc.create_user("u1");
    // bc.create_user("u2");

    // bc.create_block();
    // bc.mine_block();

    // auto u = bc.get_users();
    // std::cout << "u1 balance: " << bc.get_balance(u[0].public_key) << "\n";
    // std::cout << "u2 balance: " << bc.get_balance(u[1].public_key) << "\n";

    // bc.create_transaction(u[0].public_key, u[1].public_key, 1000);

    // bc.create_block();
    // bc.mine_block();
    
    // std::cout << "u1 balance: " << bc.get_balance(u[0].public_key) << "\n";
    // std::cout << "u2 balance: " << bc.get_balance(u[1].public_key) << "\n";

    // auto t = bc.get_user_transactions(u[1].public_key);
    // for (auto tx:t) {
    //     std::cout << tx.id << " " << tx.from << " " << tx.to << " " << tx.out[0].amount << " " << tx.out[0].unspent << " " << tx.out[1].amount << " " << tx.out[1].unspent << "\n";
    // }

    std::string input;
    while (true) {
        std::cout << "blockchain> ";
        std::cin >> input;

        if (input == "generateData") {
            generate_users(bc, 1000);
            generate_transactions(bc, 10000);
            bc.create_block();
            bc.mine_block();
            bc.write();
            std::cout << "Generated 1000 users, 10000 transactions and created block with initial user balance\n";
        } else if (input == "mineBlock") {
            bc.create_block();
            bc.mine_block();
            bc.write();
        } else if (input == "mineAllBlocks") {
            while (bc.get_unvalidated_transactions().size() > 0) {
                bc.create_block();
                bc.mine_block();
            }
            bc.write();
            std::cout << "No more transactions left\n";
        } else if (input == "showBlock") {
            int hash;
            std::cout << "Block id: ";
            std::cin >> hash;
            bc.print_block(hash);
        } else if (input == "showTransaction") {
            std::string hash;
            std::cout << "Transaction hash: ";
            std::cin >> hash;
            bc.print_transaction(hash);
        } else if (input == "quit") {
            break;
        } else if (input == "help") {
            std::cout << "Commands:\n"
                        << "\thelp\t\t\tShow this view\n"
                        << "\tgenerateData\t\tGenerate users and transactions\n"
                        << "\tmineBlock\t\tMine one block\n"
                        << "\tmineAllBlocks\t\tMine blocks until there are no transactions left\n"
                        << "\tshowBlock\t\tGet information about block\n"
                        << "\tshowTransaction\t\tGet information about transaction\n"
                        << "\tquit\t\t\tClose blockchain\n";
        } else {
            // TODO command to found
        }
    }
}