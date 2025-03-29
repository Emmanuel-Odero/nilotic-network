// include/wallet.h
#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <openssl/evp.h> // Updated include

class Wallet {
private:
    EVP_PKEY* keyPair; // Updated to EVP_PKEY
    std::string publicKey;
    std::string privateKey;

public:
    Wallet();
    ~Wallet();
    std::string getPublicKey() const;
    std::string signTransaction(const std::string& data) const;
};

#endif