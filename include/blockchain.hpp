#pragma once

#include <vector>
#include <chrono>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <fileapi.h>
#include <thread>
#include <sys/stat.h>
#include <random>
#include <cstdio>

#include <iostream>

#include "hash.hpp"
#include "SHA256.hpp"

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
         * @brief Transaction output data structure
         * 
         */
        struct txo {
            std::string transaction_id;
            std::string to;
            double amount;
            bool unspent = true;
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
            std::vector<txo> in;
            std::vector<txo> out;
        };

        /**
         * @brief Block data structure
         * 
         */
        struct block {
            std::string hash;
            int height;
            std::string prev_block_hash;
            long time;
            std::string version;
            std::string merkleroot;
            unsigned long long nonce;
            int difficulity_target;
            std::vector<std::string> tx;
        };

        // METHODS

        void mine_block();

        /**
         * @brief Create a new transaction and write its data to cache. In order to safe cached transactions to disk call write_to_disk("transactions")
         * 
         * @param from Senders private key
         * @param to Receivers private key
         * @param amount Sending amount
         */
        void create_transaction(const std::string& from, const std::string& to, const double& amount);

        /**
         * @brief Create a new user and write its data to cache. In order to safe cached users to disk call write_to_disk("users")
         * 
         * @param name Username
         */
        void create_user(const std::string& name);

        /**
         * @brief Create a new block
         * 
         * @param transactions Vector of transactions to put in a block
         */
        void create_block(const std::vector<transaction>& transactions);

        // TODO docstring
        void create_first_block();

        /**
         * @brief Generate transactions's buffer to write to file
         * 
         * @return Transactions's stringstream buffer
         */
        std::stringstream generate_transactions_buffer();

        /**
         * @brief Generate users's buffer to write to file
         * 
         * @return Users's stringstream buffer
         */
        std::stringstream generate_users_buffer();

        /**
         * @brief Generate block's that needs to be mined buffer to write to file
         * 
         * @return Block's stringstream buffer
         */
        std::stringstream generate_block_to_mine_buffer();

        /**
         * @brief 
         * 
         * @return std::stringstream 
         */
        std::stringstream generate_block_buffer();

        /**
         * @brief Write selected cached data to disk
         * 
         * @param data_type Type of data to write to disk. Possible types: block, transactions, users, block_to_mine
         */
        void write_to_disk(std::string data_type);

        /**
         * @brief Get all transaction's
         * 
         * @return Vector of all unconfirmed transactions
         */
        std::vector<transaction> get_transactions();

        /**
         * @brief Get all transactions user has received
         * 
         * @param public_key User's public key
         * @param unspent If true, returns only unspent transactions
         * @return Vector of all transactions that given user has received
         */
        std::vector<transaction> get_user_transactions(const std::string& public_key, bool unspent = false);

        /**
         * @brief Get all user's
         * 
         * @return Vector of all users in a blockchain
         */
        std::vector<user> get_users();
        
        /**
         * @brief Get the last block from the longest chain
         * 
         * @return Last added block 
         */
        block get_best_block();

        /**
         * @brief Get the block to mine
         * 
         * @return Block that needs to be mined 
         */
        block get_block_to_mine();

    private:
        // VARIABLES
        Hash hash256;
        std::mt19937 mt;

        std::string data_folder = "./blockchain";
        std::string blocks_folder = data_folder + "/bloks";
        std::string user_data_file = data_folder + "/users.dat";
        std::string unconfirmed_transaction_file = data_folder + "/transactions.dat";
        std::string block_to_mine_file = data_folder + "/block_to_mine.dat";

        block block_to_mine;
        block mined_block;
        
        std::vector<user> cached_users;
        std::vector<transaction> cached_transactions;

        std::string blockchain_version = "v0.1";
        int difficulity_target = 4;
        int blockchain_height = 0;

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

        /**
         * @brief Get transaction's merklroot hash
         * 
         * @param transactions Vector of transactions
         * @return std::string merkleroot hash
         */
        std::string get_merkleroot(std::vector<std::string> transactions);

        /**
         * @brief Method to check if file exists
         * 
         * @param name Name of file
         * @return true if file exists
         * @return false if file does not exist
         */
        bool file_exists(const std::string& name);
};