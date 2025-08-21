#ifndef PACKETIZED_HEVC_H
#define PACKETIZED_HEVC_H

#include "PacketizedES.h"

class PacketizedHEVC : public PES
{
private:
    uint8_t m_naluHeader[2];
    uint8_t m_cache[5];
    int32_t m_cacheLen;
    PSMux::Packet m_packet;

public:
    PacketizedHEVC(PSMux *mux);
    virtual ~PacketizedHEVC();

private:
    uint8_t calcNaluType(const uint8_t nalHeader[2]);
    void pushPacket(uint8_t naluType);
    int32_t packetizeFrame(const uint8_t *data, int32_t size);

public:
    int32_t packetized(const uint8_t *data, int32_t size);
    virtual void finished();
};

#endif