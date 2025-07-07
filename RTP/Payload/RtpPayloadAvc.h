#ifndef RTP_PAYLOAD_AVC_H
#define RTP_PAYLOAD_AVC_H

#include "RtpPayload.h"
#include "RtpParticipant.h"

class RtpPayloadAvc : public RtpPayload
{
private:
    uint8_t naluHeader;
    uint8_t cache[4];
    int32_t cacheLen;
    RtpParticipant::Formated formated;

public:
    RtpPayloadAvc(RtpParticipant *participant, int32_t maxLen = 1400);
    virtual ~RtpPayloadAvc();

private:
    uint8_t makeFUAIndicator(uint8_t naluHeader);
    uint8_t makeFUAHeader(uint8_t start, uint8_t end, uint8_t naluType);
    void pushFomated();

public:
    virtual int32_t format(const uint8_t *data, int32_t len);
};

#endif