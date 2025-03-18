#include <iostream>

#include "GatewayClient.hpp"

using std::cout, std::endl;

int main() {
    Client client("na", 80, "/tmp/path");
    std::vector<std::string> messages = {"Hi this is the server",
                                         "Did you get this?"};
    client.SendMessages(messages);
    std::optional<Message> msg = client.GetMessageBlocking();
    if (msg) {
        cout << *msg << endl;
    }
}
