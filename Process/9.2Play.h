#ifndef PLAY_PROCESS_H
#define PLAY_PROCESS_H

#include <memory>

class Play
{
public:
    Play();
    virtual ~Play();

public:
    virtual bool putVideoStream(void *data, int size) = 0;
    virtual bool putAudioStream(void *data, int size) = 0;
};

#endif