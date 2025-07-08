#include "RtpPacket.h"

RtpPacket::RtpPacket(const RtpHeader::Fixed& fixed, std::shared_ptr<std::vector<uint8_t>> payload)
    : m_header(fixed), m_payload(payload)
{}

RtpPacket::~RtpPacket()
{}

const uint8_t* RtpPacket::getHeader()
{
    return m_header.getData();
}

const uint8_t* RtpPacket::getPayload()
{
    if (m_payload == nullptr || m_payload->empty())
    {
        return nullptr;
    }

    return m_payload->data();
}

uint16_t RtpPacket::getHeaderLength() const
{
    return m_header.getLength();
}

uint16_t RtpPacket::getPayloadLength() const
{
    if (m_payload == nullptr)
    {
        return 0;
    }

    return m_payload->size();
}