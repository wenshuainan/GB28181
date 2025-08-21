#ifndef DEV_PLAYBACK_H
#define DEV_PLAYBACK_H

#include <stdio.h>
#include "Interface/9.8Playback.h"

class DevPlayback : public Playback
{
private:
    FILE *m_testVideo;

public:
    DevPlayback(int32_t ch);
    virtual ~DevPlayback();

public:
    virtual void setTime(time_t start, time_t end); // 设置播放起点和终点
    virtual bool start();
    virtual bool stop();
    virtual int32_t readVideo(uint8_t *data, int32_t size);
    virtual int32_t readAudio(uint8_t *data, int32_t size);
    virtual bool isFileEnd();
    virtual PES::ES_TYPE getVideoType();
    virtual PES::ES_TYPE getAudioType();

    /* MANSRTSP 媒体回放控制 */
    virtual bool play(); // 从当前位置开始播放
    virtual bool play(int32_t start); // 从距起点start秒开始播放
    virtual bool play(int32_t start, int32_t end); // 范围播放，倒放时start > end
    virtual bool pause(); // 暂停播放
    virtual bool teardown(); // 停止播放
    virtual bool setSpeed(float speed); // 快进/慢放，倒放时speed < 0
};

#endif