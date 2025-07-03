#ifndef RTP_PAYLOAD_PS_H
#define RTP_PAYLOAD_PS_H

#include "RtpPayload.h"

class RtpPayloadPS : public RtpPayload
{
public:
    RtpPayloadPS();
    virtual ~RtpPayloadPS();

public:
    virtual int format(char *data, int len, RtpPacket& packet);
}

#endif