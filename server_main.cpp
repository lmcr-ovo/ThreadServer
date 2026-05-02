// server_main.cpp
#include "net/sock.hpp"
#include "server.hpp"
#include <iostream>
#include <string>

int main() {
    try {
        WSA wsa;                 // 必须先初始化 Winsock
        Server server(5555);     // 监听端口
        server.start();

        std::cout << "Server started at 0.0.0.0:5555\n";
        std::cout << "Input 'q' then Enter to quit.\n";

        std::string cmd;
        while (std::getline(std::cin, cmd)) {
            if (cmd == "q" || cmd == "quit" || cmd == "exit") break;
        }

        server.stop();
        std::cout << "Server stopped.\n";
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
