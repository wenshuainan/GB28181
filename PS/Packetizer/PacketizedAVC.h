#ifndef PACKETIZED_AVC_H
#define PACKETIZED_AVC_H

#include "PacketizedES.h"

class PacketizedAVC : public PES
{
private:
    uint8_t naluHeader;
    bool bNaluHeaderNextTime;
    uint8_t cache[3];
    int32_t cacheLen;
    PSMux::Packet packet;

public:
    PacketizedAVC(PSMux *mux);
    virtual ~PacketizedAVC();

private:
    void pushPacket(uint8_t naluType);

public:
    int32_t packetized(uint8_t *data, int32_t size);
};

#endif