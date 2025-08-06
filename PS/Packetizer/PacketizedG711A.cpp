#include "PacketizedG711A.h"

PacketizedG711A::PacketizedG711A(PSMux *mux)
    : PES(mux)
{}

PacketizedG711A::~PacketizedG711A()
{}

int32_t PacketizedG711A::packetized(const uint8_t *data, int32_t size)
{
    PSMux::Packet packet;
    packet.bFirst = true;
    packet.bKeyFrame = true;
    packet.stream_type = 0x90; //Stream Type 0x90 - G711A
    packet.pes = std::make_shared<PESPacket>(0xC0);
    if (packet.pes)
    {
        packet.pes->writeDataByte(data, size);
        m_mux->pushAudioPES(packet);
        return size;
    }
    return 0;
}