#include "PSMux.h"

PSMux::PSMux()
{
    bRunning = false;
}

PSMux::~PSMux()
{}

void PSMux::sendPack(const std::shared_ptr<Pack>& pack)
{
    if (pack != nullptr && streamCallback != nullptr)
    {
        pack->toBitStream();
        const BitStream stream = pack->getBitStream();
        // printf(">>>>>> %s:%d [%0x %0x %0x %0x]\n", __FILE__, __LINE__, stream.getData()[0], stream.getData()[1], stream.getData()[2], stream.getData()[3]);
        streamCallback(stream.getData(), stream.getLength());

        const std::vector<std::shared_ptr<PESPacket>>& PES_packet = pack->getPESPacket();
        for (auto it : PES_packet)
        {
            const BitStream stream = it->getBitStream();
            streamCallback(stream.getData(), stream.getLength());
        }
    }
}

void PSMux::multiplexed()
{
    std::shared_ptr<Pack> pack = nullptr;
    std::shared_ptr<SystemHeader> systemheader = std::make_shared<SystemHeader>();
    std::shared_ptr<ProgramStreamMap> psm = std::make_shared<ProgramStreamMap>();
    bool bVAUFinished = false;

    while (bRunning)
    {
        if (!videoStream.empty())
        {
            Packet packet = videoStream.front();

            if (packet.bFirst)
            {
                if (pack != nullptr)
                {
                    bVAUFinished = true;
                }
                else
                {
                    pack = std::make_shared<Pack>();
                    if (packet.bKeyFrame)
                    {
                        systemheader->addVideoStreamType(0xE0);
                        // systemheader->addVideoStreamType(0xBC);
                        pack->addSystemHeader(systemheader);

                        psm->addElementaryStream(packet.stream_type);
                        pack->addPESPacket(psm);
                    }
                }
            }

            if (!bVAUFinished)
            {
                pack->addPESPacket(packet.pes);
                videoStream.pop();
            }
        }

        if (!audioStream.empty())
        {
            if (bVAUFinished)
            {
                Packet packet = audioStream.front();
                audioStream.pop();

                systemheader->addVideoStreamType(0xC0);
                psm->addElementaryStream(packet.stream_type);

                pack->addPESPacket(packet.pes);
            }
        }

        if (bVAUFinished)
        {
            // static int ii = 0;
            // printf("MMMMMM %d\n", ++ii);
            sendPack(pack);
            pack = nullptr;
            bVAUFinished = false;
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

void PSMux::setStreamCallback(std::function<void (const uint8_t *, int32_t)> callback)
{
    this->streamCallback = callback;
}

bool PSMux::start()
{
    if (bRunning)
    {
        return false;
    }

    bRunning = true;
    thread = new std::thread(&PSMux::multiplexed, this);
    return true;
}

bool PSMux::stop()
{
    if (!bRunning)
    {
        return false;
    }

    bRunning = false;
    if (thread != nullptr)
    {
        thread->join();
        delete thread;
        thread = nullptr;
    }

    return true;
}