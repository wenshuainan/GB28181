#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sys/uio.h>
#include <string.h>
#include <errno.h>
#include "RtpOverTcp.h"

bool RtpOverTcp::framing()
{
    return false;
}

bool RtpOverTcp::connect(char *ip, int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        return false;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip);

    int res = ::connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (res < 0)
    {
        std::cerr << "Connection failed: " << strerror(errno) << std::endl;
        close(sockfd);
        return false;
    }

    std::cout << "Connected to " << ip << ":" << port << std::endl;
    return true;
}

bool RtpOverTcp::disconnect()
{
    if (sockfd > 0)
    {
        close(sockfd);
        sockfd = -1;
    }

    return true;
}

bool RtpOverTcp::send(const RtpPacket& packet)
{
    return false;
}