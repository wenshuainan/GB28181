#ifndef PLAYBACK_INTERFACE_H
#define PLAYBACK_INTERFACE_H

#include <stdint.h>
#include <time.h>

class Playback
{
public:
    Playback() {}
    virtual ~Playback() {}

public:
    virtual bool seek(time_t timestamp) = 0;
    virtual int32_t read(uint8_t *data, int32_t size) = 0;
};

#endif