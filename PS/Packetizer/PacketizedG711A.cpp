#include "PacketizedG711A.h"

PacketizedG711A::PacketizedG711A(PSMux *mux)
    : PES(mux)
{}

PacketizedG711A::~PacketizedG711A()
{}

int32_t PacketizedG711A::packetized(const uint8_t *data, int32_t size)
{
    return 0;
}