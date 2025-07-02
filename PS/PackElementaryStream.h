#ifndef PACK_ELEMENTARY_STREAM_H
#define PACK_ELEMENTARY_STREAM_H

#include "ProgramStream.h"
#include "PackProgramStream.h"

class PackES
{
public:
    enum ES_TYPE
    {
        AVC,
        HEVC,
        AAC,
        G711A,
    };

protected:
    PackProgramStream *programStream;

public:
    PackES();
    virtual ~PackES();

public:
    static PackES* create(ES_TYPE type);

public:
    virtual int32_t pack(uint8_t *data, uint32_t size) = 0;
};

#endif