// include/dao.h
#ifndef DAO_H
#define DAO_H

#include "blockchain.h"
#include <string>
#include <map>

struct Proposal {
    std::string id;
    std::string description;
    std::map<std::string, bool> votes; // Address -> Vote (true = yes)
    bool executed;
};

class SulweDAO {
private:
    NiloticBlockchain& blockchain;
    std::map<std::string, Proposal> proposals;
    std::map<std::string, double> gSLW; // Governance SLW stakes

public:
    SulweDAO(NiloticBlockchain& bc);
    std::string createProposal(std::string creator, std::string desc);
    void vote(std::string proposalId, std::string voter, bool voteYes);
    void executeProposal(std::string proposalId);
};

#endif