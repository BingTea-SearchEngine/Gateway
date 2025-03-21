#include <iostream>

#include "GatewayClient.hpp"

using std::cout, std::endl;

int main() {
    Client client("35.1.252.67", 8001);
    client.SendMessage("Hi this is the client");
    client.SendMessage("Did you get this?");
    std::optional<Message> msg = client.GetMessageBlocking();
    if (msg) {
        cout << *msg << endl;
    }
}
