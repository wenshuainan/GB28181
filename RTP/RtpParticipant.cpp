#include <random>
#include "RtpParticipant.h"

RtpParticipant::RtpParticipant(Participant& participant)
{}

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
{}

bool RtpParticipant::start()
{}

bool RtpParticipant::stop()
{}