#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP


#include "net/sock.hpp"
#include "net/protocol.hpp"

#include <thread>
#include <atomic>
#include <mutex>
#include <string>

class Client {
public:
    Client() : sock_(AF_INET, SOCK_STREAM, IPPROTO_TCP) {}
    ~Client() { stop(); }

    void connectTo(const std::string& ip, uint16_t port);
    void stop();

    void sendMessage(const std::string& text);

private:
    Socket sock_;
    std::atomic<bool> running_{false};
    std::thread recvThread_;
    std::mutex sendMtx_;

    void recvLoop();
};
#endif // CLIENT_HPP