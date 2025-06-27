#ifndef RTP_PACKET_H
#define RTP_PACKET_H

#include <sys/uio.h>
#include "RtpHeader.h"

class RtpPacket
{
private:
    RtpHeader header;
    char *payload; // 数据区，直接指向RtpPlayload
    int capacity; // 负载容量，一般由MTU决定，payload根据此进行分包

public:
    RtpPacket(RtpHeader& header, int capacity);
    ~RtpPacket();

    void setPayload(char *payload, int len); // 设置负载，payload调用
    bool setIov(struct iovec& iov, int& iovcnt); // 设置iov，给网络发送
};

#endif