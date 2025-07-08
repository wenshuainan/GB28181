#include <string.h>
#include <arpa/inet.h>
#include "RtpHeader.h"

RtpHeader::RtpHeader(const Fixed& fixed)
    : m_fixed(fixed)
{}

RtpHeader::~RtpHeader()
{}

const uint8_t* RtpHeader::getData()
{
    m_fixedData[0] = *((uint8_t *)&m_fixed);
    m_fixedData[1] = *((uint8_t *)&m_fixed + 1);

    uint16_t seq = htons(m_fixed.seq);
    memcpy(m_fixedData + 2, &seq, 2);

    uint32_t ts = htonl(m_fixed.ts);
    memcpy(m_fixedData + 4, &ts, 4);

    uint32_t SSRC = htonl(m_fixed.ssrc);
    memcpy(m_fixedData + 8, &SSRC, 4);

    return m_fixedData;
}

uint16_t RtpHeader::getLength() const
{
    return sizeof(Fixed);
}