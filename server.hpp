#ifndef SERVER_HPP
#define SERVER_HPP

#pragma once
#include "sock.hpp"
#include "protocol.hpp"

#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <vector>
#include <string>

class User {
public:
    uint32_t id;
    Socket sock;
    std::mutex sendMtx;

    User(uint32_t uid, Socket&& s) : id(uid), sock(std::move(s)) {}
    User(const User&) = delete;
    User& operator=(const User&) = delete;
};

class Server {
public:
    explicit Server(uint16_t port);
    ~Server();

    void start();
    void stop();

private:
    Socket listenSock_;
    std::atomic<bool> running_{false};
    std::thread acceptThread_;

    std::mutex mtx_;
    std::unordered_map<uint32_t, std::shared_ptr<User>> users_;
    std::unordered_map<uint32_t, std::thread> workers_;

    void acceptLoop();
    void clientLoop(std::shared_ptr<User> user);
    void removeUser(uint32_t uid);
    uint32_t allocUserId();

    void broadcast(const Packet& p, uint32_t exceptId = 0);
};
#endif // SERVER_HPP