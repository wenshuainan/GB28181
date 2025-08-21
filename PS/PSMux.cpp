#include <unistd.h>
#include "PSMux.h"

PSMux::PSMux(PSCallback *callback)
{
    printf("++++++ PSMux\n");
    m_callback = callback;
    m_bFinishFlag = false;

    m_bRunning = true;
    m_thread = std::move(std::unique_ptr<std::thread>(new std::thread(&PSMux::multiplexed, this)));
}

PSMux::~PSMux()
{
    printf("----- PSMux\n");
    if (!m_bRunning)
    {
        return;
    }

    printf("wait thread join\n");
    m_bRunning = false;
    if (m_thread != nullptr)
    {
        m_thread->join();
        printf("thread joined\n");
    }
}

void PSMux::sendPack(const std::unique_ptr<Pack>& pack)
{
    if (pack != nullptr && m_callback != nullptr)
    {
        pack->toBitStream();
        const BitStream stream = pack->getBitStream();
        m_callback->onProgramStream(stream.getData(), stream.getLength());

        auto PES_packet = pack->getPESPacket();
        for (auto& it : PES_packet)
        {
            const BitStream pes = it->getBitStream();
            m_callback->onProgramStream(pes.getData(), pes.getLength());
        }
    }
}

void PSMux::multiplexed()
{
    Pack *pack = nullptr;
    std::shared_ptr<SystemHeader> systemheader = std::make_shared<SystemHeader>();
    std::shared_ptr<ProgramStreamMap> psm = std::make_shared<ProgramStreamMap>();
    bool bVauFinished = true; // video access unit finished

    if (systemheader == nullptr || psm == nullptr)
    {
        printf("make_shared systemheader and psm failed\n");
        return;
    }

    while (m_bRunning || (m_bFinishFlag && !m_videoStream.empty()))
    {
        {
            std::lock_guard<std::mutex> guard(m_vMutex);
            if (!m_videoStream.empty())
            {
                Packet packet = m_videoStream.front();
                m_videoStream.pop();
                systemheader->addVideoStreamType(0xE0);
                psm->addElementaryStream(packet.stream_type, 0xE0);

                if (packet.bFirst)
                {
                    if ((pack = new Pack()) == nullptr)
                    {
                        printf("pack new failed\n");
                        break;
                    }
                    if (packet.bKeyFrame)
                    {
                        // printf("add systemheader and psm\n");
                        pack->addSystemHeader(systemheader);
                        pack->addPESPacket(psm);
                    }
                }
                pack->addPESPacket(packet.pes);
                bVauFinished = packet.bFinished;
            }
        }

        if (pack && bVauFinished)
        {
            /* PESA放到pack最后 */
            {
                std::lock_guard<std::mutex> guard(m_aMutex);
                while (!m_audioStream.empty())
                {
                    Packet packet = m_audioStream.front();
                    m_audioStream.pop();
                    systemheader->addAudioStreamType(0xC0);
                    psm->addElementaryStream(packet.stream_type, 0xC0);
                    pack->addPESPacket(packet.pes);
                }
            }

            // printf("send pack\n");
            sendPack(std::move(std::unique_ptr<Pack>(pack)));
            pack = nullptr;
        }

        usleep(10000);
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

void PSMux::finished()
{
    printf("PS finished\n");
    m_bRunning = false;
    m_bFinishFlag = true;
    if (m_thread != nullptr)
    {
        m_thread->join();
    }
    printf("PS video and audio stream completed\n");
}