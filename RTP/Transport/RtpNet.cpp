#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
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

std::shared_ptr<RtpNet> RtpNet::create(Type type, int localPort)
{
    std::shared_ptr<RtpNet> net = nullptr;

    switch (type)
    {
    case UDP:
        net = std::make_shared<RtpOverUdp>();
        break;
    case TCP_ACTIVE:
        net = std::make_shared<RtpOverTcp>();
        break;
    case TCP_PASSIVE:
        break;
    
    default:
        break;
    }

    return net;
}

bool RtpNet::isConnected()
{
    return m_sockfd >= 0;
}

const std::string& RtpNet::getLocalIpv4()
{
    m_localIpv4 = "";

    if (!isConnected())
    {
        return m_localIpv4;
    }

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

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
    if (!isConnected())
    {
        return -1;
    }

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    // 获取套接字绑定的本地地址
    if (getsockname(m_sockfd, (struct sockaddr*)&addr, &addr_len) < 0) {
        perror("getsockname failed");
        return -1;
    }

    return ntohs(addr.sin_port);
}