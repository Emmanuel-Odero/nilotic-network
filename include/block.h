// include/block.h
#ifndef BLOCK_H
#define BLOCK_H

#include "transaction.h"
#include <string>
#include <vector>
#include <ctime>
#include <openssl/sha.h>

class Block {
public:
    std::string prevHash;
    std::vector<Transaction> transactions;
    time_t timestamp;
    std::string hash;
    int nonce; // For hybrid PoS mining
    std::string validator;

    Block(std::string prev, std::vector<Transaction> txs, std::string val);
    std::string calculateHash() const;
};

#endif