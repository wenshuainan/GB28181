#include <stdio.h>
#include <unistd.h>
#include <random>
#include "RtpParticipant.h"

RtpParticipant::RtpParticipant(Participant& participant)
{
    m_bConnected = false;
    m_payloadType = participant.payloadType;
    m_SSRC = participant.SSRC;
    m_destination = participant.destination;

    m_net = std::shared_ptr<RtpNet>(RtpNet::create(participant.netType, participant.destination.port));
    if (m_net != nullptr)
    {
        m_payloadFormat = RtpPayload::create(this, m_payloadType, m_net->getEfficLen());
    }
}

RtpParticipant::~RtpParticipant()
{
    disconnect();
}

uint16_t RtpParticipant::makeRandom()
{
    std::random_device rd;  // 用于获取随机种子
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 引擎
    std::uniform_int_distribution<> distrib(0, 65535); // 定义分布范围
    return distrib(gen); // 生成随机数
}

void RtpParticipant::process()
{
    RtpHeader::Fixed fixed = {
        .cc = 0,
        .x = 0,
        .p = 0,
        .v = 2,
        .pt = m_payloadType,
        .m = 0,
        .seq = makeRandom(),
        .ts = 0,
        .ssrc = m_SSRC
    };

    while (m_bConnected)
    {
        std::lock_guard<std::mutex> guard(m_queMutex);
        if (m_formatedQue.empty())
        {
            guard.~lock_guard();
            usleep(10000);
            continue;
        }

        Formated formated = m_formatedQue.front();
        m_formatedQue.pop();
        guard.~lock_guard();

        if (formated.bFirst)
        {
            fixed.ts = formated.tms * 90;
        }
        if (formated.marker)
        {
            fixed.m = 1;
        }
        RtpPacket packet(fixed, formated.payload);

        m_net->send(packet);

        fixed.seq++;
        fixed.m = 0;
    }
}

bool RtpParticipant::pushPayload(const Formated& formated)
{
    std::lock_guard<std::mutex> guard(m_queMutex);
    m_formatedQue.push(formated);
    return true;
}

int32_t RtpParticipant::transport(const uint8_t *data, int32_t len)
{
    if (m_payloadFormat == nullptr)
    {
        return 0;
    }

    int32_t tlen = 0;
    while (tlen < len)
    {
        int flen = m_payloadFormat->format(data + tlen, len - tlen);
        if (flen <= 0)
        {
            break;
        }
        tlen += flen;
    }
    
    return tlen;
}

bool RtpParticipant::connect()
{
    if (m_net == nullptr || m_net->isConnected())
    {
        return false;
    }
    if (m_destination.ipv4.empty() || m_destination.port <= 0)
    {
        return false;
    }

    bool connected = m_net->connect(m_destination.ipv4, m_destination.port);
    if (connected)
    {
        m_bConnected = true;
        m_thread = std::make_shared<std::thread>(&RtpParticipant::process, this);
    }

    return connected;
}

bool RtpParticipant::disconnect()
{
    if (m_net == nullptr || !m_net->isConnected())
    {
        return false;
    }

    m_bConnected = false;
    if (m_thread && m_thread->joinable())
    {
        m_thread->join();
    }

    return m_net->disconnect();
}

const char* RtpParticipant::getLocalIpv4()
{
    if (m_net == nullptr)
    {
        return "";
    }

    return m_net->getLocalIpv4().c_str();
}

int32_t RtpParticipant::getLocalPort() const
{
    if (m_net == nullptr)
    {
        return -1;
    }

    return m_net->getLocalPort();
}

uint32_t RtpParticipant::getSSRC() const
{
    return m_SSRC;
}

const char* RtpParticipant::getTransportType() const
{
    if (m_net != nullptr)
    {
        return m_net->getType();
    }
    else
    {
        return "";
    }
}
