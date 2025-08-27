#ifndef RTP_NET_H
#define RTP_NET_H

#include <string>
#include "../RtpPacket.h"

class RtpNet
{
public:
    enum Type
    {
        UDP,
        TCP_ACTIVE,
        // TCP_PASSIVE, // 附录L 只对流媒体服务器之间要求
    };

protected:
    int32_t m_sockfd;
    std::string m_localIpv4;
    int32_t m_localPort;

public:
    RtpNet(int localPort = 60000);
    virtual ~RtpNet();

public:
    virtual bool connect(const std::string& ipv4, int port) = 0;
    virtual bool disconnect() = 0;
    virtual bool send(RtpPacket& packet) = 0;
    virtual uint16_t getMTU() = 0;
    virtual const char* getType() const = 0;

public:
    static std::unique_ptr<RtpNet> create(Type type, int localPort = 0);
    virtual bool isConnected();
    virtual const std::string& getLocalIpv4();
    virtual int32_t getLocalPort();
};

#endif