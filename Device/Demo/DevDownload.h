#ifndef DEV_DOWNLOAD_H
#define DEV_DOWNLOAD_H

#include <stdio.h>
#include "Interface/9.9Download.h"

class DevDownload : public Download
{
private:
    FILE *m_testVideo;

public:
    DevDownload(int32_t ch);
    virtual ~DevDownload();

public:
    virtual void setTime(time_t start, time_t end);
    virtual bool setSpeed(int speed);
    virtual uint64_t getFileSize();
    virtual bool start();
    virtual bool stop();
    virtual int32_t readVideo(uint8_t *data, int32_t size);
    virtual int32_t readAudio(uint8_t *data, int32_t size);
    virtual bool isFileEnd();
    virtual PES::ES_TYPE getVideoType();
    virtual PES::ES_TYPE getAudioType();
};

#endif