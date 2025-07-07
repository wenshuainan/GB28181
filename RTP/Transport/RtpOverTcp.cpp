#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sys/uio.h>
#include <string.h>
#include <errno.h>
#include "RtpOverTcp.h"

RtpOverTcp::RtpOverTcp()
{}

RtpOverTcp::~RtpOverTcp()
{}

bool RtpOverTcp::connect(char *ip, int port)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
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
        close(sockfd);
        return false;
    }

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

bool RtpOverTcp::send(RtpPacket& packet)
{
    if (sockfd < 0)
    {
        return false;
    }

    struct iovec iov[3];
    uint16_t length = htons(packet.getHeaderLength() + packet.getPayloadLength());

    iov[0].iov_base = &length;
    iov[0].iov_len = sizeof(length);

    iov[1].iov_base = (void *)packet.getHeader();
    iov[1].iov_len = packet.getHeaderLength();

    iov[2].iov_base = (void *)packet.getPayload();
    iov[2].iov_len = packet.getPayloadLength();

    return writev(sockfd, iov, 3) > 0;
}