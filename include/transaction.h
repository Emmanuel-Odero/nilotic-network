// include/transaction.h
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>
#include <openssl/sha.h>

class Transaction {
public:
    std::string sender;
    std::string receiver;
    double amount; // In Lut (smallest unit of SLW)
    std::string signature;
    time_t timestamp;

    Transaction(std::string s, std::string r, double a);
    std::string hash() const;
};

#endif