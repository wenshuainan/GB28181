#include "RtpPayload.h"
#include "RtpPayloadAvc.h"
#include "RtpPayloadPS.h"

RtpPayload::RtpPayload(RtpParticipant *participant, int32_t maxLen)
    : participant(participant), maxLen(maxLen)
{}

RtpPayload::~RtpPayload()
{}

std::shared_ptr<RtpPayload> RtpPayload::create(RtpParticipant *participant, Type type, int32_t maxLen)
{
    std::shared_ptr<RtpPayload> payload = nullptr;

    if (participant == nullptr)
    {
        return nullptr;
    }

    switch (type)
    {
    case H264:
        payload = std::make_shared<RtpPayloadAvc>(participant, maxLen);
        break;
    case PS:
        payload = std::make_shared<RtpPayloadPS>(participant, maxLen);
        break;
    
    default:
        break;
    }

    return payload;
}