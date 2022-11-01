#include <iostream>
#include <string>
#include <random>

#include "../include/blockchain.hpp"

#include <windows.h>
#include <stdio.h>

volatile bool stopped = false;

BOOL WINAPI HandlerRoutine(_In_ DWORD dwCtrlType) {
    std::cout.setstate(std::ios_base::failbit);
    switch (dwCtrlType) {
        case CTRL_C_EVENT:
            printf("\r\e[K\nCaught ctrl + c\n");
            stopped = true;
            return TRUE;
        default:
            return FALSE;
    }
}

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
    BOOL ctrl_handler_set = SetConsoleCtrlHandler(HandlerRoutine, TRUE);
    if (!ctrl_handler_set) {
        std::cout << "ERROR: Could not set control handler";
        return 1;
    }

    Blockchain bc;

    std::string input;
    while (true) {
        if (stopped) {
            break;
        }

        std::cout << "blockchain> ";
        std::cin >> input;

        if (input == "generateData") {
            generate_users(bc, 1000);
            generate_transactions(bc, 10000);
            bc.create_block();
            bc.write();
            std::cout << "Generated 1000 users, 10000 transactions and created block with initial user balance\n";
        } else if (input == "startMining") {
            bc.decentralized_mining();
        } else if (input == "mineBlock") {
            bc.create_block();
            bc.write();
        } else if (input == "mineAllBlocks") {
            while (bc.get_unvalidated_transactions().size() > 0) {
                bc.create_block();
            }
            bc.write();
            std::cout << "No more transactions left\n";
        } else if (input == "outputBlock") {
            int height;
            std::cin >> height;
            bc.block_html(height);
        } else if (input == "outputTransaction") {
            std::string hash;
            std::cout << "Transaction hash: ";
            std::cin >> hash;
            bc.transaction_html(hash);
        } else if (input == "showBlock") {
            int id;
            std::cout << "Block id: ";
            std::cin >> id;
            bc.print_block(id);
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
                        << "\tstartMining\t\tStart decentralized mining\n"
                        << "\tmineBlock\t\tMine one block\n"
                        << "\tmineAllBlocks\t\tMine blocks until there are no transactions left\n"
                        << "\toutputBlock <block height>\t\tOutput block info to HTML file\n"
                        << "\toutputTransaction\t\tOutput transaction info to HTML file\n"
                        << "\tshowBlock\t\tGet information about block\n"
                        << "\tshowTransaction\t\tGet information about transaction\n"
                        << "\tquit\t\t\tClose blockchain\n";
        } else if (input.size() > 0) {
            std::cout << "Command \"" << input << "\" not found\n"
                        << "Type \"help\" to list all available commands\n";
        }
    }
}