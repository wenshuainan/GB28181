#ifndef RTP_HEADER_H
#define RTP_HEADER_H

class RtpHeader
{
public:
    struct Fixed
    {
        unsigned char cc : 4;
        unsigned char x : 1;
        unsigned char p : 1;
        unsigned char v : 2;
        unsigned char pt : 7;
        unsigned char m : 1;
        unsigned short seq;
        unsigned int ts;
        unsigned int ssrc;
    };

private:
    Fixed fixed;

public:
    RtpHeader(const Fixed& fixed);
    ~RtpHeader();

    /* 输出网络字节序 */
    const char* getNetBytes();

    void setMarker(int marker = 1);

    int getLen();
};

#endif