#pragma once

#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>

#include <iostream>
#include <optional>
#include <string>
#include <vector>

using std::cout, std::cerr, std::endl;

#include "Common.hpp"

class Client {
   public:
    Client(std::string serverIp, int serverPort);

    std::optional<Message> GetMessage();

    std::optional<Message> GetMessageBlocking();

    // Try sending all messages. Failed ones are put into output vector
    std::vector<std::string> SendMessages(std::vector<std::string> messages);

   private:
    struct sockaddr_in _serverAddr;
    int _clientSock;

    pollfd _fds[1];
};
