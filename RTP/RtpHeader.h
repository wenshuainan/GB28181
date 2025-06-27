#ifndef RTP_HEADER_H
#define RTP_HEADER_H

class RtpHeader
{
public:
    struct Fixed
    {
    };

private:
    Fixed fixed;

public:
    RtpHeader(const Fixed& fixed);
    ~RtpHeader();

    /* 输出网络字节序 */
    const char* getNetBytes();

    void setMarker(int marker = 1);
};

#endif