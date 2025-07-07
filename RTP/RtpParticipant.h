#ifndef RTP_PARTICIPANT_H
#define RTP_PARTICIPANT_H

#include <queue>
#include <memory>
#include <thread>
#include "Transport/RtpNet.h"
#include "RtpPacket.h"
#include "Payload/RtpPayload.h"

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
        uint32_t SSRC;
    };

    struct Formated
    {
        bool bFirst;
        uint8_t marker;
        uint32_t tms; //毫秒时间
        std::shared_ptr<std::vector<uint8_t>> payload;
    };
    
private:
    std::shared_ptr<RtpNet> net;
    std::shared_ptr<RtpPayload> payloadFormat;
    std::queue<Formated> formatedQue;
    std::thread *thread;
    bool bRunning;
    RtpPayload::Type payloadType;
    uint32_t SSRC;
    
public:
    RtpParticipant(Participant& participant);
    ~RtpParticipant();

private:
    uint16_t genRandom();
    void process();

public:
    bool pushPayload(const Formated& formated);

public:
    int32_t format(const uint8_t *data, int32_t len);
    bool start();
    bool stop();
};

#endif