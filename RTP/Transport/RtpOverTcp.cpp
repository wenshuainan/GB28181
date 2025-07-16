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
{
    disconnect();
}

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

    static FILE *debugf = fopen("./stream.rtpovertcp", "wb");
    if (debugf != NULL)
    {
        fwrite(packet.getHeader(), 1, packet.getHeaderLength(), debugf);
        fwrite(packet.getPayload(), 1, packet.getPayloadLength(), debugf);
        fflush(debugf);
    }

    uint16_t headerlen = packet.getHeaderLength();
    uint16_t payloadlen = packet.getPayloadLength();

    struct iovec iov[3];
    uint16_t length = htons(headerlen + payloadlen);

#if 1 // 《GB28181自动化测试工具》花屏问题
    uint8_t precede[4] = {0};
    precede[0] = '$';
    precede[1] = 0;
    memcpy(precede + 2, &length, sizeof(length));

    iov[0].iov_base = precede;
    iov[0].iov_len = sizeof(precede);
#else
    iov[0].iov_base = &length;
    iov[0].iov_len = sizeof(length);
#endif

    iov[1].iov_base = (void *)packet.getHeader();
    iov[1].iov_len = headerlen;

    iov[2].iov_base = (void *)packet.getPayload();
    iov[2].iov_len = payloadlen;

    return writev(m_sockfd, iov, 3)
            == (ssize_t)(sizeof(length) + headerlen + payloadlen);
}

uint16_t RtpOverTcp::getEfficLen()
{
    return 0xFFFF - 12;
}