#ifndef PLAY_INTERFACE_H
#define PLAY_INTERFACE_H

#include <stdint.h>
#include <time.h>
#include <memory>
#include "PS/Packetizer/PacketizedES.h"

class Play
{
public:
    enum StreamType
    {
        STREAM_MAIN,
        STREAM_SUB1,
        STREAM_SUB2,
    };

protected:
    int32_t m_ch;
    StreamType m_type;

public:
    Play(int32_t ch, StreamType type = STREAM_SUB1) : m_ch(ch), m_type(type) {}
    virtual ~Play() {}

public:
    virtual void setStreamType(StreamType type) { m_type = type; }
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual int32_t getVideo(uint8_t *data, int32_t size) = 0;
    virtual int32_t getAudio(uint8_t *data, int32_t size) = 0;
    virtual PES::ES_TYPE getVideoType() = 0;
    virtual PES::ES_TYPE getAudioType() = 0;
};

#endif