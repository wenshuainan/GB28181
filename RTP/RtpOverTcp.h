#ifndef RTP_OVER_TCP_H
#define RTP_OVER_TCP_H

#include "RtpNet.h"

class RtpOverTcp : public RtpNet
{
private:
    int16_t length; // 2 bytes before packet
};

#endif