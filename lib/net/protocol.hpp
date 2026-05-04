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
    uint32_t nickname;
    uint64_t msgLen;
    std::string msg;
public:
    Packet() = default;
    Packet(EntryType _type, uint32_t _id, uint64_t _msgLen, std::string _msg)
    : type(_type), nickname(_id), msgLen(_msgLen), msg(_msg) {}
    void send(Socket& s) const;
    void recv(Socket& s);
};
#endif 