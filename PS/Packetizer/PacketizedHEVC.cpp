#include <string.h>
#include "PacketizedHEVC.h"
#include "PSMux.h"

PacketizedHEVC::PacketizedHEVC(PSMux *mux)
    : PES(mux)
{
    m_naluHeader[0] = m_naluHeader[1] = 0;
    m_cacheLen = 0;
    m_packet.bKeyFrame = false;
    m_packet.stream_type = 0x24; // H265 PSM.stream_type = 0x24
    m_packet.pes = std::make_shared<PESPacket>(0xE0); // H265 PSM.stream_id = 0xE0
    m_packet.bFirst = true;
}

PacketizedHEVC::~PacketizedHEVC()
{}

uint8_t PacketizedHEVC::calcNaluType(const uint8_t nalHeader[2])
{
    return ((nalHeader[0] & 0x7E) >> 1);
}

void PacketizedHEVC::pushPacket(uint8_t naluType)
{
    if (m_packet.pes == nullptr)
    {
        return;
    }

    switch (naluType)
    {
    case 19: //IDR
    case 32: //VPS
    case 33: //SPS
    case 34: //PPS
        m_packet.bKeyFrame = true;
        break;

    default:
        m_packet.bKeyFrame = false;
        break;
    }

    m_mux->pushVideoPES(m_packet);

    m_packet.bFirst = false;
    m_packet.bKeyFrame = false;
}

int32_t PacketizedHEVC::packetizeFrame(const uint8_t *data, int32_t size)
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
        uint8_t assemble[10] = {0};
        int32_t assembleLen = 0;

        memcpy(assemble, m_cache, m_cacheLen);
        assembleLen += m_cacheLen;

        int32_t dataLen = size > 5 ? 5 : size;
        memcpy(assemble + assembleLen, data, dataLen);
        assembleLen += dataLen;

        int i;
        for (i = 0; i < assembleLen - 5; i++)
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
                pushPacket(calcNaluType(m_naluHeader));
                m_naluHeader[0] = assemble[i+4];
                m_naluHeader[1] = assemble[i+5];
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
            pushPacket(calcNaluType(m_naluHeader));
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
    for (parsed = 0; parsed < size - 5; parsed++)
    {
        if (data[parsed] == 0x00
            && data[parsed+1] == 0x00
            && data[parsed+2] == 0x00
            && data[parsed+3] == 0x01)
        {
            int lastNaluType = calcNaluType(m_naluHeader);
            int newNaluType = calcNaluType(data + parsed + 4);
            if (lastNaluType == 1 || newNaluType == 1) // P or B Frame，否则将IDR、VPS、SPS、PPS按照一个帧处理
            {
                break;
            }
            m_naluHeader[0] = data[parsed+4];
            m_naluHeader[1] = data[parsed+5];
        }
    }

    int len = 0;
    while (len < parsed)
    {
        int w = m_packet.pes->writeDataByte(data + len, parsed - len);
        if (w < parsed - len)
        {
            pushPacket(calcNaluType(m_naluHeader));
            m_packet.pes = std::make_shared<PESPacket>(0xE0);
        }
        len += w;
    }

    if (parsed == size - 5)
    {
        memcpy(m_cache, data + parsed, 5);
        m_cacheLen = 5;
        parsed += 5;
    }
    else
    {
        pushPacket(calcNaluType(m_naluHeader));
        m_naluHeader[0] = data[parsed+4];
        m_naluHeader[1] = data[parsed+5];
        m_packet.pes = std::make_shared<PESPacket>(0xE0);
        m_packet.pes->writeDataByte(data + parsed, 6);
        m_packet.bFirst = true;

        m_cacheLen = 0;
        parsed += 6;
    }

    return parsed;
}

int32_t PacketizedHEVC::packetized(const uint8_t *data, int32_t size)
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