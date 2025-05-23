#include "GatewayClient.hpp"
#include <netinet/in.h>
#include <sys/socket.h>

Client::Client(std::string serverIp, int serverPort) {
    _clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (_clientSock < 0) {
        cerr << "Error creating socket" << endl;
        exit(EXIT_FAILURE);
    }

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;

    if (setsockopt(_clientSock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        std::cerr << "Failed to set send timeout: " << strerror(errno) << '\n';
        throw std::runtime_error("Failed to set socket opt");
    }

    // Set up the server address structure
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIp.data(), &_serverAddr.sin_addr);

    // Connect to the server
    if (connect(_clientSock, (struct sockaddr*)&_serverAddr,
                sizeof(_serverAddr)) < 0) {
        close(_clientSock);
        cerr << "Error connecting to server" << endl;
        throw std::runtime_error("Failed to connect to server");
    }

    _fds[0].fd = _clientSock;
    _fds[0].events = POLLIN;

    FD_ZERO(&_readfds);
    FD_SET(_clientSock, &_readfds);

}

Client::~Client() {
    close(_clientSock);
}

std::optional<Message> Client::GetMessage() {
    int ret = poll(_fds, 1, 0);
    if (ret > 0 && (_fds[0].revents & POLLIN)) {
        return GetMessageBlocking();
    }
    return std::nullopt;
}

std::optional<Message> Client::GetMessageBlocking() {
    int result = select(_clientSock + 1, &_readfds, nullptr, nullptr, nullptr);
    if (result <= 0) {
        return std::nullopt;
    }

    uint32_t messageLength = 0;
    int bytesReceived = 0;
    // Get message size
    bytesReceived +=
        recv(_clientSock, &messageLength, sizeof(messageLength), MSG_WAITALL);
    if (bytesReceived < 0) {
        return std::nullopt;
    } else if (bytesReceived == 0) {
        cerr << "Connection closed" << endl;
        return std::nullopt;
    }

    messageLength = ntohl(messageLength);

    if (messageLength > 0) {
        std::string message(messageLength, '\0');
        if (recv(_clientSock, message.data(), messageLength, MSG_WAITALL) <=
            0) {
            return std::nullopt;
        }
        std::string ip;
        int port;
        if (getIPandPort(_clientSock, ip, port)) {
            return Message{
                .senderIp = ip,
                .senderPort = port,
                .senderSock = -1,
                .receiverSock = _clientSock,
                .msg = message,
            };
        } else {
            return Message{
                .senderSock = -1,
                .receiverSock = _clientSock,
                .msg = message,
            };
        }
    }
    return std::nullopt;
}

bool Client::SendMessage(std::string message) {
    int messageLen = htonl(message.size());
    if (send(_clientSock, &messageLen, sizeof(messageLen), 0) <= 0) {
        return false;
    }
    if (send(_clientSock, message.data(), message.size(), 0) <= 0) {
        return false;
    }
    return true;
}

