#include <stdio.h>
#include <iostream>
#include "DevPlay.h"
#include "Infra3/Guard.h"

#define MIN(a, b) \
    (((a) < (b)) ? (a) : (b))

DevPlay::DevPlay(int32_t ch, Play::StreamType type)
    : Play(ch, type)
{
    m_frontPktRdLen = 0;
    m_videoEnc = nullptr;
}

DevPlay::~DevPlay()
{
    stop();
}

bool DevPlay::start()
{
    Hunter::Media::StreamType type;
    if (!convertStreamType(m_type, type))
    {
        return false;
    }

    m_videoEnc = Hunter::Component::getComponentInstance<Hunter::Media::IDevVideoEnc>(m_ch, type);
    if (m_videoEnc != nullptr)
    {
        return m_videoEnc->start(Hunter::Media::IDevVideoEnc::Proc(&DevPlay::onCapture, this));
    }
    return false;
}

bool DevPlay::stop()
{
    if (m_videoEnc != nullptr)
    {
        m_videoEnc->stop(Hunter::Media::IDevVideoEnc::Proc(&DevPlay::onCapture, this));
        m_videoEnc = nullptr;
    }
    return true;
}

int32_t DevPlay::getVideo(uint8_t *data, int32_t size)
{
    Hunter::Infra::CGuard guard(m_mutex);
    int32_t len = 0;
    while (len < size)
    {
        if (m_packets.empty())
        {
            break;
        }
        Hunter::Memory::CPacket& pkt = m_packets.front();
        int32_t rdlen = MIN(pkt.size() - m_frontPktRdLen, size - len);
        memcpy(data + len, pkt.getBuffer() + m_frontPktRdLen, rdlen);
        len += rdlen;
        m_frontPktRdLen += rdlen;
        if (m_frontPktRdLen >= pkt.size())
        {
            m_packets.pop();
            m_frontPktRdLen = 0;
        }
    }
    return len;
}

int32_t DevPlay::getAudio(uint8_t *data, int32_t size)
{
    return 0;
}

bool DevPlay::convertStreamType(Play::StreamType src, Hunter::Media::StreamType& dst)
{
    switch (src)
    {
    case Play::STREAM_MAIN:
        dst = Hunter::Media::StreamType::main;
        return true;
    case Play::STREAM_SUB1:
        dst = Hunter::Media::StreamType::extra1;
        return true;
    case Play::STREAM_SUB2:
        dst = Hunter::Media::StreamType::extra2;
        return true;

    default:
        return false;
    }
}

void DevPlay::onCapture(int ch, Hunter::Memory::CPacket& pkt)
{
    Hunter::Infra::CGuard guard(m_mutex);
    m_packets.push(pkt);
}