> server: 
g++ -o ThreadServer.exe server/main.cpp server/server.cpp lib/net/protocol.cpp lib/net/sock.cpp -I. -lws2_32 -std=c++17 -static -static-libgcc -static-libstdc++
client:
g++ -o ThreadClient.exe client/main.cpp client/client.cpp lib/net/protocol.cpp lib/net/sock.cpp -I. -lws2_32 -std=c++17 -static -static-libgcc -static-libstdc++
