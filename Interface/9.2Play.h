#ifndef PLAY_INTERFACE_H
#define PLAY_INTERFACE_H

#include <stdint.h>
#include <time.h>
#include <memory>

class Play
{
public:
    Play() {}
    virtual ~Play() {}

public:
    virtual int32_t getVideo(int32_t ch, uint8_t *data, int32_t size) = 0;
    virtual int32_t getAudio(int32_t ch, uint8_t *data, int32_t size) = 0;
};

#endif