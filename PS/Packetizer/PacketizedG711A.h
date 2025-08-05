#ifndef PACKETIZED_G711A_H
#define PACKETIZED_G711A_H

#include "PacketizedES.h"

class PacketizedG711A : public PES
{
public:
    PacketizedG711A(PSMux *mux);
    ~PacketizedG711A();

public:
    virtual int32_t packetized(const uint8_t *data, int32_t size);
};

#endif