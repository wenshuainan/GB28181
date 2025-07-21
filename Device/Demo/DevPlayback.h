#ifndef DEV_PLAYBACK_H
#define DEV_PLAYBACK_H

#include <stdio.h>
#include "Interface/9.8Playback.h"

class DevPlayback : public Playback
{
private:
    FILE *m_testVideo;

public:
    DevPlayback();
    virtual ~DevPlayback();

public:
    virtual bool seek(time_t timestamp);
    virtual int32_t read(uint8_t *data, int32_t size);
};

#endif