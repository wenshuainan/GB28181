#ifndef RTP_PAYLOAD_H
#define RTP_PAYLOAD_H

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
    Type type; //媒体类型
    char *unit; //可以进行分包的数据单元缓存
    int unitLen; //可以进行分包的数据单元长度

public:
    RtpPayload();
    virtual ~RtpPayload();

public:
    static RtpPayload *create(Type type);

public:
    virtual bool format(RtpPacket *packet) = 0;
};

#endif