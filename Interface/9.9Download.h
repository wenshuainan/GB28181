#ifndef DOWNLOAD_INTERFACE_H
#define DOWNLOAD_INTERFACE_H

#include <stdint.h>
#include <time.h>
#include "PS/Packetizer/PacketizedES.h"

class Download
{
protected:
    int32_t m_ch;
    time_t m_downloadStart; // INVITE SDP t=
    time_t m_downloadEnd; // INVITE SDP t=
    int32_t m_speed; // INVITE SDP a=downloadspeed:

public:
    Download(int32_t ch)
        : m_ch(ch)
        , m_downloadStart(0)
        , m_downloadEnd(0)
        , m_speed(1)
    {}
    virtual ~Download() {}

public:
    virtual void setTime(time_t start, time_t end) = 0;
    virtual bool setSpeed(int speed) = 0;
    virtual uint64_t getFileSize() = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual int32_t readVideo(uint8_t *data, int32_t size) = 0;
    virtual int32_t readAudio(uint8_t *data, int32_t size) = 0;
    virtual bool isFileEnd() = 0;
    virtual PES::ES_TYPE getVideoType() = 0;
    virtual PES::ES_TYPE getAudioType() = 0;
};

#endif