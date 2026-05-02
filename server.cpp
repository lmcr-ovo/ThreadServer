#include "server.hpp"  // 包含 server.hpp 头文件
#include "net/sock.hpp"
#include "net/protocol.hpp"   // Packet 和 EntryType 的定义
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <vector>
#include <iostream>

Server::Server(uint16_t port)
    : listenSock_(AF_INET, SOCK_STREAM, IPPROTO_TCP) {
    listenSock_.bind(port, "0.0.0.0");
    listenSock_.listen(64);
}

Server::~Server() { stop(); }

void Server::start() {
    if (running_.exchange(true)) return;
    acceptThread_ = std::thread(&Server::acceptLoop, this);
}

void Server::stop() {
    if (!running_.exchange(false)) return;

    listenSock_.close();
    if (acceptThread_.joinable()) acceptThread_.join();

    std::vector<std::thread> toJoin;
    {
        std::lock_guard<std::mutex> lk(mtx_);
        for (auto& [uid, u] : users_) u->sock.close();
        for (auto& [uid, t] : workers_) {
            if (t.joinable()) toJoin.push_back(std::move(t));
        }
        workers_.clear();
        users_.clear();
    }
    for (auto& t : toJoin) t.join();
}

void Server::acceptLoop() {
    while (running_.load()) {
        try {
            Socket cli = listenSock_.accept();
            uint32_t uid = allocUserId();
            auto user = std::make_shared<User>(uid, std::move(cli));

            {
                std::lock_guard<std::mutex> lk(mtx_);
                users_[uid] = user;
                workers_.emplace(uid, std::thread(&Server::clientLoop, this, user));
            }

            std::cout << "user connected: " << uid << "\n";
        } catch (const std::exception& e) {
            if (running_.load()) {
                std::cerr << "acceptLoop error: " << e.what() << "\n";
            }
        }
    }
}

void Server::clientLoop(std::shared_ptr<User> user) {
    try {
        while (running_.load()) {
            Packet p;
            p.recv(user->sock);

            // 防止客户端伪造发送者 id
            p.id = user->id;

            // 处理登出请求
            if (p.type == EntryType::LOGOUT) break;

            // 将消息广播给其他客户端
            broadcast(p, user->id);
        }
    } catch (const std::exception& e) {
        std::cerr << "client " << user->id << " disconnected: " << e.what() << "\n";
    }

    removeUser(user->id);
}

void Server::broadcast(const Packet& p, uint32_t exceptId) {
    std::vector<std::shared_ptr<User>> snapshot;
    {
        std::lock_guard<std::mutex> lk(mtx_);
        snapshot.reserve(users_.size());
        for (auto& [uid, u] : users_) {
            if (uid != exceptId) snapshot.push_back(u);
        }
    }

    for (auto& u : snapshot) {
        try {
            std::lock_guard<std::mutex> lk(u->sendMtx);
            p.send(u->sock);
        } catch (...) {
            // 这里先忽略异常，后续由对端读写失败触发清理
        }
    }
}

void Server::removeUser(uint32_t uid) {
    std::thread th;
    {
        std::lock_guard<std::mutex> lk(mtx_);

        auto itU = users_.find(uid);
        if (itU != users_.end()) {
            itU->second->sock.close();
            users_.erase(itU);
        }

        auto itT = workers_.find(uid);
        if (itT != workers_.end()) {
            if (itT->second.joinable() &&
                itT->second.get_id() != std::this_thread::get_id()) {
                th = std::move(itT->second);
            } else if (itT->second.joinable()) {
                itT->second.detach();
            }
            workers_.erase(itT);
        }
    }
    if (th.joinable()) th.join();
}

uint32_t Server::allocUserId() {
    static std::atomic<uint32_t> seq{1};
    return seq.fetch_add(1);
}

/*
g++ -std=c++17 -g -Wall -Wextra .\server_main.cpp .\server.cpp .\net\sock.cpp .\net\protocol.cpp -o .\server.exe -lws2_32
g++ -std=c++17 -g -Wall -Wextra .\client_main.cpp .\client.cpp .\net\sock.cpp .\net\protocol.cpp -o .\client.exe -lws2_32
*/