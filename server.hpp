#include "lib/net/sock.hpp"
#include <unordered_map>
#include "lib/User.hpp"
#include "lib/Mutex.hpp"
#include <process.h>
#include <unordered_map>

class Server; // 前向声明

struct ThreadParam {
    Server* server;
    Socket* sock;
};

class Server {
private:
    Socket listenSock;
    Mutex logMutex;
    std::unordered_map<std::string, User> users;
public:
    Server();
    ~Server();
    void start(uint16_t port);
    void run(uint16_t port);
    void push(User&& usr) { users.emplace(std::piecewise_construct, std::forward_as_tuple(std::move(usr.getNickname())), std::forward_as_tuple(std::move(usr))); }
    void pop(const std::string& nickname) { users.erase(nickname); }
private:
    static unsigned __stdcall cliThread(void* usr);
    static unsigned __stdcall clientThread(void* param);
};