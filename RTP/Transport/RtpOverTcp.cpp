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
{
    printf("++++++ RtpOverTcp %p\n", this);
}

RtpOverTcp::~RtpOverTcp()
{
    printf("------ RtpOverTcp %p\n", this);
    disconnect();
}

bool RtpOverTcp::connect(const std::string& ipv4, int port)
{
    printf("tcp connect %p [%s:%d]\n", this, ipv4.c_str(), port);
    std::lock_guard<std::mutex> guard(m_mutex);
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0)
    {
        printf("create socket failed %p\n", this);
        return false;
    }

    if (!bind())
    {
        close(m_sockfd);
        m_sockfd = -1;
        return false;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ipv4.c_str());

    /*  设置非阻塞，超时时间内未连接成功，返回错误 */
    int flags = fcntl(m_sockfd, F_GETFL, 0);
    fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK);

    int err = ::connect(m_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (err < 0)
    {
        printf("tcp connect %d:%d\n", m_sockfd, errno);
        if (errno == EINPROGRESS)
        {
            struct timeval tval = {
                .tv_sec = 2,
                .tv_usec = 0
            };
            fd_set wset;
            FD_ZERO(&wset);
            FD_SET(m_sockfd, &wset);
            int serr = select(m_sockfd + 1, NULL, &wset, NULL, &tval);
            printf("tcp connect select %d:%d\n", m_sockfd, serr);
            if (serr > 0 && FD_ISSET(m_sockfd, &wset))
            {
                int optval = -1;
                socklen_t optlen = sizeof(optval);
                if (getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) == 0
                    && optval == 0)
                {
                    err = 0;
                }
                printf("tcp connect getsockopt %d:%d\n", m_sockfd, optval);
            }
        }
    }

    printf("tcp nonblock connected %d:%d\n", m_sockfd, err);
    if (err == 0)
    {
        /* 连接成功，恢复阻塞模式 */
        fcntl(m_sockfd, F_SETFL, flags);

        /* 设置发送超时 */
        struct timeval tv = {.tv_sec = 2, .tv_usec = 0};
        if (setsockopt(m_sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == 0)
        {
            return true;
        }
        printf("tcp set socket opt SO_SNDTIMEO failed errno=%d\n", errno);
    }

    close(m_sockfd);
    m_sockfd = -1;
    return false;
}

bool RtpOverTcp::disconnect()
{
    printf("tcp disconnect %p:%d\n", this, m_sockfd);
    std::lock_guard<std::mutex> guard(m_mutex);
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
    static FILE *file = fopen("./tcpstream.rtp", "wb");
    if (file != NULL)
    {
        fwrite(packet.getHeader(), 1, packet.getHeaderLength(), file);
        fwrite(packet.getPayload(), 1, packet.getPayloadLength(), file);
        fflush(file);
    }
#endif

    std::lock_guard<std::mutex> guard(m_mutex);
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

    if (writev(m_sockfd, iov, 3) < 0)
    {
        printf("tcp writev failed errno=%d\n", errno);
        if (errno == 0 || errno == EINTR) // EINTR不认为失败
        {
            return true;
        }
        else
        {
            /* 设置LINGER，防止长时间处于FIN_WAIT影响后续重连 */
            printf("something bad happened, gonna setsockopt SO_LINGER\n");
            struct linger opt = {.l_onoff = 1, .l_linger = 0};
            setsockopt(m_sockfd, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt));
            return false;
        }
    }
    return true;
}

uint16_t RtpOverTcp::getMTU()
{
    return (uint16_t)0xFFFF - 12;
}

const char* RtpOverTcp::getType() const
{
    return "TCP/RTP/AVP";
}

bool RtpOverTcp::isConnected()
{
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_sockfd >= 0;
}