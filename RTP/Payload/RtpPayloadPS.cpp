#include "RtpPayloadPS.h"

RtpPayloadPS::RtpPayloadPS()
{}

RtpPayloadPS::~RtpPayloadPS()
{}

int RtpPayloadPS::format(char *data, int len, RtpPacket& packet)
{
    int payloadlen = packet.write(data, len);

    return payloadlen;
}