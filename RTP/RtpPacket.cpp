#include "RtpPacket.h"

RtpPacket::RtpPacket(const RtpHeader::Fixed& fixed, std::shared_ptr<std::vector<uint8_t>> payload)
    : header(fixed), payload(payload)
{}

RtpPacket::~RtpPacket()
{}