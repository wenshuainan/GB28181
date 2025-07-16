#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "RtpPayloadAvc.h"

RtpPayloadAvc::RtpPayloadAvc(RtpParticipant *participant, int32_t maxLen)
    : RtpPayload(participant, maxLen)
{
    m_formated.marker = 0;
    m_formated.bFirst = false;
    m_formated.payload = nullptr;
}

RtpPayloadAvc::~RtpPayloadAvc()
{}

uint8_t RtpPayloadAvc::makeFUAIndicator(uint8_t naluHeader)
{
    uint8_t indicator = 0;

    indicator = naluHeader & 0xE0;
    indicator |= 28; // FU-A = 28

    return indicator;
}

uint8_t RtpPayloadAvc::makeFUAHeader(uint8_t start, uint8_t end, uint8_t naluType)
{
    uint8_t header = 0;

    header = start << 7;
    header |= end << 6;
    header |= naluType;

    return header;
}

void RtpPayloadAvc::pushFomated()
{
    if (m_formated.payload == nullptr)
    {
        return;
    }

    m_formated.payload->at(0) = makeFUAIndicator(m_naluHeader);
    m_formated.payload->at(1) = makeFUAHeader(m_formated.bFirst ? 1 : 0, m_formated.marker, m_naluHeader & 0x1f);

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    m_formated.tms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    m_participant->pushPayload(m_formated);

    m_formated.bFirst = false;
    m_formated.marker = 0;

    usleep(40000);
}

int32_t RtpPayloadAvc::format(const uint8_t *data, int32_t len)
{
    if (data == nullptr || len <= 0)
    {
        return 0;
    }

    if ((uint32_t)m_cacheLen + len <= sizeof(m_cache))
    {
        memcpy(m_cache + m_cacheLen, data, len);
        m_cacheLen += len;
        return len;
    }

    if (m_cacheLen > 0)
    {
        uint8_t assemble[8] = {0};
        int32_t assembleLen = 0;

        memcpy(assemble, m_cache, m_cacheLen);
        assembleLen += m_cacheLen;

        int32_t dataLen = len > 4 ? 4 : len;
        memcpy(assemble + assembleLen, data, dataLen);
        assembleLen += dataLen;

        int i;
        for (i = 0; i < assembleLen - 4; i++)
        {
            if (assemble[i] == 0x00
                && assemble[i+1] == 0x00
                && assemble[i+2] == 0x00
                && assemble[i+3] == 0x01)
            {
                int j;
                for (j = 0; j < i; j++)
                {
                    if (m_formated.payload->size() >= (uint32_t)m_maxLen)
                    {
                        pushFomated();
                        m_formated.payload = std::make_shared<std::vector<uint8_t>>();
                        m_formated.payload->resize(2); //FUA indicator and header
                    }
                    m_formated.payload->push_back(assemble[j]);
                }
                m_formated.marker = 1;
                pushFomated();
                m_naluHeader = assemble[i+4];
                m_formated.bFirst = true;
                m_formated.payload = std::make_shared<std::vector<uint8_t>>();
                m_formated.payload->resize(2); //FUA indicator and header

                for (j = i + 5; j < assembleLen; j++)
                {
                    m_formated.payload->push_back(assemble[j]);
                }

                m_cacheLen = 0;

                return dataLen;
            }
        }

        for (i = 0; i < m_cacheLen; i++)
        {
            if (m_formated.payload->size() >= (uint32_t)m_maxLen)
            {
                pushFomated();
                m_formated.payload = std::make_shared<std::vector<uint8_t>>();
                m_formated.payload->resize(2); //FUA indicator and header
            }
            m_formated.payload->push_back(m_cache[i]);
        }

        m_cacheLen = 0;
    }

    if ((uint32_t)len <= sizeof(m_cache))
    {
        memcpy(m_cache, data, len);
        m_cacheLen = len;
        return len;
    }

    int parsed;
    for (parsed = 0; parsed < len - 4; parsed++)
    {
        if (data[parsed] == 0x00
            && data[parsed+1] == 0x00
            && data[parsed+2] == 0x00
            && data[parsed+3] == 0x01)
        {
            break;
        }
    }

    int32_t cpyed = 0;
    while (cpyed < parsed)
    {
        int32_t ori = m_formated.payload->size();
        int wr = parsed - cpyed;
        if (wr > m_maxLen - ori)
        {
            wr = m_maxLen - ori;
        }
        m_formated.payload->resize(ori + wr);
        memcpy(m_formated.payload->data() + ori, data + cpyed, wr);
        cpyed += wr;
        if (cpyed < parsed)
        {
            pushFomated();
            m_formated.payload = std::make_shared<std::vector<uint8_t>>();
            m_formated.payload->resize(2); //FUA indicator and header
        }
    }

    if (parsed == len - 4)
    {
        memcpy(m_cache, data + parsed, 4);
        m_cacheLen = 4;
        parsed += 4;
    }
    else
    {
        m_formated.marker = 1;
        pushFomated();
        m_naluHeader = data[parsed+4];
        m_formated.bFirst = true;
        m_formated.payload = std::make_shared<std::vector<uint8_t>>();
        m_formated.payload->resize(2); //FUA indicator and header

        // m_formated.payload->push_back(data[parsed+4]);
        m_cacheLen = 0;
        parsed += 5;
    }

    return parsed;
}
