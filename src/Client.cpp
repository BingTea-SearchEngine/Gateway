#include <iostream>

#include "GatewayClient.hpp"

using std::cout, std::endl;

int main() {
    Client client("127.0.0.1", 8001);
    client.SendMessage("Hi this is the client");
    client.SendMessage("Did you get this?");
    std::optional<Message> msg = client.GetMessageBlocking();
    if (msg) {
        cout << *msg << endl;
    }
    cout << *client.GetMessageBlocking() << endl;
    client.SendMessage("HI");
}
