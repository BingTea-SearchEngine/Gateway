#pragma once

#include <arpa/inet.h>   // For inet_ntoa and ntohl
#include <netinet/in.h>  // For sockaddr_in
#include <sys/socket.h>  // For getpeername
#include <unistd.h>
#include <string>

#include <iostream>
#include <string>

struct Message {
    std::string senderIp;
    int senderPort;
    int senderSock;
    int receiverSock;
    std::string msg;

    friend std::ostream& operator<<(std::ostream& os, const Message& m) {
        os << "Message { senderIp=" << m.senderIp
           << " senderPort=" << m.senderPort << " senderSock=" << m.senderSock
           << " receiverSock=" << m.receiverSock << " msg=\"" << m.msg
           << "\" }";
        return os;
    }
};

// Pass in buffer and integer to store ip and port into
bool getIPandPort(int sockFd, std::string& ip, int& port);
