// src/smart_contract.cpp
#include "smart_contract.h"
#include <sstream>
#include <vector>
#include <algorithm>

SmartContract::SmartContract(std::string script) : code(script) {}

bool SmartContract::execute(std::string caller, double value, NiloticBlockchain& blockchain) {
    std::stringstream ss(code);
    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token) tokens.push_back(token);

    if (tokens.empty()) return false;

    if (tokens[0] == "TRANSFER" && tokens.size() >= 7) {
        double amount = std::stod(tokens[1]);
        std::string from = (tokens[3] == "caller") ? caller : tokens[3];
        std::string to = tokens[5];
        if (tokens[6] == "IF" && tokens[7] == "balance" && tokens[8] == ">=") {
            double condition = std::stod(tokens[9]);
            if (state[from] >= condition) {
                state[from] -= amount;
                state[to] += amount;
                blockchain.addBlock({Transaction(from, to, amount)}, caller);
                return true;
            }
        }
    }
    return false; // Invalid or failed execution
}

void SmartContract::setState(std::string key, double value) { state[key] = value; }
double SmartContract::getState(std::string key) const { return state.count(key) ? state.at(key) : 0.0; }