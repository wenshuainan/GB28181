#ifndef RTP_OVER_UDP_H
#define RTP_OVER_UDP_H

#include "RtpNet.h"

class RtpOverUdp : public RtpNet
{
public:
    RtpOverUdp(int localPort = 60000);
    virtual ~RtpOverUdp();

public:
    virtual bool connect(const std::string& ipv4, int port);
    virtual bool disconnect();
    virtual bool send(RtpPacket& packet);
    virtual uint16_t getMTU();
    virtual const char* getType() const;
    virtual bool isConnected();
};

#endif