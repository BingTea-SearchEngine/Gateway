#include "Common.hpp"

bool getIPandPort(int sockFd, std::string& ip, int& port) {
    struct sockaddr_in peerAddr;
    socklen_t peerAddrLen = sizeof(peerAddr);
    if (getpeername(sockFd, (struct sockaddr*)&peerAddr, &peerAddrLen) == 0) {
        char senderIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &peerAddr.sin_addr, senderIP, sizeof(senderIP));
        ip = senderIP;
        port = ntohs(peerAddr.sin_port);
        return true;
    }
    return false;
}
