#include "MediaAgent.h"

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
    Play *play = m_session->m_agent->play;
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
}

bool Session::stop()
{
    for (auto m : media)
    {
        m->unpublish();
    }
}

MediaAgent::MediaAgent(UA *ua) : Agent(ua)
{
    play = nullptr;
}

MediaAgent::~MediaAgent()
{}

bool MediaAgent::agentReqINVITE(const SipMessageApp& req, SipMessageApp& res)
{
    return false;
}

bool MediaAgent::agentReqACK()
{
    return false;
}

bool MediaAgent::agentReqBYE()
{
    return false;
}

bool MediaAgent::match(const std::string& method, const std::string& contentType)
{
    return strCaseCmp(method, "INVITE") == 0
            || strCaseCmp(method, "ACK") == 0
            || strCaseCmp(method, "BYE") == 0
            || strCaseCmp(contentType, "application/sdp") == 0;
}

bool MediaAgent::match(const std::string& callID)
{
    return outCallID == callID;
}

bool MediaAgent::agent(const SipMessageApp& message)
{
    const char* method = message.getMethod();

    if (strCaseCmp(method, "INVITE") == 0)
    {}
    else if (strCaseCmp(method, "ACK") == 0)
    {}
    else if (strCaseCmp(method, "BYE") == 0)
    {}

    return false;
}
