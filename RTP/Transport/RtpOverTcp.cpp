#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sys/uio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
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

    int flags = fcntl(m_sockfd, F_GETFL, 0);
    fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK);

    int err = ::connect(m_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (err < 0)
    {
        if (errno == EINPROGRESS)
        {
            struct timeval tval = {
                .tv_sec = 3,
                .tv_usec = 0
            };
            fd_set wset;
            FD_ZERO(&wset);
            FD_SET(m_sockfd, &wset);
            err = select(m_sockfd + 1, NULL, &wset, NULL, &tval);
            if (err > 0)
            {
                int optval = -1;
                socklen_t optlen = sizeof(optval);
                if (getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) == 0
                    && optval == 0)
                {
                    err = 0;
                }
            }
        }
    }

    if (err == 0)
    {
        fcntl(m_sockfd, F_SETFL, flags);
        return true;
    }
    else
    {
        close(m_sockfd);
        m_sockfd = -1;
        return false;
    }
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
#if 0
    static FILE *debugf = fopen("./tcpstream.rtp", "wb");
    if (debugf != NULL)
    {
        fwrite(packet.getHeader(), 1, packet.getHeaderLength(), debugf);
        fwrite(packet.getPayload(), 1, packet.getPayloadLength(), debugf);
        fflush(debugf);
    }
#endif

    if (m_sockfd < 0)
    {
        return false;
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