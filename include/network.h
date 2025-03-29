// include/network.h
#ifndef NETWORK_H
#define NETWORK_H

#include "blockchain.h"
#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <queue>

class Node {
private:
    boost::asio::io_context& io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;
    std::vector<std::shared_ptr<boost::asio::ip::tcp::socket>> peers;
    NiloticBlockchain& blockchain;
    std::queue<Block> pendingBlocks;

    void startAccept();
    void handleAccept(std::shared_ptr<boost::asio::ip::tcp::socket> peer, const boost::system::error_code& ec);
    void broadcastBlock(const Block& block);
    void syncChain();

public:
    Node(boost::asio::io_context& ioc, unsigned short port, NiloticBlockchain& bc);
    void addPeer(const std::string& host, unsigned short port);
    void notifyNewBlock(const Block& block);
};

#endif