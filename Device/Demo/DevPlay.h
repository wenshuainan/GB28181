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
    virtual bool start();
    virtual bool stop();
    virtual int32_t getVideo(int32_t ch, uint8_t *data, int32_t size);
    virtual int32_t getAudio(int32_t ch, uint8_t *data, int32_t size);
};

#endif