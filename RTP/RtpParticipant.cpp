#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <random>
#include "RtpParticipant.h"

RtpParticipant::RtpParticipant(Participant& participant)
    : m_bConnected(false)
{
    printf("++++++ RtpParticipant %p\n", this);
    m_payloadType = participant.payloadType;
    m_SSRC = participant.SSRC;
    m_destination = participant.destination;

    m_net = RtpNet::create(participant.netType, participant.destination.port);
    if (m_net != nullptr)
    {
        m_payloadFormat = RtpPayload::create(this, m_payloadType, m_net->getMTU());
    }
}

RtpParticipant::~RtpParticipant()
{
    printf("------ RtpParticipant %p\n", this);
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
        .ts = makeRandom(),
        .ssrc = m_SSRC
    };

    uint64_t lastms = 0;
    prctl(PR_SET_NAME, "RTPParticipant");

    while (m_bConnected)
    {
        m_mutex.lock();
        if (m_formated.empty())
        {
            m_mutex.unlock();
            usleep(10000);
            continue;
        }

        Formated formated = m_formated.front();
        m_formated.pop();
        m_mutex.unlock();

        if (formated.bFirst)
        {
            uint32_t delta = formated.ms - lastms;
            fixed.ts += (delta > 1000 ? 0 : delta) * 90;
            lastms = formated.ms;
        }
        if (formated.marker)
        {
            fixed.m = 1;
        }
        RtpPacket packet(fixed, formated.payload);

        if (!m_net->send(packet))
        {
            printf("rtp %p network send data failed errno=%d\n", this, errno);
            break;
        }

        fixed.seq++;
        fixed.m = 0;
    }
}

bool RtpParticipant::pushPayload(const Formated& formated)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_formated.push(formated);
    return true;
}

int32_t RtpParticipant::transport(const uint8_t *data, int32_t len)
{
    if (m_payloadFormat == nullptr)
    {
        printf("rtp %p payload format null\n", this);
        return -1;
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
    printf("rtp connect %p\n", this);
    if (m_net == nullptr || m_net->isConnected())
    {
        printf("failed %p:%p\n", this, m_net.get());
        return false;
    }
    if (m_destination.ipv4.empty() || m_destination.port <= 0)
    {
        printf("invalid dst %p\n", this);
        return false;
    }

    bool connected = m_net->connect(m_destination.ipv4, m_destination.port);
    if (connected)
    {
        printf("connect success %p\n", this);
        m_bConnected = true;
        m_thread = std::move(std::unique_ptr<std::thread>(new std::thread(&RtpParticipant::process, this)));
    }

    return connected;
}

bool RtpParticipant::disconnect()
{
    printf("rtp disconnect %p\n", this);
    if (m_net == nullptr || !m_net->isConnected())
    {
        printf("failed %p:%p\n", this, m_net.get());
        return false;
    }

    printf("wait thread join %p\n", this);
    m_bConnected = false;
    if (m_thread && m_thread->joinable())
    {
        m_thread->join();
        printf("thread joined %p\n", this);
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
