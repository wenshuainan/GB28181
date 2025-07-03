#ifndef RTP_OVER_UDP_H
#define RTP_OVER_UDP_H

#include "RtpNet.h"

class RtpOverUdp : public RtpNet
{
public:
    RtpOverUdp();
    virtual ~RtpOverUdp();

protected:
    virtual bool framing();

public:
    virtual bool connect(char *ip, int port);
    virtual bool disconnect();
    virtual bool send(RtpPacket *packet);
};

#endif