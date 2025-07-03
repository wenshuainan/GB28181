#ifndef PS_MUX_H
#define PS_MUX_H

#include <vector>
#include <queue>
#include <memory>
#include <functional>
#include <thread>
#include "Semantics.h"

class PSMux
{
public:
    struct Packet
    {
        bool bFirst;
        bool bKeyFrame;
        uint8_t stream_type;
        std::shared_ptr<PESPacket> pes;
    };

private:
    std::queue<Packet> videoStream;
    std::queue<Packet> audioStream;
    bool bRunning;
    std::thread *thread;

private:
    std::function<void (const uint8_t *, int32_t)> streamCallback;

public:
    PSMux();
    ~PSMux();

private:
    void sendPack(const std::shared_ptr<Pack>& pack);
    void multiplexed();

public:
    bool pushVideoPES(const Packet& packet);
    bool pushAudioPES(const Packet& packet);

public:
    void setStreamCallback(std::function<void (const uint8_t *, int32_t)> callback);
    bool start();
    bool stop();
};

#endif