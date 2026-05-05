#include "../lib/net/sock.hpp"
#include "../lib/net/protocol.hpp"
#include "../lib/User.hpp"
#include "../lib/Mutex.hpp"
#include <unordered_map>
#include <process.h>

#include <memory>
#include <cstdio>

class Server {
private:
    Socket listenSock;
    std::unordered_map<std::string, User> users;
    Mutex mutex;
public:
    Server() = default;
    ~Server() = default;
    void run(uint16_t port);
    auto push(User& usr) {
        LockGuard lock(mutex);
        return users.emplace(std::move(usr.getNickname()), std::move(usr));
    }
    void pop(const std::string& nickname) {
        LockGuard lock(mutex);
        users.erase(nickname);
    }
    void broadcast(const Packet& pkt);

private:
    static unsigned __stdcall cliThread(void* usr);
};


struct ThreadParam {
    Server* server;
    User* user;
};
// g++ -o ThreadServer.exe    server/main.cpp    server/server.cpp    lib/net/protocol.cpp    lib/net/sock.cpp    -I.    -lws2_32    -std=c++17
