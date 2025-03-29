// src/blockchain.cpp
#include "blockchain.h"

void NiloticBlockchain::initializeGenesis() {
    std::string validator = (network == Network::Livewire) ? "livewire_genesis" : 
                           (network == Network::Testwire) ? "testwire_genesis" : "pipe_genesis";
    std::vector<Transaction> genesisTxs = {Transaction("genesis", "network", TOTAL_SUPPLY * PREMINED_PERCENT)};
    Block genesisBlock("", genesisTxs, validator);
    chain.push_back(genesisBlock);
    currentSupply = TOTAL_SUPPLY * PREMINED_PERCENT;
    balances["network"] = TOTAL_SUPPLY * PREMINED_PERCENT;
}

NiloticBlockchain::NiloticBlockchain(Network net) : network(net), blockReward(5.0) { // Fixed reward of 5 SLW
    chainId = (net == Network::Livewire) ? "NIL-LW" : (net == Network::Testwire) ? "NIL-TW" : "NIL-PP";
    initializeGenesis();
}

bool NiloticBlockchain::addStake(std::string address, double amount) {
    if (balances[address] >= amount && amount > 0) {
        balances[address] -= amount;
        stakes[address] += amount;
        return true;
    }
    return false;
}

bool NiloticBlockchain::removeStake(std::string address, double amount) {
    if (stakes[address] >= amount && amount > 0) {
        stakes[address] -= amount;
        balances[address] += amount;
        return true;
    }
    return false;
}

bool NiloticBlockchain::validateTransaction(const Transaction& tx) {
    return tx.amount > 0 && balances[tx.sender] >= tx.amount;
}

void NiloticBlockchain::addBlock(std::vector<Transaction> txs, std::string validator) {
    if (stakes[validator] <= 0 && chain.size() > 1) return; // Allow bootstrap
    Block newBlock(chain.back().hash, txs, validator);
    chain.push_back(newBlock);
    stakes[validator] += blockReward; // Adds 5 SLW
    currentSupply += blockReward;
    // Optional halving logic if desired
}

std::vector<Block> NiloticBlockchain::getChain() const { return chain; }
std::string NiloticBlockchain::getChainId() const { return chainId; }
double NiloticBlockchain::getBalance(std::string address) const { return balances.count(address) ? balances.at(address) : 0.0; }
double NiloticBlockchain::getStake(std::string address) const { return stakes.count(address) ? stakes.at(address) : 0.0; }