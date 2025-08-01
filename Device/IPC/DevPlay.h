#ifndef DEV_PLAY_H
#define DEV_PLAY_H

#include <queue>
#include "Interface/9.2Play.h"
#include "Memory/Packet.h"
#include "Media3/DevVideoEnc.h"
#include "Media3/DevAudioEnc.h" //reserved for audio encoder in future.
#include "Infra3/Mutex.h"

class DevPlay : public Play
{
private:
    std::queue<Hunter::Memory::CPacket> m_packets;
    int32_t m_frontPktRdLen;
    Hunter::Media::IDevVideoEncPtr m_videoEnc;
    Hunter::Infra::CMutex m_mutex;

public:
    DevPlay(int32_t ch, Play::StreamType type = Play::STREAM_MAIN);
    virtual ~DevPlay();

public:
    virtual bool start();
    virtual bool stop();
    virtual int32_t getVideo(uint8_t *data, int32_t size);
    virtual int32_t getAudio(uint8_t *data, int32_t size);

private:
    bool convertStreamType(Play::StreamType src, Hunter::Media::StreamType& dst);
    void onCapture(int ch, Hunter::Memory::CPacket& pkt);
};

#endif