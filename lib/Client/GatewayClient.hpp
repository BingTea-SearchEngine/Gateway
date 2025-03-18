#pragma once

#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <optional>
#include <string>
#include <vector>

#include "Common.hpp"

class Client {
   public:
    Client(std::string serverIp, int serverPort, std::string socketPath);

    std::optional<Message> GetMessageBlocking();

    // Try sending all messages. Failed ones are put into output vector
    std::vector<std::string> SendMessages(std::vector<std::string> messages);

   private:
    struct sockaddr_un _serverAddr;
    int _clientSock;
};
