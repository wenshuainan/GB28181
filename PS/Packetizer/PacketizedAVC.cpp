#include <string.h>
#include <unistd.h>
#include "PacketizedAVC.h"
#include "PSMux.h"

PacketizedAVC::PacketizedAVC(PSMux *mux)
    : PES(mux)
{
    naluHeader = 0;
    cacheLen = 0;
    packet.bKeyFrame = false;
    packet.stream_type = 0x1B; // H264 PSM.stream_type = 0x1B
    packet.pes = std::make_shared<PESPacket>(0xE0); // H264 PSM.stream_id = 0xE0
    packet.bFirst = true;
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
    case 6: //SEI
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

    usleep(40000); // debug printf >>>>>>
}

int32_t PacketizedAVC::packetized(uint8_t *data, int32_t size)
{
    if (data == nullptr || size <= 0)
    {
        return 0;
    }

    if (cacheLen + size <= sizeof(cache))
    {
        memcpy(cache + cacheLen, data, size);
        cacheLen += size;
        return size;
    }

    if (cacheLen > 0)
    {
        uint8_t assemble[8] = {0};
        int32_t assembleLen = 0;

        memcpy(assemble, cache, cacheLen);
        assembleLen += cacheLen;

        int32_t dataLen = size > 4 ? 4 : size;
        memcpy(assemble + assembleLen, data, dataLen);
        assembleLen += dataLen;

        int i;
        for (i = 0; i < assembleLen - 4; i++)
        {
            if (assemble[i] == 0x00
                && assemble[i+1] == 0x00
                && assemble[i+2] == 0x00
                && assemble[i+3] == 0x01)
            {
                if (i > 0)
                {
                    packet.pes->writeDataByte(assemble, i);
                }
                pushPacket(naluHeader & 0x1F);
                naluHeader = assemble[i+4];
                packet.pes = std::make_shared<PESPacket>(0xE0);
                packet.pes->writeDataByte(assemble + i, assembleLen - i);
                packet.bFirst = true;
                cacheLen = 0;

                return dataLen;
            }
        }

        int len = packet.pes->writeDataByte(cache, cacheLen);
        if (len < cacheLen)
        {
            pushPacket(naluHeader & 0x1F);
            packet.pes = std::make_shared<PESPacket>(0xE0);
            packet.pes->writeDataByte(cache + len, cacheLen - len);
        }
        cacheLen = 0;
    }

    if (size <= sizeof(cache))
    {
        memcpy(cache, data, size);
        cacheLen = size;
        return size;
    }

    int parsed;
    for (parsed = 0; parsed < size - 4; parsed++)
    {
        if (data[parsed] == 0x00
            && data[parsed+1] == 0x00
            && data[parsed+2] == 0x00
            && data[parsed+3] == 0x01)
        {
            int lastNaluType = (naluHeader & 0x1F);
            int newNaluType = (data[parsed+4] & 0x1F);
            if (lastNaluType == 1 || newNaluType == 1)
            {
                break;
            }
            naluHeader = data[parsed+4];
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
        naluHeader = data[parsed+4];
        packet.pes = std::make_shared<PESPacket>(0xE0);
        packet.pes->writeDataByte(data + parsed, 5);
        packet.bFirst = true;

        cacheLen = 0;
        parsed += 5;
    }

    return parsed;
}
