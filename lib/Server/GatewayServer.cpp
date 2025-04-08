#include "GatewayServer.hpp"
#include "Common.hpp"

Server::Server(int serverPort, int max_clients) {
    _serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSock < 0) {
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(_serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(_serverSock);
        exit(EXIT_FAILURE);
    }

    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces
    _serverAddr.sin_port = htons(serverPort); // Convert port to network byte order
    
    if (bind(_serverSock, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr)) <
        0) {
        exit(EXIT_FAILURE);
    }

    if (listen(_serverSock, max_clients) < 0) {
        exit(EXIT_FAILURE);
    }

    _coreLoop = std::thread(&Server::listeningLoop, this);
}

Server::~Server() {
    _stopFlag = true;
    _coreLoop.join();
}

std::vector<Message> Server::GetMessages() {
    std::lock_guard<std::mutex> lock(_requestsMutex);
    std::vector<Message> output;
    output.reserve(_requests.size());
    while (!_requests.empty()) {
        output.push_back(_requests.front());
        _requests.pop();
    }
    return output;
}

std::vector<Message> Server::GetMessagesBlocking() {
    std::unique_lock<std::mutex> lock(_requestsMutex);
    while (_requests.empty() || _stopFlag) {
        _requestsCv.wait(lock);
    }
    std::vector<Message> output;
    output.reserve(_requests.size());
    while (!_requests.empty()) {
        output.push_back(_requests.front());
        _requests.pop();
    }
    return output;
}

bool Server::PendingMessages() {
    std::lock_guard<std::mutex> lock(_requestsMutex);
    return !_requests.empty();
}

bool Server::SendMessage(Message message) {
    uint32_t messageSize = htonl(message.msg.size());
    if (send(message.receiverSock, &messageSize, sizeof(messageSize), 0) <= 0) {
        return false;
    };
    if (send(message.receiverSock, message.msg.data(), message.msg.size(), 0) <=
        0) {
        return false;
    };
    return true;
}

int Server::GetServerSocketFd() {
    return _serverSock;
}

void* Server::listeningLoop() {
    fd_set masterSet;
    fd_set readFds;
    std::vector<int> clientSockets;
    int maxFd;

    FD_ZERO(&masterSet);
    FD_SET(_serverSock, &masterSet);
    maxFd = _serverSock;

    while (!_stopFlag) {
        readFds = masterSet;
        // Select blocks until new connection
        if (select(maxFd + 1, &readFds, nullptr, nullptr, nullptr) < 0) {
            continue;
        }

        // Accept new connections
        if (FD_ISSET(_serverSock, &readFds)) {
            struct sockaddr_un clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int clientSock =
                accept(_serverSock, (struct sockaddr*)&clientAddr, &clientLen);
            if (clientSock < 0) {
                close(clientSock);
            } else {
                FD_SET(clientSock, &masterSet);
                clientSockets.push_back(clientSock);
                maxFd = std::max(maxFd, clientSock);
            }
        }

        // Check for messages from client connections
        for (auto it = clientSockets.begin(); it != clientSockets.end();) {
            int clientSock = *it;

            if (FD_ISSET(clientSock, &readFds)) {
                if (_addMessage(clientSock) == 0) {
                    it = clientSockets.erase(it);
                } else {
                    it++;
                }
            } else {
                ++it;
            }
        }
    }

    for (auto it = clientSockets.begin(); it != clientSockets.end(); ){
            int clientSock = *it;
        if (FD_ISSET(clientSock, &readFds)) {
            int message = 0;
            send(clientSock, &message, sizeof(message), 0);
        }
        it++;
    }
    return nullptr;
}

int Server::_addMessage(int clientSock) {
    uint32_t messageLength = 0;
    int bytesReceived = 0;
    // Get message size
    bytesReceived += recv(clientSock, &messageLength, sizeof(messageLength), 0);
    if (bytesReceived <= 0) {
        return 0;
    }

    messageLength = ntohl(messageLength);
    if (messageLength > 0) {
        std::string message(messageLength, '\0');
        if (recv(clientSock, message.data(), messageLength, MSG_WAITALL) <= 0) {
            return 0;
        }
        {
            auto m = Message{.senderSock = clientSock,
                             .receiverSock = _serverSock,
                             .msg = message};
            std::string ip;
            int port;
            if (getIPandPort(clientSock, ip, port)) {
                m.senderIp = ip;
                m.senderPort = port;
            }
            std::lock_guard<std::mutex> lock(_requestsMutex);
            _requests.push(m);
            _requestsCv.notify_one();
        }
        return messageLength;
    }

    return 0;
}
