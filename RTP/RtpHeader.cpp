#include <string.h>
#include <arpa/inet.h>
#include "RtpHeader.h"

RtpHeader::RtpHeader(const Fixed& fixed)
    : fixed(fixed)
{}

RtpHeader::~RtpHeader()
{}

const uint8_t* RtpHeader::getData()
{
    uint16_t seq = htons(fixed.seq);
    memcpy(fixedData + 2, &seq, 2);

    uint32_t ts = htonl(fixed.ts);
    memcpy(fixedData + 4, &ts, 4);

    uint32_t SSRC = htonl(fixed.ssrc);
    memcpy(fixedData + 8, &SSRC, 4);

    return fixedData;
}

uint16_t RtpHeader::getLength() const
{
    return sizeof(Fixed);
}