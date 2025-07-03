#ifndef RTP_PAYLOAD_H
#define RTP_PAYLOAD_H

#include <vector>
#include <memory>
#include "RtpPacket.h"

class RtpPayload
{
public:
    enum Type
    {
        PS = 96,
        H264 = 98,
        H265 = 100,
        G711A = 8,
        AAC = 102,
        UNKNOWN = 128,
    };

protected:
    Type type;

public:
    RtpPayload();
    virtual ~RtpPayload();

public:
    static RtpPayload *create(Type type);

public:
    virtual int format(char *data, int len, RtpPacket& packet) = 0;
};

#endif