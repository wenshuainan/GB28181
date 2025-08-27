#include <string.h>
#include <arpa/inet.h>
#include "RtpPayloadPS.h"

RtpPayloadPS::RtpPayloadPS(RtpParticipant *participant, int32_t maxLen)
    : RtpPayload(participant, maxLen)
{
    printf("++++++ RtpPayloadPS %p\n", this);
}

RtpPayloadPS::~RtpPayloadPS()
{
    printf("------ RtpPayloadPS %p\n", this);
}

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
        printf("invalid param %p %d\n", data, len);
        return 0;
    }

    if (data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x01 && data[3] == 0xBA)
    {
        m_formated.bFirst = true;

        /* 时间同步Pack header SCR */
        if (len > 8)
        {
            uint64_t SCR_base = 0;
            uint8_t bit3 = (data[4] & 0x38) >> 3;
            SCR_base |= bit3 << 30;
            uint16_t bit15 = (data[4] & 0x03) << 13;
            bit15 |= data[5] << 5;
            bit15 |= (data[6] & 0xF8) >> 3;
            SCR_base |= bit15 << 15;
            bit15 = 0;
            bit15 |= (data[6] & 0x03) << 13;
            bit15 |= data[7] << 5;
            bit15 |= (data[8] & 0xF8) >> 3;
            SCR_base |= bit15;
            m_formated.ms = SCR_base * 300 / 27000;
        }
    }

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
        pushFormated();
    }

    return pushedlen;
}