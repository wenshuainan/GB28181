#ifndef RTP_PARTICIPANT_H
#define RTP_PARTICIPANT_H

#include <queue>
#include <memory>
#include "RtpNet.h"
#include "RtpPacket.h"
#include "RtpPayload.h"

class RtpParticipant
{
public:
    struct Participant
    {
        struct Destination
        {
            char ipv4[16];
            int port;
        } destination;
        RtpNet::Type netType;
        RtpPayload::Type payloadType;
    };

    struct Formated
    {
        bool bFirst;
        uint8_t marker;
        std::shared_ptr<std::vector<uint8_t>> payload;
    };
    
private:
    std::shared_ptr<RtpNet> net;
    std::queue<Formated> formated;

public:
    RtpParticipant(Participant& participant);
    ~RtpParticipant();

public:
    bool pushPayload(const Formated& formated);
    void send();
};

#endif