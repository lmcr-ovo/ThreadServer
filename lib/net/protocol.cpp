#define _WIN32_WINNT 0x0600
#define NTDDI_VERSION 0x06000000
#include "sock.hpp"
#include "protocol.hpp"
#include <windows.h>

void Packet::send(const Socket& s) const {
    uint32_t typeRaw = static_cast<uint32_t>(type);
    s.sendAll((const char*)&typeRaw, sizeof(typeRaw));
    s.sendAll((const char*)&nickLen, sizeof(uint64_t));
    if (nickLen) s.sendAll((const char*) nickname.data(), nickLen);
    s.sendAll((const char*) &msgLen, sizeof(uint64_t));
    if (msgLen) s.sendAll((const char*) msg.data(), msgLen);
}

void Packet::recv(const Socket& s) {
    s.recvAll((char*) &type, sizeof(uint32_t));
    s.recvAll((char*) &nickLen, sizeof(uint64_t));
    nickname.resize(nickLen);
    s.recvAll((char*) nickname.data(), nickLen);
    s.recvAll((char*) &msgLen, sizeof(uint64_t));
    msg.resize(msgLen);
    if (msgLen) s.recvAll(msg.data(), msgLen);
}
