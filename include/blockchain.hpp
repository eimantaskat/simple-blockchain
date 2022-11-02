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
#include <algorithm>
#include <cstdio>
#include <unordered_map>
#include <utility>
#include <ctime>
#include <iostream>
#include <windows.h>

#include "hash.hpp"

using hrClock = std::chrono::high_resolution_clock;

class Blockchain {
    public:
        /**
         * @brief Construct a new Blockchain:: Blockchain object
         * 
         */
        Blockchain();

        /**
         * @brief Destroy the Blockchain object
         * 
         */
        ~Blockchain();

        // DATA STRUCTURES
        /**
         * @brief Transaction output data structure
         * 
         */
        struct txo {
            std::string transaction_id;
            std::string to;
            int amount;
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
            int amount;
            long time;
            std::vector<txo> in;
            std::vector<txo> out;
        };

        /**
         * @brief User data structure
         * 
         */
        struct user {
            std::string name;
            std::string public_key;
            long time_created;
            std::vector<std::string> utx_ids;
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
        /**
         * @brief Writes all cached data into the disk
         * 
         */
        void write();

        /**
         * @brief Simulate "decentralized" mining
         * 
         * @param max_guesses Amount of hash guesses each miner gets. Defaults to 20,000
         * @param miners Amount of miners to simualte. Defaults to 5
         */
        void decentralized_mining(const int& max_guesses = 20000, const int& miners = 5);


        // BLOCK
        /**
         * @brief Create a new block if there are any unconfirmed transactions
         * 
         */
        bool create_block(bool limit_guesses = false, const int& max_guesses = 20000);

        /**
         * @brief Get the last block from the longest chain
         * 
         * @return Last added block 
         */
        block get_best_block();

        /**
         * @brief Get block by its height
         * 
         * @param height Block's height
         * @return Block 
         */
        block get_block(const int& height);

        /**
         * @brief Print block info to console
         * 
         * @param height Block height
         */
        void print_block(const int& height);

        /**
         * @brief Generate HTML file containing block info
         * 
         * @param height Block height
         */
        void block_html(const int& height);

        inline int get_blockchain_height() { return blockchain_height; }

        // TRANSACTION
        /**
         * @brief Create a new transaction and write its data to cache. In order to safe cached transactions to disk call write_to_disk("transactions")
         * 
         * @param from Senders private key
         * @param to Receivers private key
         * @param amount Sending amount
         */
        void create_transaction(const std::string& from, const std::string& to, const int& amount);
        
        /**
         * @brief Get all unvalidated transaction's
         * 
         * @return Vector of unvalidated transactions
         */
        std::vector<transaction> get_unvalidated_transactions();

        /**
         * @brief Get all validated transaction's
         * 
         * @return Unordered map of validated transactions
         */
        std::unordered_map<std::string, transaction> get_transactions();

        /**
         * @brief Print transaction info to console
         * 
         * @param id Transacion id
         */
        void print_transaction(const std::string& id);

        /**
         * @brief Generate HTML file containing transaction info
         * 
         * @param id Transaction id
         */
        void transaction_html(const std::string& id);


        // USER
        /**
         * @brief Create a new user and write its data to cache. In order to safe cached users to disk call write_to_disk("users")
         * 
         * @param name Username
         */
        void create_user(const std::string& name);

        /**
         * @brief Get all users
         * 
         * @return Vector of users
         */
        std::vector<user> get_users();

        /**
         * @brief Get user's account balance
         * 
         * @param public_key User's public key
         * @return User's balance
         */
        int get_balance(const std::string& public_key);

    private:
        // VARIABLES
        Hash hash256;
        std::mt19937 mt;

        std::string data_folder = "./blockchain";
        std::string blocks_folder = data_folder + "/bloks";
        std::string user_data_file = data_folder + "/users.dat";
        std::string unvalidated_transaction_file = data_folder + "/unvalidated_transactions.dat";
        std::string transaction_file = data_folder + "/transactions.dat";
        std::string html_block_file = "./block.html";
        std::string html_transaction_file = "./transaction.html";

