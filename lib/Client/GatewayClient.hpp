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

    bool SendMessage(std::string message);


   private:
    struct sockaddr_in _serverAddr;
    int _clientSock;

    pollfd _fds[1];
};
