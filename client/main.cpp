#include "client.hpp"
#include "../lib/net/sock.hpp"
#include <iostream>
#include <string>

int main() {
    try {
        WSA wsa;
        printf("input nick name: ");
        std::string nickname = "";
        std::cin >> nickname;
        Client client(nickname);
        printf("input '/quit' to end\n");
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        system("pause");
        return 1;
    }
    return 0;
}
//g++ -o ThreadServer.exe server/main.cpp server/server.cpp lib/net/protocol.cpp lib/net/sock.cpp -I. -lws2_32 -std=c++17
//g++ -o ThreadClient.exe client/main.cpp client/client.cpp lib/net/protocol.cpp lib/net/sock.cpp -I. -lws2_32 -std=c++17