        block mined_block;
        
        std::vector<user> cached_users;
        std::vector<transaction> cached_unvalidated_transactions;
        std::unordered_map<std::string, transaction> cached_transactions;

        std::string blockchain_version = "v0.2";
        int difficulity_target = 4;
        int blockchain_height = 0;
        int max_block_tx = 100;

        // METHODS
        /**
         * @brief Write selected cached data to disk
         * 
         * @param data_type Type of data to write to disk. Possible types: block, transactions, unvalidated_transactions, users, block_to_mine
         */
        void write_to_disk(std::string data_type);

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
         * @return true if file exists false otherwise
         */
        bool file_exists(const std::string& name);

        /**
         * @brief Convert epocth time to date string
         * 
         * @param time Epoch time
         * @return Date string
         */
        std::string epoch_to_date(long time);

        /**
         * @brief Convert seconds to days/hours/minutes/seconds
         * 
         * @param distance Amount o seconds
         * @return Converted string
         */
        std::string time_distance_to_str(long distance);

        // BLOCK
        /**
         * @brief Mine given block
         * 
         * @param mineable_block Block to mine
         * @param limit_guesses If true, limits amount of hash guesses. Defaults to false
         * @param max_guesses The limit of guesses if amount of hash guesses is limited. Defaults to 20,000
         */
        bool mine_block(block mineable_block, bool limit_guesses = false, const int& max_guesses = 20000);

        /**
         * @brief Create a first block in a blockchain and gives every user starting balance
         * 
         */
        bool create_first_block();

        /**
         * @brief Read block from file
         * 
         * @param height Block height
         * @return Block 
         */
        block read_block(const int& height);

        /**
         * @brief Generate block buffer to write to file
         * 
         * @return Block's stringstream buffer
         */
        std::stringstream generate_block_buffer();


        // TRANSACTION
        /**
         * @brief Read validated transactions from disk to cache
         * 
         */
        void read_transactions();

        /**
         * @brief Read unvalidated transacctions from disk to cache
         * 
         */
        void read_unvalidated_transactions();

        /**
         * @brief Generate transactions' buffer to write to file
         * 
         * @return Transactions' stringstream buffer
         */
        std::stringstream generate_transactions_buffer();

        /**
         * @brief Generate unvalidated transactions' buffer to write to file
         * 
         * @return Unavlidated transactions' stringstream buffer
         */
        std::stringstream generate_unvalidated_transactions_buffer();

        /**
         * @brief Erase given transactions from unvalidated transactions pool
         * 
         * @param transactions Vector of transaction ids
         */
        void erase_transactions(const std::vector<std::string>& transactions);

        /**
         * @brief Check if sender has enough money to send and if transaction hash is correct
         * 
         * @param current_tx_it Iterator pointing to a transaction to verify
         * @return Transaction if it is valid, empty transaction otherwise
         */
        transaction verify_transaction(transaction current_tx);

        void unspend_transactions(std::vector<transaction> validated_tx);

        /**
         * @brief Select random transaction to put in a block
         * 
         * @return Vector of selected transactions' ids
         */
        std::vector<std::string> select_random_transactions();


        // USER
        /**
         * @brief Get all transactions user has sent or received
         * 
         * @param public_key User's public key
         * @return Vector of all transactions that given user has sent/received
         */
        std::vector<transaction> get_user_transactions(const std::string& public_key);

        /**
         * @brief Read users from disk
         * 
         * @return Vector of all users in a blockchain
         */
        void read_users();

        /**
         * @brief Generate users's buffer to write to file
         * 
         * @return Users's stringstream buffer
         */
        std::stringstream generate_users_buffer();


        // HTML
        /**
         * @brief Generate HTML file containing information about given block
         * 
         * @param b Block
         */
        void generate_block_html(block b);

        /**
         * @brief Generate HTML file containing information about given tranasction
         * 
         * @param tx Transaction
         */
        void generate_transaction_html(transaction tx);
};