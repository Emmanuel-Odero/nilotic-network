// include/blockchain.h
#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "block.h"
#include <vector>
#include <map>

enum class Network { Livewire, Testwire, Pipe };

class NiloticBlockchain {
private:
    std::vector<Block> chain;
    std::map<std::string, double> stakes;
    std::map<std::string, double> balances;
    Network network;
    const double TOTAL_SUPPLY = 555000000;
    const double PREMINED_PERCENT = 0.35;
    double currentSupply;
    double blockReward;
    std::string chainId;

    void initializeGenesis();

public:
    NiloticBlockchain(Network net = Network::Livewire);
    bool addStake(std::string address, double amount);
    bool removeStake(std::string address, double amount);
    bool validateTransaction(const Transaction& tx);
    void addBlock(std::vector<Transaction> txs, std::string validator);
    std::vector<Block> getChain() const;
    std::string getChainId() const;
    double getBalance(std::string address) const;
    double getStake(std::string address) const;
};

#endif