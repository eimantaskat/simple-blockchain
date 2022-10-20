#include <iostream>

#include "../include/blockchain.hpp"


int main() {
    Blockchain bc;
    bc.create_user("Eimantas Katinas");

    std::vector<std::string> users = {"Ei", "Man", "Tas", "Ka", "Ti", "Nas"};

    bc.create_user(users);

    std::vector<Blockchain::user> u = bc.get_users();
    std::cout << u.size();
}