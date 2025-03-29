// include/smart_contract.h
#ifndef SMART_CONTRACT_H
#define SMART_CONTRACT_H

#include "blockchain.h"
#include <string>
#include <map>

class SmartContract {
private:
    std::string code; // Script code (e.g., "TRANSFER 5 FROM caller TO bob IF balance >= 5")
    std::map<std::string, double> state; // Contract state (e.g., balances)

public:
    SmartContract(std::string script);
    bool execute(std::string caller, double value, NiloticBlockchain& blockchain);
    void setState(std::string key, double value);
    double getState(std::string key) const;
};

#endif