#include <iostream>

#include "../include/blockchain.hpp"


int main() {
    Blockchain bc;
    bc.create_user("Eimantas Katinas");

    std::vector<std::string> users = {"Ei", "Man", "Tas", "Ka", "Ti", "Nas"};

    for (auto u:users) {
        bc.create_user(u);
    }
    
    bc.write_users();

    bc.create_transaction("602ee1e365757877846b8d4db798589decd897a8a7d8e697e5fd8fa79d775a4d", "933303d1be310ee3d2e2187d676c9477a9764f89dd878bc6aafcff655786d7a8", 100);
    bc.write_transactions();

    std::vector<Blockchain::transaction> transactions = bc.get_transactions();
    for (auto t:transactions) {
        std::cout << t.id << " " << t.from << " " << t.to << " " << t.amount << " " << t.time << " " << t.spent << "\n";
    }
}