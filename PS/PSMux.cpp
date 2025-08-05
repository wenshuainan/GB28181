#include "PSMux.h"

PSMux::PSMux(PSCallback *callback)
{
    m_callback = callback;

    bRunning = true;
    thread = new std::thread(&PSMux::multiplexed, this);
}

PSMux::~PSMux()
{
    if (!bRunning)
    {
        return;
    }

    bRunning = false;
    if (thread != nullptr)
    {
        thread->join();
        delete thread;
        thread = nullptr;
    }
}

void PSMux::sendPack(const std::shared_ptr<Pack>& pack)
{
    if (pack != nullptr && m_callback != nullptr)
    {
        pack->toBitStream();
        const BitStream stream = pack->getBitStream();
        m_callback->onProgramStream(stream.getData(), stream.getLength());

        const std::vector<std::shared_ptr<PESPacket>>& PES_packet = pack->getPESPacket();
        for (auto it : PES_packet)
        {
            const BitStream pes = it->getBitStream();
            m_callback->onProgramStream(pes.getData(), pes.getLength());
        }
    }
}

void PSMux::multiplexed()
{
    std::shared_ptr<Pack> pack = nullptr;
    std::shared_ptr<SystemHeader> systemheader = std::make_shared<SystemHeader>();
    std::shared_ptr<ProgramStreamMap> psm = std::make_shared<ProgramStreamMap>();
    bool bVauFinished = false; // video access unit finished
    bool bVKeyFrame = false;

    while (bRunning)
    {
        if (!videoStream.empty())
        {
            Packet packet = videoStream.front();

            if (packet.bFirst)
            {
                if (pack != nullptr)
                {
                    bVauFinished = true;
                }
                else
                {
                    pack = std::make_shared<Pack>();
                    if (packet.bKeyFrame)
                    {
                        bVKeyFrame = true;
                        systemheader->addVideoStreamType(0xE0);
                        pack->addSystemHeader(systemheader);

                        psm->addElementaryStream(packet.stream_type, 0xE0);
                        pack->addPESPacket(psm);
                    }
                }
            }

            if (!bVauFinished)
            {
                pack->addPESPacket(packet.pes);
                videoStream.pop();
            }
        }

        if (bVKeyFrame && !audioStream.empty())
        {
            if (bVauFinished)
            {
                Packet packet = audioStream.front();
                systemheader->addAudioStreamType(0xC0);
                psm->addElementaryStream(packet.stream_type, 0xC0);
                while (!audioStream.empty())
                {
                    Packet packet = audioStream.front();
                    audioStream.pop();
                    pack->addPESPacket(packet.pes);
                }
            }
        }

        if (bVauFinished)
        {
            sendPack(pack);
            pack = nullptr;
            bVauFinished = false;
            bVKeyFrame = false;
        }
    }
}

bool PSMux::pushVideoPES(const Packet& packet)
{
    this->videoStream.push(packet);
    return true;
}

bool PSMux::pushAudioPES(const Packet& packet)
{
    this->audioStream.push(packet);
    return true;
}
