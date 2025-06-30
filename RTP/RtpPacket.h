#ifndef RTP_PACKET_H
#define RTP_PACKET_H

#include <sys/uio.h>
#include "RtpHeader.h"

class RtpPacket
{
private:
    RtpHeader header;
    char *payload; // 数据区，直接指向RtpPlayload
    int size; // 负载容量，一般由MTU决定，payload根据此进行分包
    int len; // 负载长度，由setPayload设置
    bool bFinished;

public:
    RtpPacket(RtpHeader& fixed, int size);
    ~RtpPacket();

    char *getPayload();
    bool write(char *data, int len);
    bool write(char *data, int len, int offset);
    int getFreeSpace();
    void setFinished();
    bool isFinished();

    bool setIov(struct iovec& iov, int& iovcnt); // 设置iov，给网络发送
};

#endif