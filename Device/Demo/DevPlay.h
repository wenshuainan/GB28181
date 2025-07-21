#ifndef DEV_PLAY_H
#define DEV_PLAY_H

#include "Interface/9.2Play.h"

class DevPlay : public Play
{
private:
    FILE *m_testVideo;

public:
    DevPlay();
    virtual ~DevPlay();

public:
    virtual int32_t getVideo(uint8_t *data, int32_t size);
    virtual int32_t getAudio(uint8_t *data, int32_t size);
};

#endif