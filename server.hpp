#include "lib/net/sock.hpp"
#include <unordered_map>
#include "lib/User.hpp"
#include "lib/Mutex.hpp"
#include <process.h>

class Server; // 前向声明

struct ThreadParam {
    Server* server;
    Socket* sock;
};

class Server {
private:
    Socket listenSock;
    Mutex logMutex;

public:
    Server();
    ~Server();
    void start(uint16_t port);

private:
    static unsigned __stdcall clientThread(void* param);
};