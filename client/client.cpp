#include "client.hpp"
#include "../lib/net/sock.hpp"
#include "../lib/net/protocol.hpp"
#include <iostream>
#include <string>
#include <cstdio>

void Client::run() {
    // 调用移动赋值函数
    sock = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sock.connect("26.204.58.8", 12345);
    Packet pkt(EntryType::LOGIN, nickname.size(), nickname, 0, "");
    pkt.send(sock);
    HANDLE handle = (HANDLE)_beginthreadex(nullptr, 0, msgThread, this, 0, nullptr);
    if (handle != NULL) CloseHandle(handle);
    std::string msg;
    while (true) {
        std::cin >> msg;
        if (msg == "/quit") {
            pkt.type = EntryType::LOGOUT;
            pkt.nickname = nickname;
            pkt.nickLen = nickname.size();
            pkt.msgLen = 0;
            pkt.msg = "";
            pkt.send(sock);
            break;
        }
        pkt.type = EntryType::Message;
        pkt.nickname = nickname;
        pkt.nickLen = nickname.size();
        pkt.msgLen = msg.size();
        pkt.msg = msg;
        pkt.send(sock);
    }
}

unsigned __stdcall Client::msgThread(void* cli) {
    try {
        Client* ptr = (Client*) cli;
        Packet pkt;
        while (true) {
            try {
                pkt.recv(ptr->getSock());
                pkt.prt();
            } catch (const std::exception& e) {
                printf("[Error in msgThread]: %s\n", e.what());
                break;
            }
        }
    } catch (...) {
        printf("[Fatal error in msgThread]\n");
    }
    return 0;
}