#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sys/uio.h>
#include <string.h>
#include <errno.h>
#include "RtpOverTcp.h"

RtpOverTcp::RtpOverTcp(int localPort)
    : RtpNet(localPort)
{}

RtpOverTcp::~RtpOverTcp()
{}

bool RtpOverTcp::connect(const std::string& ipv4, int port)
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0)
    {
        return false;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ipv4.c_str());

    int res = ::connect(m_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (res < 0)
    {
        close(m_sockfd);
        return false;
    }

    return true;
}

bool RtpOverTcp::disconnect()
{
    if (m_sockfd > 0)
    {
        close(m_sockfd);
        m_sockfd = -1;
    }

    return true;
}

bool RtpOverTcp::send(RtpPacket& packet)
{
    if (m_sockfd < 0)
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

    return writev(m_sockfd, iov, 3) > 0;
}