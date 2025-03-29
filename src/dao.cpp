// src/dao.cpp
#include "dao.h"
#include <sstream>

SulweDAO::SulweDAO(NiloticBlockchain& bc) : blockchain(bc) {}

std::string SulweDAO::createProposal(std::string creator, std::string desc) {
    std::stringstream ss;
    ss << "PROP_" << proposals.size() + 1;
    std::string id = ss.str();
    proposals[id] = {id, desc, {}, false};
    blockchain.addStake(creator, 0); // Ensure creator is in stakes
    return id;
}

void SulweDAO::vote(std::string proposalId, std::string voter, bool voteYes) {
    if (proposals.count(proposalId) && blockchain.getBalance(voter) > 0) {
        proposals[proposalId].votes[voter] = voteYes;
    }
}

void SulweDAO::executeProposal(std::string proposalId) {
    Proposal& prop = proposals[proposalId];
    if (prop.executed) return;

    int yesVotes = 0;
    for (const auto& [voter, vote] : prop.votes) {
        if (vote) yesVotes += static_cast<int>(blockchain.getBalance(voter));
    }
    if (yesVotes > static_cast<int>(blockchain.getChain().size() * 10)) { // Arbitrary threshold
        prop.executed = true;
        blockchain.addBlock({Transaction("dao", "treasury", 100.0)}, "dao_validator"); // Example action
    }
}