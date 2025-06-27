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

protected:
    virtual bool framing() = 0;

public:
    virtual bool connect(char *ip, int port, Type type) = 0; //打开socket
    virtual bool disconnect() = 0; //关闭socket
    virtual bool send(RtpPacket *packet) = 0;
};

#endif