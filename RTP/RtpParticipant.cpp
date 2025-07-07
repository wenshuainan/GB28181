#include <stdio.h>
#include <unistd.h>
#include <random>
#include "RtpParticipant.h"

RtpParticipant::RtpParticipant(Participant& participant)
{
    bRunning = false;
    payloadType = participant.payloadType;
    SSRC = participant.SSRC;

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
        .v = 2,
        .p = 0,
        .x = 0,
        .cc = 0,
        .m = 0,
        .pt = payloadType,
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
            fixed.ts = formated.tms / 90;
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

bool RtpParticipant::start()
{
    if (bRunning)
    {
        return false;
    }

    bRunning = true;
    thread = new std::thread(&RtpParticipant::process, this);
    return true;
}

bool RtpParticipant::stop()
{
    if (!bRunning)
    {
        return false;
    }

    bRunning = false;
    if (thread->joinable())
    {
        thread->join();
    }
    delete thread;
    thread = nullptr;

    return true;
}