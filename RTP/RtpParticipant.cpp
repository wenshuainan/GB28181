#include "RtpParticipant.h"

RtpParticipant::RtpParticipant(Participant& participant)
{
    packet = nullptr;
}

RtpParticipant::~RtpParticipant()
{}

bool RtpParticipant::inputData(RtpPayload::Type type, char *data, int len)
{
    int formatLen = 0;
    
    while (formatLen < len)
    {
        if (packet == nullptr)
        {
            packet = std::make_shared<RtpPacket>();
        }

        int flen = payload->format(data + formatLen, len - formatLen, packet);
        if (flen < 0)
        {
            return false;
        }

        if (packet->isFinished())
        {
            packets.push_back(packet);
            packet = nullptr;
        }

        formatLen += flen;
    }

    return true;
}

bool RtpParticipant::inputFrame(RtpPayload::Type type, char *frame, int len)
{
    return false;
}