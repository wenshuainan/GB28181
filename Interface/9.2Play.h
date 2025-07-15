#ifndef PLAY_INTERFACE_H
#define PLAY_INTERFACE_H

#include <memory>

class Play
{
public:
    struct Coded
    {
        uint8_t *data;
        int32_t size;
        bool bFramed; // 完整一帧数据，标志是否需要再解析
        uint64_t userdata;
    };
    
public:
    Play() {}
    virtual ~Play() {}

public:
    virtual bool getVideo(Coded &coded) = 0;
    virtual bool getAudio(Coded &coded) = 0;
    virtual bool putCoded(const Coded &coded) = 0;
};

#endif