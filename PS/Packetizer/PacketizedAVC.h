#ifndef PACKETIZED_AVC_H
#define PACKETIZED_AVC_H

#include "PacketizedES.h"

class PacketizedAVC : public PES
{
private:
    uint8_t m_naluHeader;
    uint8_t m_cache[4];
    int32_t m_cacheLen;
    PSMux::Packet m_packet;
    uint64_t m_sysms;

public:
    PacketizedAVC(PSMux *mux);
    virtual ~PacketizedAVC();

private:
    void pushPacket(uint8_t naluType);
    int32_t packetizeFrame(const uint8_t *data, int32_t size);
    uint64_t getSystemMs();

public:
    int32_t packetized(const uint8_t *data, int32_t size);
    virtual void finished();
};

#endif