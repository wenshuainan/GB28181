#ifndef RTP_PAYLOAD_AVC_H
#define RTP_PAYLOAD_AVC_H

#include "RtpPayload.h"

class RtpPayloadAvc : public RtpPayload
{
private:
    int startBit;
    int endBit;
    unsigned char naluHeader;
    bool bNaluHeaderNextTime;
    char lastTimeTail[3];
    int tailLen;

public:
    RtpPayloadAvc();
    virtual ~RtpPayloadAvc();

private:
    unsigned char makeFUAIndicator();
    unsigned char makeFUAHeader();
    bool makeFUA(RtpPacket& packet);

public:
    virtual int format(char *data, int len, RtpPacket& packet);
};

#endif