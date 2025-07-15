#include <string.h>
#include <sys/time.h>
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
        if (m_formated.payload != nullptr)
        {
            m_formated.marker = 1;
            m_participant->pushPayload(m_formated);
        }

        m_formated.bFirst = true;
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        m_formated.tms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        m_formated.marker = 0;
        m_formated.payload = std::make_shared<std::vector<uint8_t>>();
    }
    else
    {
        if (m_formated.payload != nullptr)
        {
            m_participant->pushPayload(m_formated);
        }

        m_formated.payload = std::make_shared<std::vector<uint8_t>>();
    }

    int pushedlen = 0;
    while (pushedlen < len)
    {
        int32_t wr = m_maxLen > len - pushedlen ? len - pushedlen : m_maxLen;
        m_formated.payload->resize(wr);
        memcpy(m_formated.payload->data(), data + pushedlen, wr);
        pushedlen += wr;
        if (pushedlen < len)
        {
            m_participant->pushPayload(m_formated);
            m_formated.payload = std::make_shared<std::vector<uint8_t>>();
        }
    }

    return len;
}