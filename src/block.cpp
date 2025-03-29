// src/block.cpp
#include "block.h"

Block::Block(std::string prev, std::vector<Transaction> txs, std::string val)
    : prevHash(prev), transactions(txs), timestamp(std::time(nullptr)), nonce(0), validator(val) {
    hash = calculateHash();
}

std::string Block::calculateHash() const {
    std::string data = prevHash + std::to_string(timestamp) + std::to_string(nonce) + validator;
    for (const auto& tx : transactions) {
        data += tx.hash();
    }
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash);
    return std::string(reinterpret_cast<char*>(hash), SHA256_DIGEST_LENGTH);
}