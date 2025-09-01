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
    std::unique_ptr<RtpPayload> payload;

    if (participant == nullptr)
    {
        return nullptr;
    }

    switch (type)
    {
    case H264:
        payload = std::move(std::unique_ptr<RtpPayload>(new RtpPayloadAvc(participant, maxLen)));
        break;
    case PS:
        payload = std::move(std::unique_ptr<RtpPayload>(new RtpPayloadPS(participant, maxLen)));
        break;
    
    default:
        break;
    }

    return payload;
}