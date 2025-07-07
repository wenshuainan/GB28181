#ifndef RTP_NET_H
#define RTP_NET_H

#include "RtpPacket.h"

class RtpNet
{
public:
    enum Type
    {
        UDP,
        TCP_ACTIVE,
        TCP_PASSIVE,
    };

protected:
    int sockfd;
    int localPort;

public:
    RtpNet(int localPort = 0);
    virtual ~RtpNet();

public:
    virtual bool connect(char *ip, int port) = 0;
    virtual bool disconnect() = 0;
    virtual bool send(RtpPacket& packet) = 0;

public:
    static std::shared_ptr<RtpNet> create(Type type, int localPort = 0);
};

#endif