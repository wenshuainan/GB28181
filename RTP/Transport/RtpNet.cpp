#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <memory>
#include "RtpNet.h"
#include "RtpOverTcp.h"
#include "RtpOverUdp.h"

RtpNet::RtpNet(int localPort)
{
    m_sockfd = -1;
    m_localIpv4 = "";
    m_localPort = localPort;
}

RtpNet::~RtpNet()
{}

bool RtpNet::bind()
{
    /* 地址复用 */
    int reuse = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        printf("setsockopt SO_REUSEADDR failed %d:%d\n", m_sockfd, errno);
        return false;
    }

    /* 绑定本地ip:port */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(m_localPort);

    if (::bind(m_sockfd, (struct sockaddr*)&addr, sizeof(addr)) == 0)
    {
        return true;
    }
    else
    {
        printf("bind failed %d:%d\n", m_sockfd, m_localPort);
        return false;
    }
}

std::unique_ptr<RtpNet> RtpNet::create(Type type, int localPort)
{
    std::unique_ptr<RtpNet> net;

    switch (type)
    {
    case UDP:
        net = std::move(std::unique_ptr<RtpNet>(new RtpOverUdp(localPort)));
        break;
    case TCP_ACTIVE:
        net = std::move(std::unique_ptr<RtpNet>(new RtpOverTcp(localPort)));
        break;
    
    default:
        break;
    }

    return net;
}

const std::string& RtpNet::getLocalIpv4()
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_sockfd < 0)
    {
        return m_localIpv4;
    }

    // 获取套接字绑定的本地地址
    if (getsockname(m_sockfd, (struct sockaddr*)&addr, &addr_len) < 0) {
        perror("getsockname failed");
        return m_localIpv4;
    }

    // 提取 IP 和端口
    char ip[16];
    inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
    m_localIpv4 = ip;

    return m_localIpv4;
}

int32_t RtpNet::getLocalPort()
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_sockfd < 0)
    {
        return m_localPort;
    }

    // 获取套接字绑定的本地地址
    if (getsockname(m_sockfd, (struct sockaddr*)&addr, &addr_len) < 0) {
        perror("getsockname failed");
        return -1;
    }
    m_localPort = ntohs(addr.sin_port);

    return m_localPort;
}
