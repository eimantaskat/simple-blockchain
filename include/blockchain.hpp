#pragma once

#include <vector>
#include <chrono>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <fileapi.h>
#include <thread>

#include "hash.hpp"

using hrClock = std::chrono::high_resolution_clock;

class Blockchain {
    public:
        /**
         * @brief Construct a new Blockchain:: Blockchain object
         * 
         */
        Blockchain();

        // DATA STRUCTURES
        /**
         * @brief User data structure
         * 
         */
        struct user {
            std::string name;
            std::string public_key;
            long time_created;
        };

        /**
         * @brief Transaction data structure
         * 
         */
        struct transaction {
            std::string id;
            std::string from;
            std::string to;
            double amount;
            long time;
            bool spent = false;
        };

        // METHODS
        void create_transaction(const std::string& from, const std::string& to, const double& amount);

        /**
         * @brief Function to create new user
         * 
         * @param name Username
         */
        void create_user(const std::string& name);

        /**
         * @brief Function to create new user
         * 
         * @param names Vector of usernames
         */
        void create_user(const std::vector<std::string>& names);

        /**
         * @brief Get all user data
         * 
         * @param file_name 
         * @return Vector of all users in a blockchain
         */
        std::vector<user> get_users();

    private:
        // VARIABLES
        Hash hash256;

        std::string data_folder = "./blockchain";
        std::string user_data_file = data_folder + "/users.dat";

        std::vector<user> cached_users;

        // METHODS
        /**
         * @brief Write user data to file
         * 
         * @param file_name File to write user data to
         * @param u User to write to file
         */
        void write_user(const std::string& file_name, const user& u);

        /**
         * @brief Write user data to file
         * 
         * @param file_name File to write user data to
         * @param users Vector of users to write to file
         */
        void write_user(const std::string& file_name, const std::vector<user>& users);

        /**
         * @brief Creates blockchain data file of given type
         * 
         * @param name file_type File type to create. Possible types: "user", "block", "transaction"
         * @param file_name Created file name
         */
        void create_file(std::string file_type, std::string file_name);

        /**
         * @brief Method to get current time
         * 
         * @return Epoch time in seconds
         */
        long get_epoch_time();
};