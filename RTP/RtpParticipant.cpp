#include <stdio.h>
#include <unistd.h>
#include <random>
#include "RtpParticipant.h"

RtpParticipant::RtpParticipant(Participant& participant)
{
    bRunning = false;
    payloadType = participant.payloadType;
    SSRC = participant.SSRC;
    destination = participant.destination;

    net = std::shared_ptr<RtpNet>(RtpNet::create(participant.netType, participant.destination.port));

    switch (payloadType)
    {
    case RtpPayload::Type::PS:
        payloadFormat = std::shared_ptr<RtpPayload>(RtpPayload::create(this, RtpPayload::Type::PS));
        break;
    case RtpPayload::Type::H264:
        payloadFormat = std::shared_ptr<RtpPayload>(RtpPayload::create(this, RtpPayload::Type::H264));
        break;
    
    default:
        break;
    }
}

RtpParticipant::~RtpParticipant()
{}

uint16_t RtpParticipant::genRandom()
{
    std::random_device rd;  // 用于获取随机种子
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 引擎
    std::uniform_int_distribution<> distrib(0, 65535); // 定义分布范围
    return distrib(gen); // 生成随机数
}

void RtpParticipant::process()
{
    RtpHeader::Fixed fixed =
    {
        .cc = 0,
        .x = 0,
        .p = 0,
        .v = 2,
        .pt = payloadType,
        .m = 0,
        .seq = genRandom(),
        .ts = 0,
        .ssrc = SSRC
    };

    while (bRunning)
    {
        if (formatedQue.empty())
        {
            usleep(10000);
            continue;
        }

        Formated formated = formatedQue.front();
        formatedQue.pop();

        if (formated.bFirst)
        {
            fixed.ts = formated.tms * 90;
        }
        if (formated.marker)
        {
            fixed.m = 1;
        }
        RtpPacket packet(fixed, formated.payload);

        net->send(packet);

        fixed.seq++;
        fixed.m = 0;
    }
}

bool RtpParticipant::pushPayload(const Formated& formated)
{
    formatedQue.push(formated);
    return true;
}

int32_t RtpParticipant::format(const uint8_t *data, int32_t len)
{
    if (payloadFormat == nullptr)
    {
        return 0;
    }

    return payloadFormat->format(data, len);
}

bool RtpParticipant::connect()
{
    if (net == nullptr || net->isConnected())
    {
        return false;
    }
    if (destination.ipv4.empty() || destination.port <= 0)
    {
        return false;
    }

    bool ret = net->connect(destination.ipv4, destination.port);
    if (ret)
    {
        bRunning = true;
        thread = new std::thread(&RtpParticipant::process, this);
    }

    return ret;
}

bool RtpParticipant::disconnect()
{
    if (net == nullptr || !net->isConnected())
    {
        return false;
    }

    bool ret = net->disconnect();
    if (ret)
    {
        bRunning = false;
        if (thread && thread->joinable())
        {
            thread->join();
        }
        delete thread;
        thread = nullptr;
    }

    return ret;
}

const char* RtpParticipant::getLocalIpv4()
{
    if (net == nullptr)
    {
        return "";
    }

    return net->getLocalIpv4().c_str();
}

int32_t RtpParticipant::getLocalPort() const
{
    if (net == nullptr)
    {
        return -1;
    }

    return net->getLocalPort();
}
