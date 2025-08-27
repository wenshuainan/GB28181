#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>
#include "RtpOverUdp.h"

RtpOverUdp::RtpOverUdp(int localPort)
    : RtpNet(localPort)
{
    printf("++++++ RtpOverUdp %p\n", this);
}

RtpOverUdp::~RtpOverUdp()
{
    printf("------ RtpOverUdp %p\n", this);
    disconnect();
}

bool RtpOverUdp::connect(const std::string& ipv4, int port)
{
    printf("udp connect %p [%s:%d]\n", this, ipv4.c_str(), port);
    m_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_sockfd < 0)
    {
        printf("failed socket %p\n", this);
        return false;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ipv4.c_str());

    if (::connect(m_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("failed connect %p\n", this);
        close(m_sockfd);
        m_sockfd = -1;
        return false;
    }

    return true;
}

bool RtpOverUdp::disconnect()
{
    printf("udp disconnect %p\n", this);
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
    static FILE *file = fopen("./udpstream.rtp", "wb");
    if (file != NULL)
    {
        fwrite(packet.getHeader(), 1, packet.getHeaderLength(), file);
        fwrite(packet.getPayload(), 1, packet.getPayloadLength(), file);
        fflush(file);
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

    if (writev(m_sockfd, iov, 2) < 0)
    {
        return errno == 0 || errno == EINTR; // EINTR不认为失败
    }
    return true;
}

uint16_t RtpOverUdp::getMTU()
{
    return 1460;
}

const char* RtpOverUdp::getType() const
{
    return "RTP/AVP";
}