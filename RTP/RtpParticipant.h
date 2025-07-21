#ifndef RTP_PARTICIPANT_H
#define RTP_PARTICIPANT_H

#include <queue>
#include <memory>
#include <thread>
#include <string>
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
            std::string ipv4;
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
    std::shared_ptr<RtpNet> m_net;
    std::shared_ptr<RtpPayload> m_payloadFormat;
    std::queue<Formated> m_formatedQue;
    std::shared_ptr<std::thread> m_thread;
    bool m_bConnected;
    RtpPayload::Type m_payloadType;
    Participant::Destination m_destination;
    uint32_t m_SSRC;
    
public:
    RtpParticipant(Participant& participant);
    ~RtpParticipant();

private:
    uint16_t makeRandom();
    void process();

public:
    bool pushPayload(const Formated& formated);

public:
    int32_t transport(const uint8_t *data, int32_t len);
    bool connect();
    bool disconnect();
    const char* getLocalIpv4();
    int32_t getLocalPort() const;
    uint32_t getSSRC() const;
    const char* getTransportType() const;
};

#endif