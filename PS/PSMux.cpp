#include "PSMux.h"

PSMux::PSMux(PSCallback *callback)
{
    m_callback = callback;

    m_bRunning = true;
    m_thread = new std::thread(&PSMux::multiplexed, this);
}

PSMux::~PSMux()
{
    if (!m_bRunning)
    {
        return;
    }

    m_bRunning = false;
    if (m_thread != nullptr)
    {
        m_thread->join();
        delete m_thread;
        m_thread = nullptr;
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
    std::shared_ptr<Pack> pack;
    std::shared_ptr<SystemHeader> systemheader = std::make_shared<SystemHeader>();
    std::shared_ptr<ProgramStreamMap> psm = std::make_shared<ProgramStreamMap>();
    bool bVauFinished = false; // video access unit finished

    while (m_bRunning)
    {
        {
            std::lock_guard<std::mutex> guard(m_vMutex);
            if (!m_videoStream.empty())
            {
                Packet packet = m_videoStream.front();
                systemheader->addVideoStreamType(0xE0);
                psm->addElementaryStream(packet.stream_type, 0xE0);

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
                            pack->addSystemHeader(systemheader);
                            pack->addPESPacket(psm);
                        }
                    }
                }

                if (!bVauFinished)
                {
                    pack->addPESPacket(packet.pes);
                    m_videoStream.pop();
                }
            }
        }

        {
            std::lock_guard<std::mutex> guard(m_aMutex);
            if (!m_audioStream.empty())
            {
                Packet packet = m_audioStream.front();
                systemheader->addAudioStreamType(0xC0);
                psm->addElementaryStream(packet.stream_type, 0xC0);
            }
        }

        if (bVauFinished)
        {
            {
                std::lock_guard<std::mutex> guard(m_aMutex);
                while (!m_audioStream.empty())
                {
                    Packet packet = m_audioStream.front();
                    m_audioStream.pop();
                    pack->addPESPacket(packet.pes);
                }
            }

            sendPack(pack);
            pack = nullptr;
            bVauFinished = false;
        }
    }
}

bool PSMux::pushVideoPES(const Packet& packet)
{
    std::lock_guard<std::mutex> guard(m_vMutex);
    this->m_videoStream.push(packet);
    return true;
}

bool PSMux::pushAudioPES(const Packet& packet)
{
    std::lock_guard<std::mutex> guard(m_aMutex);
    this->m_audioStream.push(packet);
    return true;
}
