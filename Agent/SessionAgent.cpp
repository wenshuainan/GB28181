#include "SessionAgent.h"
#include "DevPlay.h"
#include "UA.h"

Media::Media(Session* session, const Attr& attr)
{
    m_bRunning = false;
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

    m_rtpPayloadType = attr.payloadType;

    if (m_rtpPayloadType == RtpPayload::PS)
    {
        m_psMux = std::make_shared<PSMux>(this);
        
        //根据设备目前的视音频编码类型创建
        m_videoPES = PES::create(PES::AVC, m_psMux.get());
        m_audioPES = PES::create(PES::G711A, m_psMux.get());
    }

    RtpParticipant::Participant participant;
    participant.destination.ipv4 = conInfo->ipv4;
    participant.destination.port = attr.port;
    participant.netType = attr.netType;
    participant.payloadType = attr.payloadType;
    participant.SSRC = attr.SSRC != NULL ? *attr.SSRC : 0;
    m_rtpParticipant = std::make_shared<RtpParticipant>(participant);
}

Media::~Media()
{}

void Media::proc()
{
    const std::shared_ptr<Play> &play = m_session->m_agent->m_devPlay;
    if (play == nullptr)
    {
        return;
    }

    while (m_bRunning)
    {
        switch (m_rtpPayloadType)
        {
        case RtpPayload::PS:
        {
            if (m_videoPES)
            {
                Play::Coded coded;
                if (play->getVideo(coded))
                {
                    m_videoPES->packetized(coded.data, coded.size);
                    play->putCoded(coded);
                }
            }
            if (m_audioPES)
            {
                Play::Coded coded;
                if (play->getAudio(coded))
                {
                    m_audioPES->packetized(coded.data, coded.size);
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
                m_rtpParticipant->format(coded.data, coded.size);
                play->putCoded(coded);
            }
            break;
        }
        case RtpPayload::G711A:
        {
            Play::Coded coded;
            if (play->getAudio(coded))
            {
                m_rtpParticipant->format(coded.data, coded.size);
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
    if (m_rtpParticipant != nullptr)
    {
        m_rtpParticipant->format(data, size);
    }
}

bool Media::connect()
{
    return m_rtpParticipant->connect();
}

bool Media::disconnect()
{
    return m_rtpParticipant->disconnect();
}

bool Media::publish()
{
    m_bRunning = true;
    m_thread = new std::thread(&Media::proc, this);
    return true;
}

bool Media::unpublish()
{
    m_bRunning = false;

    if (m_thread != nullptr)
    {
        if (m_thread->joinable())
        {
            m_thread->join();
        }
        delete m_thread;
        m_thread = nullptr;
    }

    return true;
}

Session::Session(SessionAgent *agent, const Attr& attr)
{
    m_agent = agent;

    if (attr.conInfo != NULL)
    {
        m_conInfo = std::make_shared<Connection>(*attr.conInfo);
    }
}

Session::~Session()
{}

std::shared_ptr<Media> Session::addMedia(const Media::Attr& attr)
{
    std::shared_ptr<Media> m = std::make_shared<Media>(this, attr);
    m->connect();
    media.push_back(m);
    return m;
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

SessionAgent::SessionAgent(UA *ua) : Agent(ua)
{
    m_devPlay = std::make_shared<DevPlay>();
}

SessionAgent::~SessionAgent()
{}

bool SessionAgent::dispatchINVITE(const SipUserMessage& req)
{
    SipUserMessage res;
    std::shared_ptr<SipUserAgent> sip = m_ua->getSip();
    sip->makeSessionResponse(req, res, 200);
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

            std::shared_ptr<Media> m = m_sessionPlay->addMedia(mattr);
            if (m != nullptr)
            {
                res.setSdpMediaNum(i + 1);
                res.setSdpMediaPort(i, m->getRtpParticipant()->getLocalPort());
            }
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

    return sip->sendSessionResponse(res);
}

bool SessionAgent::dispatchACK()
{
    if (m_sessionPlay != nullptr)
    {
        m_sessionPlay->start();
        return true;
    }

    return false;
}

bool SessionAgent::dispatchBYE()
{
    if (m_sessionPlay != nullptr)
    {
        m_sessionPlay->stop();
        m_sessionPlay = nullptr;
        return true;
    }

    return false;
}

RtpNet::Type SessionAgent::parseNetType(const std::string& str) const
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

bool SessionAgent::match(const std::string& method, const std::string& contentType)
{
    return strCaseCmp(method, "INVITE") == 0
            || strCaseCmp(method, "ACK") == 0
            || strCaseCmp(method, "BYE") == 0
            || strCaseCmp(contentType, "application/sdp") == 0;
}

bool SessionAgent::agent(const SipUserMessage& message)
{
    const char* method = message.getMethod();

    if (strCaseCmp(method, "INVITE") == 0)
    {
        return dispatchINVITE(message);
    }
    else if (strCaseCmp(method, "ACK") == 0)
    {
        return dispatchACK();
    }
    else if (strCaseCmp(method, "BYE") == 0)
    {
        return dispatchBYE();
    }
    else
    {
        return false;
    }
}
