#include "RtpPacket.h"

RtpPacket::RtpPacket(const RtpHeader::Fixed& fixed, std::shared_ptr<std::vector<uint8_t>> payload)
    : header(fixed), payload(payload)
{}

RtpPacket::~RtpPacket()
{}

const uint8_t* RtpPacket::getHeader()
{
    return header.getData();
}

const uint8_t* RtpPacket::getPayload()
{
    if (payload == nullptr || payload->empty())
    {
        return nullptr;
    }

    return payload->data();
}

uint16_t RtpPacket::getHeaderLength() const
{
    return header.getLength();
}

uint16_t RtpPacket::getPayloadLength() const
{
    if (payload == nullptr)
    {
        return 0;
    }

    return payload->size();
}