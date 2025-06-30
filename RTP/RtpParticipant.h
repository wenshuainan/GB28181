#ifndef RTP_PARTICIPANT_H
#define RTP_PARTICIPANT_H

#include <vector>
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
    
private:
    std::shared_ptr<RtpNet> net;
    std::shared_ptr<RtpPayload> payload;
    std::vector<std::shared_ptr<RtpPacket>> packets;
    std::shared_ptr<RtpPacket> packet;

public:
    RtpParticipant(Participant& participant);
    ~RtpParticipant();
    
    bool inputData(RtpPayload::Type type, char *data, int len);
    bool inputFrame(RtpPayload::Type type, char *frame, int len);
};

#endif