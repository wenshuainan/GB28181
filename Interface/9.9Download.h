#ifndef DOWNLOAD_INTERFACE_H
#define DOWNLOAD_INTERFACE_H

#include <stdint.h>
#include <time.h>

class Download
{
public:
    Download() {}
    virtual ~Download() {}

public:
    virtual bool seek(time_t timestamp) = 0;
    virtual int32_t read(uint8_t *data, int32_t size) = 0;
};

#endif