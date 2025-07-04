#ifndef RTP_PAYLOAD_PS_H
#define RTP_PAYLOAD_PS_H

#include "RtpPayload.h"

class RtpPayloadPS : public RtpPayload
{
public:
    RtpPayloadPS(RtpParticipant *participant, int32_t maxLen = 1400);
    virtual ~RtpPayloadPS();

public:
    virtual int32_t format(uint8_t *data, int32_t len);
};

#endif