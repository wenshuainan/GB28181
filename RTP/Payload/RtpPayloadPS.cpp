#include <string.h>
#include <sys/time.h>
#include "RtpPayloadPS.h"

RtpPayloadPS::RtpPayloadPS(RtpParticipant *participant, int32_t maxLen)
    : RtpPayload(participant, maxLen)
{}  

RtpPayloadPS::~RtpPayloadPS()
{}

void RtpPayloadPS::pushFormated()
{
    m_participant->pushPayload(m_formated);
    m_formated.marker = 0;
    m_formated.bFirst = false;
    m_formated.payload = nullptr;
}

/*
 * 每次送进来的都是以 0x00 0x00 0x01 0xBA 开头的完整pack
 * 函数内部直接按照RTP maxlen进行分割
 */
int32_t RtpPayloadPS::format(const uint8_t *data, int32_t len)
{
    if (data == nullptr || len <= 0)
    {
        return 0;
    }

    m_formated.bFirst = true;
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    m_formated.tms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    int pushedlen = 0;
    while (pushedlen < len)
    {
        int32_t wr = m_maxLen > len - pushedlen ? len - pushedlen : m_maxLen;
        if (m_formated.payload == nullptr)
        {
            m_formated.payload = std::make_shared<std::vector<uint8_t>>();
        }
        m_formated.payload->resize(wr);
        memcpy(m_formated.payload->data(), data + pushedlen, wr);
        pushedlen += wr;
        if (m_formated.payload->size() >= (uint32_t)m_maxLen)
        {
            pushFormated();
        }
    }

    if (m_formated.payload != nullptr)
    {
        m_formated.marker = 1;
        pushFormated();
    }

    return pushedlen;
}