#include <iostream>

#include "GatewayServer.hpp"

using std::cout, std::endl;

int main() {
    cout << "Server started" << endl;
    Server server(80, 10, "/tmp/path");
    while (true) {
        auto requests = server.GetMessagesBlocking();
        for (auto r : requests) {
            cout << r << endl;
            server.SendMessage(Message{.senderSock = server.GetServerSocketFd(),
                                       .receiverSock = r.senderSock,
                                       .msg = "This is server"});
        }
    }
}
