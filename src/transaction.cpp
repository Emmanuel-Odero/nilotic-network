// src/transaction.cpp
#include "transaction.h"

Transaction::Transaction(std::string s, std::string r, double a)
    : sender(s), receiver(r), amount(a), timestamp(std::time(nullptr)) {}

std::string Transaction::hash() const {
    std::string data = sender + receiver + std::to_string(amount) + std::to_string(timestamp);
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash);
    return std::string(reinterpret_cast<char*>(hash), SHA256_DIGEST_LENGTH);
}