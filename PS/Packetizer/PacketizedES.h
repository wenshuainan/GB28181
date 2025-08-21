#ifndef PACKETIZED_ELEMENTARY_STREAM_H
#define PACKETIZED_ELEMENTARY_STREAM_H

#include <memory>
#include "../Semantics.h"
#include "../PSMux.h"

class PES
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
    PSMux *m_mux;

public:
    PES(PSMux *mux);
    virtual ~PES();

public:
    static std::unique_ptr<PES> create(ES_TYPE type, PSMux *mux);

public:
    virtual int32_t packetized(const uint8_t *data, int32_t size) = 0;
    virtual void finished() = 0;
};

#endif