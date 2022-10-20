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

        // TODO destructor to write cached data to disk

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
        /**
         * @brief Create a new transaction and write its data to cache. In order to safe cached transactions to disk call write_transactions()
         * 
         * @param from Senders private key
         * @param to Receivers private key
         * @param amount Sending amount
         */
        void create_transaction(const std::string& from, const std::string& to, const double& amount);

        /**
         * @brief Create a new user and write its data to cache. In order to safe cached users to disk call write_users()
         * 
         * @param name Username
         */
        void create_user(const std::string& name);

        /**
         * @brief Write cahed transactions to disk
         * 
         */
        void write_transactions();

        /**
         * @brief Write cached user data to disk
         * 
         */
        void write_users();

        /**
         * @brief Get all transaction's
         * 
         * @return Vector of all unconfirmed transactions
         */
        std::vector<transaction> get_transactions();

        /**
         * @brief Get all user's
         * 
         * @return Vector of all users in a blockchain
         */
        std::vector<user> get_users();

    private:
        // VARIABLES
        Hash hash256;

        std::string data_folder = "./blockchain";
        std::string user_data_file = data_folder + "/users.dat";
        std::string unconfirmed_transaction_file = data_folder + "/transactions.dat";

        std::vector<user> cached_users;
        std::vector<transaction> cached_transactions;

        // METHODS

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