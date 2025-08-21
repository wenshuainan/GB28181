#include "RtpPayload.h"
#include "RtpPayloadAvc.h"
#include "RtpPayloadPS.h"

RtpPayload::RtpPayload(RtpParticipant *participant, int32_t maxLen)
    : m_participant(participant), m_maxLen(maxLen)
{}

RtpPayload::~RtpPayload()
{}

std::unique_ptr<RtpPayload> RtpPayload::create(RtpParticipant *participant, Type type, int32_t maxLen)
{
    RtpPayload *payload = nullptr;

    if (participant == nullptr)
    {
        return nullptr;
    }

    switch (type)
    {
    case H264:
        payload = new RtpPayloadAvc(participant, maxLen);
        break;
    case PS:
        payload = new RtpPayloadPS(participant, maxLen);
        break;
    
    default:
        break;
    }

    return std::unique_ptr<RtpPayload>(payload);
}