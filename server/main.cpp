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
        system("pause");
        return 1;
    }
    return 0;
}
// g++ -o ThreadServer.exe server/main.cpp server/server.cpp lib/net/protocol.cpp lib/net/sock.cpp -I. -lws2_32 -std=c++17