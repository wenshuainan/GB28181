#ifndef RTP_PAYLOAD_H
#define RTP_PAYLOAD_H

#include <vector>
#include <memory>

class RtpParticipant;

class RtpPayload
{
public:
    enum Type
    {
        PS = 96,
        H264 = 98,
        H265 = 100,
        G711A = 8,
        AAC = 102,
        UNKNOWN = 128,
    };

protected:
    Type m_type;
    RtpParticipant *m_participant;
    int32_t m_maxLen;

public:
    RtpPayload(RtpParticipant *participant, int32_t maxLen = 1400);
    virtual ~RtpPayload();

public:
    static std::shared_ptr<RtpPayload> create(RtpParticipant *participant, Type type, int32_t maxLen = 1400);

public:
    virtual int32_t format(const uint8_t *data, int32_t len) = 0;
};

#endif