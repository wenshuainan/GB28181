#include <string.h>
#include <unistd.h>
#include "PacketizedAVC.h"
#include "PSMux.h"

PacketizedAVC::PacketizedAVC(PSMux *mux)
    : PES(mux)
{
    m_naluHeader = 0;
    m_cacheLen = 0;
    m_packet.bKeyFrame = false;
    m_packet.stream_type = 0x1B; // H264 PSM.stream_type = 0x1B
    m_packet.pes = std::make_shared<PESPacket>(0xE0); // H264 PSM.stream_id = 0xE0
    m_packet.bFirst = true;
}

PacketizedAVC::~PacketizedAVC()
{}

void PacketizedAVC::pushPacket(uint8_t naluType)
{
    if (m_packet.pes == nullptr)
    {
        return;
    }

    switch (naluType)
    {
    case 5: //IDR
    case 6: //SEI
    case 7: //SPS
    case 8: //PPS
        m_packet.bKeyFrame = true;
        break;

    default:
        m_packet.bKeyFrame = false;
        break;
    }

    m_mux->pushVideoPES(m_packet);

    m_packet.bFirst = false;
    m_packet.bKeyFrame = false;

    usleep(40000);
}

int32_t PacketizedAVC::packetizeFrame(const uint8_t *data, int32_t size)
{
    if (data == nullptr || size <= 0)
    {
        return 0;
    }

    if ((uint32_t)m_cacheLen + size <= sizeof(m_cache))
    {
        memcpy(m_cache + m_cacheLen, data, size);
        m_cacheLen += size;
        return size;
    }

    if (m_cacheLen > 0)
    {
        uint8_t assemble[8] = {0};
        int32_t assembleLen = 0;

        memcpy(assemble, m_cache, m_cacheLen);
        assembleLen += m_cacheLen;

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
                    m_packet.pes->writeDataByte(assemble, i);
                }
                pushPacket(m_naluHeader & 0x1F);
                m_naluHeader = assemble[i+4];
                m_packet.pes = std::make_shared<PESPacket>(0xE0);
                m_packet.pes->writeDataByte(assemble + i, assembleLen - i);
                m_packet.bFirst = true;
                m_cacheLen = 0;

                return dataLen;
            }
        }

        int len = m_packet.pes->writeDataByte(m_cache, m_cacheLen);
        if (len < m_cacheLen)
        {
            pushPacket(m_naluHeader & 0x1F);
            m_packet.pes = std::make_shared<PESPacket>(0xE0);
            m_packet.pes->writeDataByte(m_cache + len, m_cacheLen - len);
        }
        m_cacheLen = 0;
    }

    if ((uint32_t)size <= sizeof(m_cache))
    {
        memcpy(m_cache, data, size);
        m_cacheLen = size;
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
            int lastNaluType = (m_naluHeader & 0x1F);
            int newNaluType = (data[parsed+4] & 0x1F);
            if (lastNaluType == 1 || newNaluType == 1) // P or B Frame，否则将IDR、SPS、PPS按照一个帧处理
            {
                break;
            }
            m_naluHeader = data[parsed+4];
        }
    }

    int len = 0;
    while (len < parsed)
    {
        int w = m_packet.pes->writeDataByte(data + len, parsed - len);
        if (w < parsed - len)
        {
            pushPacket(m_naluHeader & 0x1F);
            m_packet.pes = std::make_shared<PESPacket>(0xE0);
        }
        len += w;
    }

    if (parsed == size - 4)
    {
        memcpy(m_cache, data + parsed, 4);
        m_cacheLen = 4;
        parsed += 4;
    }
    else
    {
        pushPacket(m_naluHeader & 0x1F);
        m_naluHeader = data[parsed+4];
        m_packet.pes = std::make_shared<PESPacket>(0xE0);
        m_packet.pes->writeDataByte(data + parsed, 5);
        m_packet.bFirst = true;

        m_cacheLen = 0;
        parsed += 5;
    }

    return parsed;
}

int32_t PacketizedAVC::packetized(const uint8_t *data, int32_t size)
{
    if (data == nullptr || size <= 0)
    {
        return 0;
    }

    int32_t len = 0;
    while (len < size)
    {
        int32_t plen = packetizeFrame(data + len, size - len);
        if (plen <= 0)
        {
            break;
        }
        len += plen;
    }

    return len;
}