// client_main.cpp
#include "net/sock.hpp"
#include "client.hpp"
#include <iostream>
#include <string>

int main() {
    try {
        WSA wsa;  // 必须先初始化 Winsock

        Client client;
        client.connectTo("127.0.0.1", 5555);

        std::cout << "Connected to server.\n";
        std::cout << "Type message and press Enter. Input '/quit' to exit.\n";

        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "/quit") break;
            if (!line.empty()) client.sendMessage(line);
        }

        client.stop();
        std::cout << "Client exited.\n";
    } catch (const std::exception& e) {
        std::cerr << "Client error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
