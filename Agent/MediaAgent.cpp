#include "MediaAgent.h"
#include "DevPlay.h"

Media::Media(Session* session, const Attr& attr)
{
    bRunning = false;
    m_session = session;
    Connection *conInfo = attr.conInfo;

    if (attr.conInfo == NULL)
    {
        conInfo = session->m_conInfo != nullptr ? session->m_conInfo.get() : NULL;
    }
    if (conInfo == NULL)
    {
        return;
    }

    rtpPayloadType = attr.payloadType;

    if (rtpPayloadType == RtpPayload::PS)
    {
        psMux = std::make_shared<PSMux>(this);
        
        //根据设备目前的视音频编码类型创建
        videoPES = PES::create(PES::AVC, psMux.get());
        audioPES = PES::create(PES::G711A, psMux.get());
    }

    RtpParticipant::Participant participant;
    participant.destination.ipv4 = conInfo->ipv4;
    participant.destination.port = attr.port;
    participant.netType = attr.netType;
    participant.payloadType = attr.payloadType;
    participant.SSRC = attr.SSRC != NULL ? *attr.SSRC : 0;
    rtpParticipant = std::make_shared<RtpParticipant>(participant);
}

Media::~Media()
{}

void Media::proc()
{
    const std::shared_ptr<Play> &play = m_session->m_agent->play;
    if (play == nullptr)
    {
        return;
    }

    while (bRunning)
    {
        switch (rtpPayloadType)
        {
        case RtpPayload::PS:
        {
            if (videoPES)
            {
                Play::Coded coded;
                if (play->getVideo(coded))
                {
                    videoPES->packetized(coded.data, coded.size);
                    play->putCoded(coded);
                }
            }
            if (audioPES)
            {
                Play::Coded coded;
                if (play->getAudio(coded))
                {
                    audioPES->packetized(coded.data, coded.size);
                    play->putCoded(coded);
                }
            }
            break;
        }
        case RtpPayload::H264:
        {
            Play::Coded coded;
            if (play->getVideo(coded))
            {
                rtpParticipant->format(coded.data, coded.size);
                play->putCoded(coded);
            }
            break;
        }
        case RtpPayload::G711A:
        {
            Play::Coded coded;
            if (play->getAudio(coded))
            {
                rtpParticipant->format(coded.data, coded.size);
                play->putCoded(coded);
            }
            break;
        }
        
        default:
            break;
        }
    }
}

void Media::onProgramStream(const uint8_t *data, int32_t size)
{
    if (rtpParticipant != nullptr)
    {
        rtpParticipant->format(data, size);
    }
}

bool Media::connect()
{
    return rtpParticipant->connect();
}

bool Media::disconnect()
{
    return rtpParticipant->disconnect();
}

bool Media::publish()
{
    bRunning = true;
    thread = new std::thread(&Media::proc, this);
    return true;
}

bool Media::unpublish()
{
    bRunning = false;

    if (thread != nullptr)
    {
        if (thread->joinable())
        {
            thread->join();
        }
        delete thread;
    }

    return true;
}

Session::Session(MediaAgent *agent, const Attr& attr)
{
    m_agent = agent;

    if (attr.conInfo != NULL)
    {
        m_conInfo = std::make_shared<Connection>(*attr.conInfo);
    }
}

Session::~Session()
{}

bool Session::addMedia(const Media::Attr& attr)
{
    std::shared_ptr<Media> m = std::make_shared<Media>(this, attr);
    m->connect();
    media.push_back(m);
    return true;
}

bool Session::start()
{
    for (auto m : media)
    {
        m->publish();
    }

    return true;
}

bool Session::stop()
{
    for (auto m : media)
    {
        m->unpublish();
    }

    return true;
}

MediaAgent::MediaAgent(UA *ua) : Agent(ua)
{
    play = std::make_shared<DevPlay>();
}

MediaAgent::~MediaAgent()
{}

bool MediaAgent::agentReqINVITE(const SipMessageApp& req)
{
    std::string name = req.getSdpSessionName();

    if (strCaseCmp(name, "Play"))
    {
        Session::Attr attr;
        Connection sessionCon;

        sessionCon.ipv4 = req.getSdpSessionIpv4();
        if (!sessionCon.ipv4.empty())
        {
            attr.conInfo = &sessionCon;
        }

        m_sessionPlay = std::make_shared<Session>(this, attr);

        int32_t n = req.getSdpMediaNum();
        for (int32_t i = 0; i < n; i++)
        {
            Media::Attr mattr;
            Connection mCon;
            uint32_t SSRC = 0;

            mattr.port = req.getSdpMediaPort(i);
            mattr.netType = parseNetType(req.getSdpMediaTransport(i));
            mattr.payloadType = (RtpPayload::Type)req.getSdpMediaPayloadType(i);
            mCon.ipv4 = req.getSdpMediaIpv4(i);
            if (!mCon.ipv4.empty())
            {
                mattr.conInfo = &mCon;
            }
            SSRC = req.getSdpMediaSSRC(i);
            mattr.SSRC = &SSRC;

            m_sessionPlay->addMedia(mattr);
        }
    }
    else if (strCaseCmp(name, "Playback"))
    {
        /* add code */
    }
    else
    {
        return false;
    }
}

bool MediaAgent::agentReqACK(const SipMessageApp& req)
{
    if (m_sessionPlay != nullptr)
    {
        m_sessionPlay->start();
        return true;
    }

    return false;
}

bool MediaAgent::agentReqBYE(const SipMessageApp& req)
{
    if (m_sessionPlay != nullptr)
    {
        m_sessionPlay->stop();
        m_sessionPlay = nullptr;
        return true;
    }

    return false;
}

RtpNet::Type MediaAgent::parseNetType(const std::string& str) const
{
    if (strCaseCmp(str, "RTP/AVP"))
    {
        return RtpNet::UDP;
    }
    else if (strCaseCmp(str, "TCP/RTP/AVP"))
    {
        return RtpNet::TCP_ACTIVE;
    }
    else
    {
        return RtpNet::UDP;
    }
}

bool MediaAgent::match(const std::string& method, const std::string& contentType)
{
    return strCaseCmp(method, "INVITE") == 0
            || strCaseCmp(method, "ACK") == 0
            || strCaseCmp(method, "BYE") == 0
            || strCaseCmp(contentType, "application/sdp") == 0;
}

bool MediaAgent::agent(const SipMessageApp& message)
{
    const char* method = message.getMethod();

    if (strCaseCmp(method, "INVITE") == 0)
    {
        return agentReqINVITE(message);
    }
    else if (strCaseCmp(method, "ACK") == 0)
    {
        return agentReqACK(message);
    }
    else if (strCaseCmp(method, "BYE") == 0)
    {
        return agentReqBYE(message);
    }

    return false;
}
