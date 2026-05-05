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
        printf("DEBUG: name: %s\n", nickname.c_str());
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