// src/main.cpp
#include "blockchain.h"
#include "wallet.h"
#include "dao.h"
#include "smart_contract.h"
#include <iostream>
#include <crow.h>
#include <thread>

int main() {
    NiloticBlockchain blockchain(Network::Testwire);
    Wallet wallet;
    SulweDAO dao(blockchain);

    // No initial hardcoded balance; user must mine to earn SLW
    // Simulate initial state (optional, kept for DAO demo)
    Transaction tx(wallet.getPublicKey(), "bob", 0.0); // Zero-amount tx
    blockchain.addBlock({tx}, wallet.getPublicKey());

    // API setup
    crow::SimpleApp app;

    CROW_ROUTE(app, "/chain")([&blockchain]() {
        crow::json::wvalue result;
        std::vector<crow::json::wvalue> blocks;
        for (const auto& block : blockchain.getChain()) {
            blocks.push_back({{"hash", block.hash}, {"validator", block.validator}});
        }
        result["blocks"] = std::move(blocks);
        
        double totalSupply = blockchain.getChain().size() * 10.0;
        double remaining = 555000000.0 * 0.6 - totalSupply;
        
        result["totalSupply"] = totalSupply;
        result["remaining"] = remaining;
        return result;
    });

    CROW_ROUTE(app, "/transaction").methods("POST"_method)([&blockchain, &wallet](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");
        
        std::string receiver = x["receiver"].s();
        double amount = x["amount"].d();
        Transaction tx(wallet.getPublicKey(), receiver, amount);
        blockchain.addBlock({tx}, wallet.getPublicKey());
        return crow::response(200, "Transaction added");
    });

    CROW_ROUTE(app, "/stake").methods("POST"_method)([&blockchain, &wallet](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");
        
        double amount = x["amount"].d();
        if (blockchain.addStake(wallet.getPublicKey(), amount)) {
            return crow::response(200, "Stake added");
        }
        return crow::response(400, "Insufficient balance");
    });

    CROW_ROUTE(app, "/unstake").methods("POST"_method)([&blockchain, &wallet](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");

        double amount = x["amount"].d();
        double currentStake = blockchain.getStake(wallet.getPublicKey());
        if (blockchain.removeStake(wallet.getPublicKey(), amount)) {
            return crow::response(200, crow::json::wvalue{
                {"message", "Stake removed"},
                {"amount", amount},
                {"new_stake", blockchain.getStake(wallet.getPublicKey())},
                {"new_balance", blockchain.getBalance(wallet.getPublicKey())}
            });
        }
        return crow::response(400, crow::json::wvalue{
            {"error", "Insufficient stake"},
            {"requested", amount},
            {"available", currentStake}
        });
    });

    CROW_ROUTE(app, "/proposal").methods("POST"_method)([&dao, &wallet](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");
        
        std::string desc = x["description"].s();
        std::string propId = dao.createProposal(wallet.getPublicKey(), desc);
        return crow::response(200, crow::json::wvalue{{"proposalId", propId}});
    });

    CROW_ROUTE(app, "/contract").methods("POST"_method)([&blockchain, &wallet](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");
        
        std::string script = x["script"].s();
        double value = x["value"].d();
        SmartContract contract(script);
        contract.setState(wallet.getPublicKey(), 100.0);
        bool success = contract.execute(wallet.getPublicKey(), value, blockchain);
        return crow::response(success ? 200 : 400, success ? "Contract executed" : "Execution failed");
    });

    CROW_ROUTE(app, "/mine").methods("POST"_method)([&blockchain, &wallet](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Invalid JSON");

        std::string miner = wallet.getPublicKey();
        double stake = x["stake"].d();
        double reward = 5.0; // Fixed reward, hardcoded or from blockchain

        double currentBalance = blockchain.getBalance(miner);
        double currentStake = blockchain.getStake(miner);
        if (currentBalance == 0 && currentStake == 0) {
            Transaction dummyTx(miner, "network", 0.0);
            blockchain.addBlock({dummyTx}, miner);
            return crow::response(200, crow::json::wvalue{
                {"message", "Bootstrap block mined"},
                {"reward", reward}, // Always 5 SLW
                {"blockHash", blockchain.getChain().back().hash}
            });
        }

        if (currentBalance < stake) {
            return crow::response(400, "Insufficient stake");
        }

        Transaction dummyTx(miner, "network", 0.0);
        blockchain.addStake(miner, stake);
        blockchain.addBlock({dummyTx}, miner);
        return crow::response(200, crow::json::wvalue{
            {"message", "Block mined"},
            {"reward", reward}, // Always 5 SLW
            {"blockHash", blockchain.getChain().back().hash}
        });
    });

    CROW_ROUTE(app, "/balance")([&blockchain, &wallet]() {
        double balance = blockchain.getBalance(wallet.getPublicKey());
        double stake = blockchain.getStake(wallet.getPublicKey());
        return crow::response(200, crow::json::wvalue{
            {"balance", balance},
            {"stake", stake}
        });
    });

    std::thread serverThread([&app]() { app.port(8080).multithreaded().run(); });
    serverThread.detach();

    std::cout << "Server running on http://localhost:8080. Press Enter to exit." << std::endl;
    std::cin.get();

    return 0;
}