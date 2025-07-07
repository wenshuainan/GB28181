#include <string.h>
#include "RtpPayloadPS.h"

RtpPayloadPS::RtpPayloadPS(RtpParticipant *participant, int32_t maxLen)
    : RtpPayload(participant, maxLen)
{}  

RtpPayloadPS::~RtpPayloadPS()
{}

int32_t RtpPayloadPS::format(const uint8_t *data, int32_t len)
{
    if (data == nullptr || len <= 0)
    {
        return 0;
    }

    if (data[0] == 0x00
        && data[1] == 0x00
        && data[2] == 0x01
        && data[3] == 0xBA)
    {
        if (formated.payload != nullptr)
        {
            formated.marker = 1;
            participant->pushPayload(formated);
        }

        formated.bFirst = true;
        formated.marker = 0;
        formated.payload = std::make_shared<std::vector<uint8_t>>();
    }
    else
    {
        if (formated.payload != nullptr)
        {
            participant->pushPayload(formated);
        }

        formated.payload = std::make_shared<std::vector<uint8_t>>();
    }

    int pushedlen = 0;
    while (pushedlen < len)
    {
        int32_t wr = maxLen > len - pushedlen ? len - pushedlen : maxLen;
        formated.payload->resize(wr);
        memcpy(formated.payload->data(), data + pushedlen, wr);
        pushedlen += wr;
        if (pushedlen < len)
        {
            participant->pushPayload(formated);
            formated.payload = std::make_shared<std::vector<uint8_t>>();
        }
    }

    return len;
}