#ifndef DEV_PLAY_H
#define DEV_PLAY_H

#include "9.2Play.h"

class DevPlay : public Play
{
private:
    FILE *m_testVideo;
    uint8_t m_videoBuf[1024];

public:
    DevPlay();
    virtual ~DevPlay();

public:
    virtual bool getVideo(Coded &coded);
    virtual bool getAudio(Coded &coded);
    virtual bool putCoded(const Coded &coded);
};

#endif