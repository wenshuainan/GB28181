#ifndef PACK_AVC_H
#define PACK_AVC_H

#include "PackElementaryStream.h"

class PackAVC : public PackES
{
private:
    uint8_t naluHeader;

public:
    PackAVC();
    virtual ~PackAVC();

public:
    void pack(uint8_t *data, uint32_t size);
};

#endif