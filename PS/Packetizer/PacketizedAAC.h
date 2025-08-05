#ifndef PACKETIZED_AAC_H
#define PACKETIZED_AAC_H

#include "PacketizedES.h"

class PacketizedAAC : public PES
{
public:
    PacketizedAAC(PSMux *mux);
    ~PacketizedAAC();

public:
    virtual int32_t packetized(const uint8_t *data, int32_t size);
};

#endif