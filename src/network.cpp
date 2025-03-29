// src/network.cpp
#include "network.h"
#include <boost/bind.hpp>
#include <iostream>

Node::Node(boost::asio::io_context& ioc, unsigned short port, NiloticBlockchain& bc)
    : io_context(ioc), acceptor(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      socket(ioc), blockchain(bc) {
    startAccept();
    syncChain();
}

void Node::startAccept() {
    auto peer = std::make_shared<boost::asio::ip::tcp::socket>(io_context);
    acceptor.async_accept(*peer, boost::bind(&Node::handleAccept, this, peer, boost::asio::placeholders::error));
}

void Node::handleAccept(std::shared_ptr<boost::asio::ip::tcp::socket> peer, const boost::system::error_code& ec) {
    if (!ec) {
        peers.push_back(peer);
        std::cout << "New peer connected: " << peer->remote_endpoint() << std::endl;
    }
    startAccept(); // Accept next connection
}

void Node::addPeer(const std::string& host, unsigned short port) {
    auto peer = std::make_shared<boost::asio::ip::tcp::socket>(io_context);
    boost::asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(host, std::to_string(port));
    boost::asio::connect(*peer, endpoints);
    peers.push_back(peer);
    std::cout << "Connected to peer: " << host << ":" << port << std::endl;
}

void Node::notifyNewBlock(const Block& block) {
    pendingBlocks.push(block);
    broadcastBlock(block);
}

void Node::broadcastBlock(const Block& block) {
    std::string data = block.hash + "|" + block.validator; // Simple serialization
    for (auto& peer : peers) {
        boost::asio::async_write(*peer, boost::asio::buffer(data),
            [](const boost::system::error_code& ec, std::size_t) {
                if (ec) std::cerr << "Broadcast error: " << ec.message() << std::endl;
            });
    }
}

void Node::syncChain() {
    if (peers.empty()) return;
    auto peer = peers.front();
    boost::asio::streambuf buffer;
    boost::asio::async_read_until(*peer, buffer, '|',
        [this, peer](const boost::system::error_code& ec, std::size_t) {
            if (!ec) {
                std::istream is(&buffer);
                std::string data;
                std::getline(is, data, '|');
                std::cout << "Received block data: " << data << std::endl;
                // Parse and add to blockchain (simplified)
            }
        });
}