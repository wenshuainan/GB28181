#include <iostream>
#include <algorithm>
#include "SessionAgent.h"
#include "DevPlay.h"
#include "UA.h"

Media::Media(Session* session, const Attr& attr, RtpPayload::Type payloadType)
{
    m_bPublish = false;
    m_session = session;

    m_rtpPayloadType = payloadType;
    if (m_rtpPayloadType == RtpPayload::PS)
    {
        m_psMux = std::make_shared<PSMux>(this);
        //根据设备当前的视音频编码类型创建
        m_videoPES = PES::create(PES::AVC, m_psMux.get());
        m_audioPES = PES::create(PES::G711A, m_psMux.get());
    }

    RtpParticipant::Participant participant;
    participant.destination.ipv4 = attr.conInfo.ipv4;
    participant.destination.port = attr.port;
    participant.netType = attr.netType;
    participant.payloadType = m_rtpPayloadType;
    participant.SSRC = attr.SSRC;
    m_rtpParticipant = std::make_shared<RtpParticipant>(participant);
}

Media::~Media()
{
    unpublish();
}

void Media::proc()
{
    static FILE *debugf = fopen("./stream.raw", "wb");

    const std::shared_ptr<Play> &play = m_session->m_agent->m_devPlay;
    if (play == nullptr)
    {
        return;
    }

    while (m_bPublish)
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
                    if (debugf != NULL)
                    {
                        fwrite(coded.data, 1, coded.size, debugf);
                        fflush(debugf);
                    }

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
                m_rtpParticipant->transport(coded.data, coded.size);
                play->putCoded(coded);
            }
            break;
        }
        case RtpPayload::G711A:
        {
            Play::Coded coded;
            if (play->getAudio(coded))
            {
                m_rtpParticipant->transport(coded.data, coded.size);
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
    static FILE *debugf = fopen("./stream.ps", "wb");
    if (debugf != NULL)
    {
        fwrite(data, 1, size, debugf);
        fflush(debugf);
    }

    if (m_rtpParticipant != nullptr)
    {
        m_rtpParticipant->transport(data, size);
    }
}

const std::shared_ptr<RtpParticipant>& Media::getRtpParticipant() const
{
    return m_rtpParticipant;
}

RtpPayload::Type Media::getRtpPaylodaType() const
{
    return m_rtpPayloadType;
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
    if (m_bPublish)
    {
        return false;
    }

    m_bPublish = true;
    m_thread = std::make_shared<std::thread>(&Media::proc, this);
    return true;
}

bool Media::unpublish()
{
    if (!m_bPublish)
    {
        return false;
    }

    m_bPublish = false;
    if (m_thread != nullptr)
    {
        if (m_thread->joinable())
        {
            m_thread->join();
        }
    }

    m_rtpParticipant->disconnect();

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
{
    m_media.clear();
}

std::shared_ptr<Media> Session::addMedia(const Media::Attr& attr)
{
    std::shared_ptr<Media> media;

    if (attr.type == "video")
    {
        /* 优先查找PS，如果不存在则查找设备当前视频编码类型 */
        auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), RtpPayload::PS);
        if (it != attr.payloadType.end())
        {
            media = std::make_shared<Media>(this, attr, RtpPayload::PS);
        }
        else
        {
            RtpPayload::Type payloadType = RtpPayload::H264;
            // 获取设备当前视频编码类型
            // payloadType = xxx

            auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
            if (it != attr.payloadType.end())
            {
                media = std::make_shared<Media>(this, attr, payloadType);
            }
            else // 不支持这个类型
            {
                media = nullptr;
            }
        }
    }
    else if (attr.type == "audio")
    {
        RtpPayload::Type payloadType = RtpPayload::G711A;
        // 获取设备当前音频编码类型...
        // payloadType = xxx

        auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
        if (it != attr.payloadType.end())
        {
            media = std::make_shared<Media>(this, attr, payloadType);
        }
        else // 不支持这个类型
        {
            media = nullptr;
        }
    }

    if (media != nullptr)
    {
        media->connect();
        m_media.push_back(media);
    }

    return media;
}

bool Session::start()
{
    for (auto m : m_media)
    {
        m->publish();
    }

    return true;
}

bool Session::stop()
{
    for (auto m : m_media)
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
    int32_t reqMediaNum = 0;
    int32_t resMediaNum = 0;

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

        reqMediaNum = req.getSdpMediaNum();
        for (int32_t i = 0; i < reqMediaNum; i++)
        {
            Media::Attr mattr;

            mattr.type = req.getSdpMediaType(i);
            mattr.port = req.getSdpMediaPort(i);
            mattr.netType = parseNetType(req.getSdpMediaTransport(i));
            uint16_t payloadType[10] = {0};
            int32_t num = req.getSdpMediaPayloadType(i, payloadType);
            for (int32_t j = 0; j < num; j++)
            {
                mattr.payloadType.push_back((RtpPayload::Type)payloadType[j]);
            }
            mattr.conInfo.ipv4 = req.getSdpMediaIpv4(i);
            mattr.SSRC = req.getSdpMediaSSRC(i);
            std::cout << "media:" << std::endl
                      << "type=" << mattr.type.c_str() << " "
                      << "port=" << mattr.port  << " "
                      << "net=" << mattr.netType  << " "
                      << "payload=";
                      for (int32_t j = 0; (uint32_t)j < mattr.payloadType.size(); j++)
                      {
                        std::cout << mattr.payloadType[j]  << " ";
                      }
                      std::cout << "ipv4=" << mattr.conInfo.ipv4.c_str()  << " "
                      << "ssrc=" << mattr.SSRC  << " "
                      << std::endl;

            std::shared_ptr<Media> media = m_sessionPlay->addMedia(mattr);
            if (media != nullptr)
            {
                resMediaNum++;
                res.setSdpMediaNum(resMediaNum);
                res.setSdpMediaType(resMediaNum - 1, mattr.type.c_str());
                res.setSdpMediaPort(resMediaNum -1, media->getRtpParticipant()->getLocalPort());
                res.setSdpMediaTransport(resMediaNum -1, req.getSdpMediaTransport(i));
                res.setSdpMediaPayloadType(resMediaNum -1, media->getRtpPaylodaType());
                res.setSdpMediaSSRC(resMediaNum -1, mattr.SSRC);
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
    else if (strCaseCmp(str, "RTP/AVP/TCP"))
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
    return strCaseCmp(method, "INVITE")
            || strCaseCmp(method, "ACK")
            || strCaseCmp(method, "BYE")
            || strCaseCmp(contentType, "application/sdp");
}

bool SessionAgent::agent(const SipUserMessage& message)
{
    const char* method = message.getMethod();

    if (strCaseCmp(method, "INVITE"))
    {
        return dispatchINVITE(message);
    }
    else if (strCaseCmp(method, "ACK"))
    {
        return dispatchACK();
    }
    else if (strCaseCmp(method, "BYE"))
    {
        return dispatchBYE();
    }
    else
    {
        return false;
    }
}
