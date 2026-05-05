#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP
#include "sock.hpp"
#include <stdio.h>
#include <cstdint>
#include <string>
enum class EntryType : uint32_t {
    LOGIN = 1,
    LOGOUT = 2,
    Message = 3
};

class Packet {
public:
    EntryType type;
    uint64_t nickLen;
    std::string nickname;
    uint64_t msgLen;
    std::string msg;
public:
    Packet() = default;
    Packet(EntryType _type, u_int64 _nickLen, std::string _nickname, uint64_t _msgLen, std::string _msg)
    : type(_type), nickLen(_nickLen), nickname(_nickname), msgLen(_msgLen), msg(_msg) {}
    void send(const Socket& s) const;
    void recv(const Socket& s);
};
#endif 