#include <string.h>
#include "PacketizedAVC.h"
#include "PSMux.h"

PacketizedAVC::PacketizedAVC(PSMux *mux)
    : PES(mux)
{
    naluHeader = 0;
    bNaluHeaderNextTime = false;
    cacheLen = 0;
    packet.bFirst = false;
    packet.bKeyFrame = false;
    packet.stream_type = 0x1B;
    packet.pes = nullptr;
}

PacketizedAVC::~PacketizedAVC()
{}

void PacketizedAVC::pushPacket(uint8_t naluType)
{
    if (packet.pes == nullptr)
    {
        return;
    }

    switch (naluType)
    {
    case 5: //IDR
    case 7: //SPS
    case 8: //PPS
        packet.bKeyFrame = true;
        break;
    
    default:
        packet.bKeyFrame = false;
        break;
    }

    mux->pushVideoPES(packet);

    packet.bFirst = false;
    packet.bKeyFrame = false;
}

int32_t PacketizedAVC::packetized(uint8_t *data, uint32_t size)
{
    int parsed;

    if (size + cacheLen <= sizeof(cache))
    {
        memcpy(cache + cacheLen, data, size);
        cacheLen += size;
        return size;
    }
    else if (cacheLen == sizeof(cache))
    {
        if (size > 0
            && cache[0] == 0x00 && cache[1] == 0x00 && cache[2] == 0x00
            && data[0] == 0x01)
        {
            pushPacket(naluHeader & 0x1F);
            packet.pes = std::make_shared<PESPacket>();
            packet.pes->writeDataByte(cache, cacheLen);
            packet.pes->writeDataByte(data, 1);
            packet.bFirst = true;
            cacheLen = 0;
            return 1;
        }
        else if (size > 1
            && cache[1] == 0x00 && cache[2] == 0x00
            && data[0] == 0x00 && data[1] == 0x01)
        {
            pushPacket(naluHeader & 0x1F);
            packet.pes = std::make_shared<PESPacket>();
            packet.pes->writeDataByte(cache, cacheLen);
            packet.pes->writeDataByte(data, 2);
            packet.bFirst = true;
            cacheLen = 0;
            return 2;
        }
        else if (size > 2
            && cache[2] == 0x00
            && data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x01)
        {
            pushPacket(naluHeader & 0x1F);
            packet.pes = std::make_shared<PESPacket>();
            packet.pes->writeDataByte(cache, cacheLen);
            packet.pes->writeDataByte(data, 3);
            packet.bFirst = true;
            cacheLen = 0;
            return 3;
        }
        else
        {
            int len = packet.pes->writeDataByte(cache, cacheLen);
            if (len < cacheLen)
            {
                pushPacket(naluHeader & 0x1F);
                packet.pes = std::make_shared<PESPacket>();
                packet.pes->writeDataByte(cache + len, cacheLen - len);
            }
            cacheLen = 0;
        }
    }

    if (bNaluHeaderNextTime)
    {
        naluHeader = data[0];
    }

    for (parsed = 0; parsed < size - 3; parsed++)
    {
        if (data[parsed] == 0x00
            && data[parsed+1] == 0x00
            && data[parsed+2] == 0x00
            && data[parsed+3] == 0x01)
        {
            break;
        }
    }

    int len = 0;
    while (len < parsed)
    {
        int w = packet.pes->writeDataByte(data + len, parsed - len);
        if (w < parsed - len)
        {
            pushPacket(naluHeader & 0x1F);
            packet.pes = std::make_shared<PESPacket>();
        }
        len += w;
    }

    if (parsed == size - 3)
    {
        memcpy(cache, data + parsed, 3);
        cacheLen = 3;
    }
    else
    {
        pushPacket(naluHeader & 0x1F);
        packet.pes = std::make_shared<PESPacket>();
        packet.pes->writeDataByte(data + parsed, 4);
        packet.bFirst = true;

        if (parsed + 4 < size)
        {
            naluHeader = data[parsed+4];
            bNaluHeaderNextTime = false;
        }
        else
        {
            bNaluHeaderNextTime = true;
        }

        parsed += 4;
    }

    return parsed;
}
