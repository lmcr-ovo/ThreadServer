#include "server.hpp"
#include "lib/net/sock.hpp"
#include <iostream>

int main() {
    try {
        WSA wsa; // 初始化WSA
        Server server;
        server.run(12345); // 例如端口12345
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}