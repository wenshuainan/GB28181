#ifndef DEV_DOWNLOAD_H
#define DEV_DOWNLOAD_H

#include <stdio.h>
#include "Interface/9.9Download.h"

class DevDownload : public Download
{
private:
    FILE *m_testVideo;

public:
    DevDownload();
    virtual ~DevDownload();

public:
    virtual bool seek(int32_t ch, time_t timestamp);
    virtual int32_t read(int32_t ch, uint8_t *data, int32_t size);
};

#endif