#ifndef RTP_OVER_UDP_H
#define RTP_OVER_UDP_H

#include "RtpNet.h"

class RtpOverUdp : public RtpNet
{
public:
    RtpOverUdp();
    virtual ~RtpOverUdp();

public:
    virtual bool connect(const std::string& ipv4, int port);
    virtual bool disconnect();
    virtual bool send(RtpPacket& packet);
};

#endif