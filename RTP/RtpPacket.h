#ifndef RTP_PACKET_H
#define RTP_PACKET_H

#include <cstdint>
#include <vector>
#include <memory>
#include "RtpHeader.h"

class RtpPacket
{
private:
    RtpHeader header;
    std::shared_ptr<std::vector<uint8_t>> payload;

public:
    RtpPacket();
    ~RtpPacket();
};

#endif