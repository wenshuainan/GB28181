#ifndef DOWNLOAD_INTERFACE_H
#define DOWNLOAD_INTERFACE_H

#include <stdint.h>
#include <time.h>

class Download
{
protected:
    int32_t m_ch;

public:
    Download(int32_t ch) : m_ch(ch) {}
    virtual ~Download() {}

public:
    virtual bool seek(time_t timestamp) = 0;
    virtual int32_t readVideo(uint8_t *data, int32_t size) = 0;
    virtual int32_t readAudio(uint8_t *data, int32_t size) = 0;
};

#endif