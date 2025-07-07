#ifndef RTP_PAYLOAD_PS_H
#define RTP_PAYLOAD_PS_H

#include "RtpPayload.h"
#include "RtpParticipant.h"

class RtpPayloadPS : public RtpPayload
{
private:
    RtpParticipant::Formated formated;

public:
    RtpPayloadPS(RtpParticipant *participant, int32_t maxLen = 1400);
    virtual ~RtpPayloadPS();

public:
    virtual int32_t format(const uint8_t *data, int32_t len);
};

#endif