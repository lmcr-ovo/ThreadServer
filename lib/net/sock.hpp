#ifndef SOCK_HPP
#define SOCK_HPP

#include <winsock2.h>
#include <cstdint>
#include <string>

class WSA {
public:
    WSA();
    ~WSA();
    WSA(const WSA&) = delete;
    WSA& operator=(const WSA&) = delete;
};


class Socket {
private:
    SOCKET sock_;
public:
    Socket() = default;
    explicit Socket(SOCKET s) : sock_(s) {}
    explicit Socket(int af, int type, int protocol);
    ~Socket();

    Socket(Socket&& other) noexcept;
    Socket& operator= (Socket&& other) noexcept;

    SOCKET get() const {return sock_;}
    bool valid() const {return sock_ != INVALID_SOCKET;}

    void connect(const std::string& ip, uint16_t port);
    void bind(uint16_t port, const std::string& ip = "0.0.0.0");
    void listen(int backlog = 5);
    Socket accept();

    void sendAll(const char* buf, uint64_t size) const;
    void recvAll(char* buf, uint64_t size) const;

    void close();
};

#endif // SOCK_HPP