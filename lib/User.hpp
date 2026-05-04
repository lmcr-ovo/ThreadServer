#ifndef USER
#define USER
#include "net/sock.hpp"
#include <string>
#include <unordered_map>

class User {
private:
    std::string nickname;
    Socket sock;
public:
    User(std::string n, Socket&& s) : nickname(std::move(n)), sock(std::move(s)) {}
    User(const User&) = delete;
    User& operator=(const User&) = delete;
    Socket& getSock() { return sock; }
    const std::string& getNickname() const { return nickname; }
};
#endif