#include "lib/net/sock.hpp"
#include <unordered_map>
#include "lib/User.hpp"
#include <process.h>

#include <memory>
#include <cstdio>

class Server {
private:
    Socket listenSock;
public:
    std::unordered_map<std::string, User> users;
    Server() = default;
    ~Server() = default;
    void run(uint16_t port);
    auto push(User& usr) {return users.emplace(std::move(usr.getNickname()), std::move(usr));}
    void pop(const std::string& nickname) { users.erase(nickname); }
private:
    static unsigned __stdcall cliThread(void* usr);
};


struct ThreadParam {
    Server* server;
    User* user;
};
