#include "client.hpp"  // 包含 client.hpp 头文件
#include "sock.hpp"
#include "protocol.hpp"
#include <thread>
#include <atomic>
#include <mutex>
#include <string>
#include <iostream>

// 移除构造函数和析构函数的重复定义
/*
Client::Client() : sock_(AF_INET, SOCK_STREAM, IPPROTO_TCP) {}
Client::~Client() { stop(); }
*/

void Client::connectTo(const std::string& ip, uint16_t port) {
    sock_.connect(ip, port);
    running_.store(true);
    recvThread_ = std::thread(&Client::recvLoop, this);

    // 可选：上线包
    Packet login(EntryType::LOGIN, 0, 0, "login");
    std::lock_guard<std::mutex> lk(sendMtx_);
    login.send(sock_);
}

void Client::stop() {
    if (!running_.exchange(false)) return;

    try {
        Packet logout(EntryType::LOGOUT, 0, 0, "logout");
        std::lock_guard<std::mutex> lk(sendMtx_);
        logout.send(sock_);
    } catch (...) {}

    sock_.close();
    if (recvThread_.joinable()) recvThread_.join();
}

void Client::sendMessage(const std::string& text) {
    Packet p(EntryType::Message, 0, static_cast<uint64_t>(text.size()), text);
    std::lock_guard<std::mutex> lk(sendMtx_);
    p.send(sock_);
}

void Client::recvLoop() {
    try {
        while (running_.load()) {
            Packet p;
            p.recv(sock_);

            if (p.type == EntryType::Message) {
                std::cout << "[from " << p.id << "] " << p.msg << "\n";
            }
        }
    } catch (const std::exception& e) {
        if (running_.load()) {
            std::cerr << "recvLoop error: " << e.what() << "\n";
        }
    }
}
