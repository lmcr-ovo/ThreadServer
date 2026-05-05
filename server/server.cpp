#include "server.hpp"
#include "../lib/net/protocol.hpp"
#include <iostream>
#include <memory>

void Server::run(uint16_t port) {
    listenSock = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    listenSock.bind(port);
    listenSock.listen();
    printf("Server listening on port: %d\n", port);
    while (true) {
        Socket cliSock = listenSock.accept();
        Packet pkt;
        pkt.recv(cliSock);
        if (pkt.type != EntryType::LOGIN) {
            cliSock.close();
            throw std::runtime_error("Invaild Login");
        }
        User usr(pkt.nickname, std::move(cliSock));
        printf("[update]: %s login\n", pkt.nickname.c_str());
        auto [it, inserted] = push(usr);

        ThreadParam* param = new ThreadParam{this, &it->second};
        HANDLE handle = (HANDLE)_beginthreadex(nullptr, 0, cliThread, param, 0, nullptr);
        if (handle != NULL) CloseHandle(handle);
    }
}

void Server::broadcast(const Packet& pkt) {
    LockGuard lock(mutex);
    for (const auto& [nickname, usr] : users) {
        if (pkt.nickname == nickname) continue;
        pkt.send(usr.getSock());
    }
}

unsigned __stdcall Server::cliThread(void* arg) {
    ThreadParam* param = static_cast<ThreadParam*>(arg);
    Server* server = param->server;
    User* user = param->user;
    std::string nickname = user->getNickname();
    delete param;
    Packet pkt;
    while (true) {
        pkt.recv(user->getSock());
        if (pkt.type == EntryType::LOGOUT) {
            server->pop(nickname);
            printf("[update]: %s logout\n", nickname.c_str());
            break;
        }
        pkt.prt();
        server->broadcast(pkt);
   }
   return 0;
}
