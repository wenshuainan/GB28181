#ifndef PLAYBACK_INTERFACE_H
#define PLAYBACK_INTERFACE_H

#include <stdint.h>
#include <time.h>

class Playback
{
protected:
    int32_t m_ch;

public:
    Playback(int32_t ch) : m_ch(ch) {}
    virtual ~Playback() {}

public:
    virtual bool seek(time_t timestamp) = 0;
    virtual int32_t readVideo(uint8_t *data, int32_t size) = 0;
    virtual int32_t readAudio(uint8_t *data, int32_t size) = 0;
};

#endif