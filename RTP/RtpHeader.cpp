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
    data = std::shared_ptr<uint8_t>(new uint8_t[sizeof(Fixed)]);
    uint8_t* ptr = data.get();
    if (ptr == nullptr)
    {
        return nullptr;
    }

    uint16_t seq = htons(fixed.seq);
    memcpy(ptr + 2, &seq, 2);

    uint32_t ts = htonl(fixed.ts);
    memcpy(ptr + 4, &ts, 4);

    uint32_t SSRC = htonl(fixed.ssrc);
    memcpy(ptr + 8, &SSRC, 4);

    return ptr;
}

uint16_t RtpHeader::getLength() const
{
    return sizeof(Fixed);
}