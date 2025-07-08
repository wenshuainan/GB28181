#ifndef RTP_PACKET_H
#define RTP_PACKET_H

#include <cstdint>
#include <vector>
#include <memory>
#include "RtpHeader.h"

class RtpPacket
{
private:
    RtpHeader m_header;
    std::shared_ptr<std::vector<uint8_t>> m_payload;

public:
    RtpPacket(const RtpHeader::Fixed& fixed, std::shared_ptr<std::vector<uint8_t>> payload);
    ~RtpPacket();

public:
    const uint8_t* getHeader();
    const uint8_t* getPayload();
    uint16_t getHeaderLength() const;
    uint16_t getPayloadLength() const;
};

#endif