#include "RtpPacket.h"

RtpPacket::RtpPacket(RtpHeader& fixed, int size)
    : header(fixed)
{
    len = 0;
    bFinished = false;
    if (payload = new char[size])
    {
        this->size = size;
    }
}

RtpPacket::~RtpPacket()
{
    if (payload)
    {
        delete[] payload;
        payload = NULL;
    }
}

char* RtpPacket::getPayload()
{
    return payload;
}

bool RtpPacket::write(char *data, int len)
{
    if (len > size - this->len)
    {
        return false;
    }

    memcpy(payload + this->len, data, len);
    this->len += len;

    return true;
}

bool RtpPacket::write(char *data, int len, int offset)
{
    if (len > size - this->len - offset)
    {
        return false;
    }

    memcpy(payload + offset, data, len);

    return true;
}

int RtpPacket::getFreeSpace()
{
    return size - len;
}

void RtpPacket::setFinished()
{
    if (len == 0)
    {
        return;
    }

    bFinished = true;
}

bool RtpPacket::isFinished()
{
    return bFinished;
}

bool RtpPacket::setIov(struct iovec& iov, int& iovcnt);
{
    return false;
}