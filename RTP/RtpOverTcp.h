#ifndef RTP_OVER_TCP_H
#define RTP_OVER_TCP_H

#include "RtpNet.h"

class RtpOverTcp : public RtpNet
{
private:
    unsigned short length;

public:
    RtpOverTcp();
    virtual ~RtpOverTcp();

protected:
    virtual bool framing();

public:
    virtual bool connect(char *ip, int port, Type type);
    virtual bool disconnect();
    virtual bool send(RtpPacket *packet);
};

#endif