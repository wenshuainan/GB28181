#ifndef RTP_HEADER_H
#define RTP_HEADER_H

#include <cstdint>
#include <memory>

class RtpHeader
{
public:
    struct Fixed
    {
        uint8_t v : 2;
        uint8_t p : 1;
        uint8_t x : 1;
        uint8_t cc : 4;
        uint8_t m : 1;
        uint8_t pt : 7;
        uint16_t seq;
        uint32_t ts;
        uint32_t ssrc;
    };

private:
    Fixed fixed;
    uint8_t fixedData[sizeof(Fixed)];

public:
    RtpHeader(const Fixed& fixed);
    ~RtpHeader();

public:
    const uint8_t* getData();
    uint16_t getLength() const;
};

#endif