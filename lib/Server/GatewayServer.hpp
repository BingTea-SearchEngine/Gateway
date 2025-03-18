#pragma once

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <memory>

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "Common.hpp"

using std::cout, std::endl;

class Server {
   public:
    // Creates a thread that continously listens on a given socket and puts requests into
    // receivedRequests
    Server(int serverPort, int max_clients, std::string socketPath);

    ~Server();

    std::vector<Message> GetMessages();

    // Blocks while there are no messages
    std::vector<Message> GetMessagesBlocking();

    bool PendingMessages();

    bool SendMessage(Message message);

    int GetServerSocketFd();

   private:
    std::queue<Message> _requests;
    std::thread _coreLoop;

    std::atomic<bool> _stopFlag{false};
    std::mutex _requestsMutex;
    std::condition_variable _requestsCv;

    struct sockaddr_un _serverAddr;
    int _serverSock;

    // These functions are used in hte listening loop and are in a separate thread
    void* listeningLoop();
    int _addMessage(int clientSock);
};
