#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>
#include "RtpOverUdp.h"

RtpOverUdp::RtpOverUdp()
{}

RtpOverUdp::~RtpOverUdp()
{}

bool RtpOverUdp::connect(char *ip, int port)
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        return false;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip);

    if (::connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        close(sockfd);
        sockfd = -1;
        return false;
    }

    return true;
}

bool RtpOverUdp::disconnect()
{
    if (sockfd > 0)
    {
        close(sockfd);
        sockfd = -1;
    }

    return true;
}

bool RtpOverUdp::send(RtpPacket& packet)
{
#if 0
    if (sockfd < 0)
    {
        return false;
    }

    const uint8_t* header = packet.getHeader();
    const uint8_t* payload = packet.getPayload();
    uint16_t hlen = packet.getHeaderLength();
    uint16_t plen = packet.getPayloadLength();

    struct iovec iov[2];
    iov[0].iov_base = (void *)header;
    iov[0].iov_len = hlen;
    iov[1].iov_base = (void *)payload;
    iov[1].iov_len = plen;

    ssize_t sent = writev(sockfd, iov, 2);
    return sent == (ssize_t)(hlen + plen);
#else
    static FILE* file = fopen("./rtp_over_udp.log", "a");
    if (file)
    {
        fwrite(packet.getHeader(), 1, packet.getHeaderLength(), file);
        fwrite(packet.getPayload(), 1, packet.getPayloadLength(), file);
        fflush(file);
    }
    return false; // For debugging purposes, always return false
#endif
}