#include <string>
#include "../lib/net/sock.hpp"

class Client {
private:
    std::string nickname = "";
    Socket sock;
public:
    Client(std::string _nickname) {
        nickname = _nickname;
    }
    void run();
    const Socket& getSock() const {return sock;}
private:
    static unsigned __stdcall msgThread(void* cli);
};