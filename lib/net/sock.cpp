#include <stdexcept>
#include "sock.hpp"
#include <cstdint>

WSA::WSA() {
    WORD wVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    int error = WSAStartup(wVersion, &wsaData);
    if (error != 0) {
        throw std::runtime_error("WSAStartup failed: " + std::to_string(error));
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        throw std::runtime_error("WSA version mismatch");
    }
}

WSA::~WSA() {
    WSACleanup();
}

Socket::Socket(int af, int type, int protocol) {
    sock_ = socket(af, type, protocol);
    if (sock_ == INVALID_SOCKET) {
        throw std::runtime_error("socket creation failed: " + std::to_string(WSAGetLastError()));
    }
}

Socket::~Socket() {
    close();
}

// 移动构造函数
Socket::Socket(Socket&& other) noexcept : sock_(other.sock_) {
    other.sock_ = INVALID_SOCKET;
}

// 移动赋值函数
Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        close();
        sock_ = other.sock_;
        other.sock_ = INVALID_SOCKET;
    }
    return *this;
}

void Socket::close() {
    if (sock_ != INVALID_SOCKET) {
        closesocket(sock_);
        sock_ = INVALID_SOCKET;
    }
}

void Socket::connect(const std::string& ip, uint16_t port) {
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (::connect(sock_, (SOCKADDR*) &addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
        throw std::runtime_error("connect failed: " + std::to_string(WSAGetLastError()));
    }
}

void Socket::bind(uint16_t port, const std::string& ip) {
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (::bind(sock_, (SOCKADDR*) &addr, sizeof(addr)) == SOCKET_ERROR) {
        throw std::runtime_error("bind failed: " + std::to_string(WSAGetLastError()));
    }
}

void Socket::listen(int backlog) {
    if (::listen(sock_, backlog) == SOCKET_ERROR) {
        throw std::runtime_error("listen failed: " + std::to_string(WSAGetLastError()));
    }
}

Socket Socket::accept() {
    SOCKADDR_IN addrCli;
    int len = sizeof(addrCli);
    SOCKET s = ::accept(sock_, (SOCKADDR*) &addrCli, &len);
    if (s == INVALID_SOCKET) {
        throw std::runtime_error("accept failed: " + std::to_string(WSAGetLastError()));
    }
    return Socket(s);
}

void Socket::sendAll(const char* buf, uint64_t size) const {
    uint64_t sent = 0;
    while (sent < size) {
        int n = send(sock_, buf + sent, static_cast<int>(size - sent), 0);
        if (n <= 0) {
            throw std::runtime_error("send failed or connection closed");
        }
        sent += n;
    }
}

void Socket::recvAll(char* buf, uint64_t size) const {
    uint64_t recvd = 0;
    while (recvd < size) {
        int n = recv(sock_, buf + recvd, static_cast<int> (size - recvd), 0);
        if (n <= 0) {
            throw std::runtime_error("recv failed or connection closed");
        }
        recvd += n;
    }
}
