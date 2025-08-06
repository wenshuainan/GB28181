#ifndef PS_MUX_H
#define PS_MUX_H

#include <vector>
#include <queue>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include "Semantics.h"

class PSCallback
{
public:
    PSCallback() {}
    virtual ~PSCallback() {}
    virtual void onProgramStream(const uint8_t *data, int32_t size) = 0;
};

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
    std::queue<Packet> m_videoStream;
    std::queue<Packet> m_audioStream;
    bool m_bRunning;
    std::thread *m_thread;
    std::mutex m_vMutex;
    std::mutex m_aMutex;

private:
    PSCallback *m_callback;

public:
    PSMux(PSCallback *callback);
    ~PSMux();

private:
    void sendPack(const std::shared_ptr<Pack>& pack);
    void multiplexed();

public:
    bool pushVideoPES(const Packet& packet);
    bool pushAudioPES(const Packet& packet);
};

#endif