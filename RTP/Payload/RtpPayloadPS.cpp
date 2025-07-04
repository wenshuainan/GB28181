#include "RtpPayloadPS.h"

RtpPayloadPS::RtpPayloadPS(RtpParticipant *participant, int32_t maxLen)
    : RtpPayload(participant, maxLen)
{}  

RtpPayloadPS::~RtpPayloadPS()
{}

int32_t RtpPayloadPS::format(uint8_t *data, int32_t len)
{
    return 0;
}