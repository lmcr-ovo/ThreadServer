#include "server.hpp"
#include "lib/net/protocol.hpp"
#include <iostream>
#include <memory>

Server::Server() = default;

Server::~Server() = default;

void Server::start(uint16_t port) {
    listenSock = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    listenSock.bind(port);
    listenSock.listen();

    {
        LockGuard guard(logMutex);
        std::cout << "Server listening on port " << port << std::endl;
    }

    while (true) {
        Socket clientSock = listenSock.accept();
        {
            LockGuard guard(logMutex);
            std::cout << "New connection accepted" << std::endl;
        }

        ThreadParam* tp = new ThreadParam{this, new Socket(std::move(clientSock))};
        HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, clientThread, tp, 0, NULL);
        if (thread == NULL) {
            {
                LockGuard guard(logMutex);
                std::cerr << "Failed to create thread" << std::endl;
            }
            delete tp->sock;
            delete tp;
            continue;
        }
        CloseHandle(thread);
    }
}

unsigned __stdcall Server::clientThread(void* param) {
    std::unique_ptr<ThreadParam> tpPtr((ThreadParam*)param);
    Server* server = tpPtr->server;
    std::unique_ptr<Socket> sockPtr(tpPtr->sock);
    Socket& sock = *sockPtr;
    try {
        Packet loginPkt;
        loginPkt.recv(sock);
        if (loginPkt.type != EntryType::LOGIN) {
            {
                LockGuard guard(server->logMutex);
                std::cerr << "Invalid login packet" << std::endl;
            }
            sock.close();
            return 1;
        }

        std::string nickname = loginPkt.msg;
        User user(nickname, std::move(sock));

        {
            LockGuard guard(server->logMutex);
            std::cout << "User " << user.getNickname() << " logged in" << std::endl;
        }

        while (true) {
            Packet pkt;
            pkt.recv(user.getSock());
            if (pkt.type == EntryType::LOGOUT) {
                {
                    LockGuard guard(server->logMutex);
                    std::cout << "User " << user.getNickname() << " logged out" << std::endl;
                }
                break;
            }
            // 处理Message
            {
                LockGuard guard(server->logMutex);
                std::cout << "Message from " << user.getNickname() << ": " << pkt.msg << std::endl;
            }
        }
    } catch (const std::exception& e) {
        {
            LockGuard guard(server->logMutex);
            std::cerr << "Error in client thread: " << e.what() << std::endl;
        }
    }
    return 0;
}