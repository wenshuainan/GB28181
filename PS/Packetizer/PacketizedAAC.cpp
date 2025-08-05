#include "PacketizedAAC.h"

PacketizedAAC::PacketizedAAC(PSMux *mux)
    : PES(mux)
{}

PacketizedAAC::~PacketizedAAC()
{}

int32_t PacketizedAAC::packetized(const uint8_t *data, int32_t size)
{
    PSMux::Packet packet;
    packet.bFirst = true;
    packet.bKeyFrame = true;
    packet.stream_type = 0x0F; //Stream Type 0x0F - AAC
    packet.pes = std::make_shared<PESPacket>(0xC0);
    if (packet.pes)
    {
        packet.pes->writeDataByte(data, size);
        m_mux->pushAudioPES(packet);
        return size;
    }
    return 0;
}