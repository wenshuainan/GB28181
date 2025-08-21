#ifndef PLAYBACK_INTERFACE_H
#define PLAYBACK_INTERFACE_H

#include <stdint.h>
#include <time.h>
#include "PS/Packetizer/PacketizedES.h"

class Playback
{
protected:
    int32_t m_ch;
    time_t m_playStart; // INVITE SDP t=
    time_t m_playEnd; // INVITE SDP t=
    time_t m_rangeStart; // INFO Range
    time_t m_rangeEnd; // INFO Range
    float m_speed;

public:
    Playback(int32_t ch)
        : m_ch(ch)
        , m_playStart(0)
        , m_playEnd(0)
        , m_rangeStart(0)
        , m_rangeEnd(0)
        , m_speed(1.0)
    {}
    virtual ~Playback() {}

public:
    virtual void setTime(time_t start, time_t end) = 0; // 设置播放起点和终点
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual int32_t readVideo(uint8_t *data, int32_t size) = 0;
    virtual int32_t readAudio(uint8_t *data, int32_t size) = 0;
    virtual bool isFileEnd() = 0;
    virtual PES::ES_TYPE getVideoType() = 0;
    virtual PES::ES_TYPE getAudioType() = 0;

    /* MANSRTSP 媒体回放控制 */
    virtual bool play() = 0; // 从当前位置开始播放
    virtual bool play(int32_t start) = 0; // 从距起点start秒开始播放
    virtual bool play(int32_t start, int32_t end) = 0; // 范围播放，倒放时start > end
    virtual bool pause() = 0; // 暂停播放
    virtual bool teardown() = 0; // 停止播放
    virtual bool setSpeed(float speed) = 0; // 快进/慢放，倒放时speed < 0
};

#endif