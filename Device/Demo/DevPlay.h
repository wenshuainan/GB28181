#ifndef DEV_PLAY_H
#define DEV_PLAY_H

#include "Interface/9.2Play.h"

class DevPlay : public Play
{
private:
    FILE *m_testVideo;
    FILE *m_testAudio;

public:
    DevPlay(int32_t ch);
    virtual ~DevPlay();

public:
    virtual bool start();
    virtual bool stop();
    virtual int32_t getVideo(uint8_t *data, int32_t size);
    virtual int32_t getAudio(uint8_t *data, int32_t size);
    virtual PES::ES_TYPE getVideoType();
    virtual PES::ES_TYPE getAudioType();
};

#endif