#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>
#include "RtpOverUdp.h"

RtpOverUdp::RtpOverUdp(int localPort)
    : RtpNet(localPort)
{}

RtpOverUdp::~RtpOverUdp()
{
    disconnect();
}

bool RtpOverUdp::connect(const std::string& ipv4, int port)
{
    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sockfd < 0)
    {
        return false;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ipv4.c_str());

    if (::connect(m_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        close(m_sockfd);
        m_sockfd = -1;
        return false;
    }

    return true;
}

bool RtpOverUdp::disconnect()
{
    if (m_sockfd > 0)
    {
        close(m_sockfd);
        m_sockfd = -1;
    }

    return true;
}

bool RtpOverUdp::send(RtpPacket& packet)
{
#if 0
    static FILE *debugf = fopen("./udpstream.rtp", "wb");
    if (debugf != NULL)
    {
        fwrite(packet.getHeader(), 1, packet.getHeaderLength(), debugf);
        fwrite(packet.getPayload(), 1, packet.getPayloadLength(), debugf);
        fflush(debugf);
    }
#endif

    if (m_sockfd < 0)
    {
        printf("socket not connected\n");
        return false;
    }

    const uint8_t* header = packet.getHeader();
    const uint8_t* payload = packet.getPayload();
    uint16_t headerlen = packet.getHeaderLength();
    uint16_t payloadlen = packet.getPayloadLength();

    struct iovec iov[2];
    iov[0].iov_base = (void *)header;
    iov[0].iov_len = headerlen;
    iov[1].iov_base = (void *)payload;
    iov[1].iov_len = payloadlen;

    return writev(m_sockfd, iov, 2)
        == (ssize_t)(headerlen + payloadlen);
}

uint16_t RtpOverUdp::getEfficLen()
{
    return 1460;
}