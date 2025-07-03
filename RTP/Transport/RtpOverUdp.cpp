#include "RtpOverUdp.h"

RtpOverUdp::RtpOverUdp()
{}

RtpOverUdp::~RtpOverUdp()
{}

bool RtpOverUdp::framing()
{
    return true;
}

bool RtpOverUdp::connect(char *ip, int port)
{}

bool RtpOverUdp::disconnect()
{}

bool RtpOverUdp::send(RtpPacket *packet)
{}