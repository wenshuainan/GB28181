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
    virtual bool seek(time_t timestamp);
    virtual int32_t readVideo(uint8_t *data, int32_t size);
    virtual int32_t readAudio(uint8_t *data, int32_t size);
};

#endif