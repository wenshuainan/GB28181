#include <string.h>
#include <unistd.h>
#include "PacketizedAVC.h"
#include "PSMux.h"

PacketizedAVC::PacketizedAVC(PSMux *mux)
    : PES(mux)
{
    naluHeader = 0;
    cacheLen = 0;
    packet.bFirst = false;
    packet.bKeyFrame = false;
    packet.stream_type = 0x1B; // H264 PSM.stream_type = 0x1B
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

    // static int ii = 0;
    // printf(">>>>>> ii=%d len=%d\n", ++ii);

    // printf("typeeeeeeeeeeeeeeeeeeee %d\n", naluType);

    switch (naluType)
    {
    // case 5: //IDR
    case 7: //SPS
    // case 8: //PPS
        packet.bKeyFrame = true;
        break;

    case 6:
        return;
    
    default:
        packet.bKeyFrame = false;
        break;
    }

    // printf(">>>>>> %s:%d\n", __FILE__, __LINE__);
    mux->pushVideoPES(packet);

    packet.bFirst = false;
    packet.bKeyFrame = false;

    usleep(40000);
}

int32_t PacketizedAVC::packetized(uint8_t *data, int32_t size)
{
    if (size <= 0 || data == nullptr)
    {
        return 0;
    }

    if (size + cacheLen <= sizeof(cache))
    {
        memcpy(cache + cacheLen, data, size);
        cacheLen += size;
        return size;
    }
    else if (cacheLen == sizeof(cache))
    {
        if (cache[0] == 0x00 && cache[1] == 0x00 && cache[2] == 0x00 && cache[3] == 0x01)
        {
            pushPacket(naluHeader & 0x1F);
            packet.pes = std::make_shared<PESPacket>(0xE0);
            packet.pes->writeDataByte(cache, cacheLen);
            packet.pes->writeDataByte(data, 1);
            packet.bFirst = true;
            naluHeader = data[0];
            cacheLen = 0;
            return 1;
        }
        else if (size > 1
            && cache[1] == 0x00 && cache[2] == 0x00 && cache[3] == 0x00
            && data[0] == 0x01)
        {
            packet.pes->writeDataByte(cache, 1);
            pushPacket(naluHeader & 0x1F);
            packet.pes = std::make_shared<PESPacket>(0xE0);
            packet.pes->writeDataByte(cache + 1, cacheLen - 1);
            packet.pes->writeDataByte(data, 2);
            packet.bFirst = true;
            naluHeader = data[1];
            cacheLen = 0;
            return 2;
        }
        else if (size > 2
            && cache[2] == 0x00 && cache[3] == 0x00
            && data[0] == 0x00 && data[1] == 0x01)
        {
            packet.pes->writeDataByte(cache, 2);
            pushPacket(naluHeader & 0x1F);
            packet.pes = std::make_shared<PESPacket>(0xE0);
            packet.pes->writeDataByte(cache + 2, cacheLen - 2);
            packet.pes->writeDataByte(data, 3);
            packet.bFirst = true;
            naluHeader = data[2];
            cacheLen = 0;
            return 3;
        }
        else if (size > 3
            && cache[3] == 0x00
            && data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x01)
        {
            packet.pes->writeDataByte(cache, 3);
            pushPacket(naluHeader & 0x1F);
            packet.pes = std::make_shared<PESPacket>(0xE0);
            packet.pes->writeDataByte(cache + 3, cacheLen - 3);
            packet.pes->writeDataByte(data, 4);
            packet.bFirst = true;
            naluHeader = data[3];
            cacheLen = 0;
            return 4;
        }
        else
        {
            int len = packet.pes->writeDataByte(cache, cacheLen);
            if (len < cacheLen)
            {
                pushPacket(naluHeader & 0x1F);
                packet.pes = std::make_shared<PESPacket>(0xE0);
                packet.pes->writeDataByte(cache + len, cacheLen - len);
            }
            cacheLen = 0;
        }
    }

    int parsed;
    for (parsed = 0; parsed < size - 4; parsed++)
    {
        if (data[parsed] == 0x00
            && data[parsed+1] == 0x00
            && data[parsed+2] == 0x00
            && data[parsed+3] == 0x01)
        {
            int type = (data[parsed+4] & 0x1F);
            if (type != 5 && type != 8)
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
            packet.pes = std::make_shared<PESPacket>(0xE0);
        }
        len += w;
    }

    if (parsed == size - 4)
    {
        memcpy(cache, data + parsed, 4);
        cacheLen = 4;
        parsed += 4;
    }
    else
    {
        pushPacket(naluHeader & 0x1F);
        packet.pes = std::make_shared<PESPacket>(0xE0);
        packet.pes->writeDataByte(data + parsed, 5);
        packet.bFirst = true;

        naluHeader = data[parsed+4];
        cacheLen = 0;
        parsed += 5;
    }

    // printf(">>>>>>> size=%d\n", size);
    return parsed;
}
