#ifndef RTP_OVER_TCP_H
#define RTP_OVER_TCP_H

#include "RtpNet.h"

class RtpOverTcp : public RtpNet
{
public:
    RtpOverTcp();
    virtual ~RtpOverTcp();

public:
    virtual bool connect(const std::string& ipv4, int port);
    virtual bool disconnect();
    virtual bool send(RtpPacket& packet);
};

#endif