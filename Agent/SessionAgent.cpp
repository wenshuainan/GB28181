#include <unistd.h>
#include <sys/time.h>
#include <iostream>
#include <algorithm>
#include "UA.h"
#include "SessionAgent.h"
#include "DevPlay.h"
#include "DevPlayback.h"
#include "DevDownload.h"
#include "MANSCDPAgent.h"

Media::Media(const Attr& attr, RtpPayload::Type payloadType)
{
    m_rtpPayloadType = payloadType;
    if (m_rtpPayloadType == RtpPayload::PS)
    {
        m_psmux = std::make_shared<PSMux>(this);
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
    disconnect();
}

void Media::onProgramStream(const uint8_t *data, int32_t size)
{
#if 1
    static FILE *psfile = fopen("./stream.ps", "wb");
    if (psfile)
    {
        fwrite(data, 1, size, psfile);
        fflush(psfile);
    }
#endif
    if (m_rtpParticipant != nullptr)
    {
        m_rtpParticipant->transport(data, size);
    }
}

bool Media::connect()
{
    if (m_rtpParticipant)
    {
        return m_rtpParticipant->connect();
    }
    return false;
}

bool Media::disconnect()
{
    if (m_rtpParticipant)
    {
        return m_rtpParticipant->disconnect();
    }
    return false;
}

bool Media::input(PES::ES_TYPE type, const uint8_t *data, int32_t size)
{
    if (!m_rtpParticipant)
    {
        return false;
    }

    switch (type)
    {
    case PES::AVC:
    case PES::HEVC:
    {
        switch (m_rtpPayloadType)
        {
            case RtpPayload::PS:
            {
                if (m_vpes == nullptr)
                {
                    m_vpes = PES::create(type, m_psmux.get());
                    if (m_vpes == nullptr)
                    {
                        return false;
                    }
                }
                m_vpes->packetized(data, size);
                break;
            }
            case RtpPayload::H264:
            case RtpPayload::H265:
            {
                m_rtpParticipant->transport(data, size);
                break;
            }
            
            default:
                break;
        }
        break;
    }
    case PES::AAC:
    case PES::G711A:
    {
        switch (m_rtpPayloadType)
        {
            case RtpPayload::PS:
            {
                if (m_apes == nullptr)
                {
                    m_apes = PES::create(type, m_psmux.get());
                    if (m_apes == nullptr)
                    {
                        return false;
                    }
                }
                m_apes->packetized(data, size);
                break;
            }
            case RtpPayload::AAC:
            case RtpPayload::G711A:
            {
                m_rtpParticipant->transport(data, size);
                break;
            }
            
            default:
                break;
        }
        break;
    }
    
    default:
        break;
    }

    return true;
}

Session::Session(SessionAgent *agent, const Attr& attr)
    : m_attr(attr)
    , m_bStarted(false)
    , m_buffer(nullptr)
    , m_size(0)
    , m_agent(agent)
{}

Session::~Session()
{
    stop();
}

std::shared_ptr<Session> Session::create(SessionAgent *agent, const Attr& attr)
{
    if (attr.name == "Play")
    {
        return std::make_shared<SessionPlay>(agent, attr);
    }
    else if (attr.name == "Playback")
    {
        return std::make_shared<SessionPlayback>(agent, attr);
    }
    else if (attr.name == "Download")
    {
        return std::make_shared<SessionDownload>(agent, attr);
    }
    else
    {
        return nullptr;
    }
}

void Session::threadProc()
{
    while (m_bStarted)
    {
        int32_t len = fetchAudio(m_buffer, m_size);
        if (len > 0)
        {
            for (auto& m : m_media)
            {
                m->input(m_aType, m_buffer, len);
            }
        }
        len = fetchVideo(m_buffer, m_size);
        if (len > 0)
        {
            for (auto& m : m_media)
            {
                m->input(m_vType, m_buffer, len);
            }
        }

        if (isFileEnd())
        {
            m_agent->notifyFileEnd(getName(), m_attr.uriId);
        }

        usleep(10000);
    }
}

bool Session::getSdp(SipUserMessage& sdp)
{
    int num = m_media.size();
    if (num != 0)
    {
        sdp.setSdpMediaNum(num);
    }
    else
    {
        return false;
    }

    for (int i = 0; i < num; i++)
    {
        auto media = m_media[i];
        switch (media->m_rtpPayloadType)
        {
        case RtpPayload::PS:
        case RtpPayload::H264:
        case RtpPayload::H265:
            sdp.setSdpMediaType(i, "video");
            break;
        case RtpPayload::AAC:
        case RtpPayload::G711A:
            sdp.setSdpMediaType(i, "audio");
            break;
        
        default:
            break;
        }
        sdp.setSdpMediaPort(i, media->m_rtpParticipant->getLocalPort());
        sdp.setSdpMediaTransport(i, media->m_rtpParticipant->getTransportType());
        sdp.setSdpMediaPayloadType(i, media->m_rtpPayloadType);
        sdp.addSdpMediaAttr(i, "sendonly", "");
        sdp.setSdpMediaSSRC(i, media->m_rtpParticipant->getSSRC());
    }
    return true;
}

bool Session::convertPayloadType(PES::ES_TYPE src, RtpPayload::Type& dst) const
{
    switch (src)
    {
    case PES::AVC:
        dst = RtpPayload::H264;
        break;
    case PES::HEVC:
        dst = RtpPayload::H265;
        break;
    case PES::AAC:
        dst = RtpPayload::AAC;
        break;
    case PES::G711A:
        dst = RtpPayload::G711A;
        break;
        
    default:
        return false;
    }
    return true;
}

bool Session::start()
{
    if (m_bStarted)
    {
        return false;
    }

    m_size = 64 * 1024;
    m_buffer = new uint8_t[m_size];
    if (m_buffer == nullptr)
    {
        return false;
    }

    m_bStarted = true;
    m_thread = std::make_shared<std::thread>(&Session::threadProc, this);

    return true;
}

bool Session::stop()
{
    if (!m_bStarted)
    {
        return false;
    }

    m_bStarted = false;
    if (m_thread != nullptr)
    {
        m_thread->join();
        m_thread = nullptr;
    }
    delete []m_buffer;
    return true;
}

SessionPlay::SessionPlay(SessionAgent *agent, const Attr& attr)
    : Session(agent, attr)
{
    m_devPlay = std::make_shared<DevPlay>(m_agent->m_ch);
    if (m_devPlay != nullptr)
    {
        m_vType = m_devPlay->getVideoType();
        m_aType = m_devPlay->getAudioType();
    }
}

SessionPlay::~SessionPlay()
{}

bool SessionPlay::start()
{
    if (m_devPlay == nullptr || !m_devPlay->start())
    {
        return false;
    }
    
    return Session::start();
}

bool SessionPlay::stop()
{
    if (m_devPlay != nullptr)
    {
        Session::stop();
        return m_devPlay->stop();
    }
    return false;
}

void SessionPlay::setStreamNumber(int32_t num)
{
    if (m_devPlay != nullptr)
    {
        Play::StreamType type;
        switch (num)
        {
        case 0:
            type = Play::STREAM_MAIN;
            break;
        case 1:
            type = Play::STREAM_SUB1;
            break;
        
        default:
            return;
        }
        m_devPlay->setStreamType(type);
    }
}

std::shared_ptr<Media> SessionPlay::addMedia(const Media::Attr& attr)
{
    std::shared_ptr<Media> media;

    if (attr.type == "video")
    {
        /* 优先查找PS，如果不存在则查找设备当前视频编码类型 */
        auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), RtpPayload::PS);
        if (it != attr.payloadType.end())
        {
            media = std::make_shared<Media>(attr, RtpPayload::PS);
        }
        else
        {
            /* 获取设备当前视频编码类型 */
            RtpPayload::Type payloadType;
            if (!convertPayloadType(m_vType, payloadType))
            {
                return nullptr;
            }

            /* 查询当前设备视频编码是否匹配SDP请求 */
            auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
            if (it != attr.payloadType.end())
            {
                media = std::make_shared<Media>(attr, payloadType);
            }
            else // 不支持这个类型
            {
                return nullptr;
            }
        }
    }
    else if (attr.type == "audio")
    {
        /* 获取设备当前音频编码类型 */
        RtpPayload::Type payloadType;
        if (!convertPayloadType(m_aType, payloadType))
        {
            return nullptr;
        }

        /* 查询当前设备音频编码是否匹配SDP请求 */
        auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
        if (it != attr.payloadType.end())
        {
            media = std::make_shared<Media>(attr, payloadType);
        }
        else // 不支持这个类型
        {
            return nullptr;
        }
    }

    if (media != nullptr)
    {
        m_media.push_back(media);
        media->connect();
    }

    return media;
}

int32_t SessionPlay::fetchVideo(uint8_t *data, int32_t size)
{
    return m_devPlay->getVideo(data, size);
}

int32_t SessionPlay::fetchAudio(uint8_t *data, int32_t size)
{
    return m_devPlay->getAudio(data, size);
}

SessionPlayback::SessionPlayback(SessionAgent *agent, const Attr& attr)
    : Session(agent, attr)
{
    m_devPlayback = std::make_shared<DevPlayback>(m_agent->m_ch);
}

SessionPlayback::~SessionPlayback()
{}

std::shared_ptr<Media> SessionPlayback::addMedia(const Media::Attr& attr)
{
    std::shared_ptr<Media> media;

    if (attr.type == "video")
    {
        /* 优先查找PS，如果不存在则查找录像文件视频编码类型 */
        auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), RtpPayload::PS);
        if (it != attr.payloadType.end())
        {
            media = std::make_shared<Media>(attr, RtpPayload::PS);
        }
        else
        {
            /* 获取录像文件视频编码类型 */
            RtpPayload::Type payloadType;
            if (!convertPayloadType(m_vType, payloadType))
            {
                return nullptr;
            }

            auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
            if (it != attr.payloadType.end())
            {
                media = std::make_shared<Media>(attr, payloadType);
            }
            else // 不支持这个类型
            {
                return nullptr;
            }
        }
    }
    else if (attr.type == "audio")
    {
        /* 获取录像文件音频编码类型 */
        RtpPayload::Type payloadType;
        if (!convertPayloadType(m_aType, payloadType))
        {
            return nullptr;
        }

        auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
        if (it != attr.payloadType.end())
        {
            media = std::make_shared<Media>(attr, payloadType);
        }
        else // 不支持这个类型
        {
            return nullptr;
        }
    }

    if (media != nullptr)
    {
        m_media.push_back(media);
        media->connect();
    }

    return media;
}

int32_t SessionPlayback::fetchVideo(uint8_t *data, int32_t size)
{
    return m_devPlayback->readVideo(data, size);
}

int32_t SessionPlayback::fetchAudio(uint8_t *data, int32_t size)
{
    return m_devPlayback->readAudio(data, size);
}

bool SessionPlayback::play()
{
    return false;
}

bool SessionPlayback::pause()
{
    return false;
}

bool SessionPlayback::scalePlay()
{
    return false;
}

bool SessionPlayback::rangePlay()
{
    return false;
}

bool SessionPlayback::teardown()
{
    return false;
}

bool SessionPlayback::isFileEnd()
{
    /* 根据时间戳或者文件长度等判断是否结束 */
    /* ... */

    static timeval tv1 = {
        .tv_sec = 0,
        .tv_usec = 0
    };
    
    timeval tv2;
    gettimeofday(&tv2, NULL);
    if (tv1.tv_sec == 0)
    {
        tv1 = tv2;
    }
    else
    {
        if (tv2.tv_sec - tv1.tv_sec > 10)
        {
            tv1 = tv2;
            return true;
        }
    }
    return false;
}

SessionDownload::SessionDownload(SessionAgent *agent, const Attr& attr)
    : Session(agent, attr)
{
    m_devDownload = std::make_shared<DevDownload>(m_agent->m_ch);
}

SessionDownload::~SessionDownload()
{}

std::shared_ptr<Media> SessionDownload::addMedia(const Media::Attr& attr)
{
    std::shared_ptr<Media> media;

    if (attr.type == "video")
    {
        /* 优先查找PS，如果不存在则查找录像文件视频编码类型 */
        auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), RtpPayload::PS);
        if (it != attr.payloadType.end())
        {
            media = std::make_shared<Media>(attr, RtpPayload::PS);
        }
        else
        {
            /* 获取录像文件视频编码类型 */
            RtpPayload::Type payloadType;
            if (!convertPayloadType(m_vType, payloadType))
            {
                return nullptr;
            }

            auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
            if (it != attr.payloadType.end())
            {
                media = std::make_shared<Media>(attr, payloadType);
            }
            else // 不支持这个类型
            {
                return nullptr;
            }
        }
    }
    else if (attr.type == "audio")
    {
        /* 获取录像文件音频编码类型 */
        RtpPayload::Type payloadType;
        if (!convertPayloadType(m_aType, payloadType))
        {
            return nullptr;
        }

        auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
        if (it != attr.payloadType.end())
        {
            media = std::make_shared<Media>(attr, payloadType);
        }
        else // 不支持这个类型
        {
            return nullptr;
        }
    }

    if (media != nullptr)
    {
        m_media.push_back(media);
        media->connect();
    }

    return media;
}

int32_t SessionDownload::fetchVideo(uint8_t *data, int32_t size)
{
    return m_devDownload->readVideo(data, size);
}

int32_t SessionDownload::fetchAudio(uint8_t *data, int32_t size)
{
    return m_devDownload->readAudio(data, size);
}

bool SessionDownload::isFileEnd()
{
    /* 根据时间戳或者文件长度等判断是否结束 */
    /* ... */

    static timeval tv1 = {
        .tv_sec = 0,
        .tv_usec = 0
    };
    
    timeval tv2;
    gettimeofday(&tv2, NULL);
    if (tv1.tv_sec == 0)
    {
        tv1 = tv2;
    }
    else
    {
        if (tv2.tv_sec - tv1.tv_sec > 10)
        {
            tv1 = tv2;
            return true;
        }
    }
    return false;
}

SessionAgent::SessionAgent(UA *ua, int32_t ch)
    : Agent(ua), m_ch(ch)
{}

SessionAgent::~SessionAgent()
{}

std::shared_ptr<Session> SessionAgent::createSession(const SipUserMessage& req)
{
    Session::Attr attr = {
        .version = req.getSdpSessionVersion(),
        .owner = req.getSdpSessionOwner(),
        .name = req.getSdpSessionName(),
        .uriId = req.getSdpSessionUriId(),
        .uriParam = req.getSdpSessionUriParam(),
        .startTime = req.getSdpTimeStart(),
        .endTime = req.getSdpTimeEnd()
    };

    std::cout << "session:" << std::endl
                << "version=" << attr.version << " "
                << "owner=" << attr.owner.c_str() << " "
                << "name=" << attr.name.c_str() << " "
                << "uriId=" << attr.uriId.c_str() << " "
                << "uriParam=" << attr.uriParam << " "
                << "start=" << attr.startTime << " "
                << "end=" << attr.endTime
                << std::endl;

    std::shared_ptr<Session> session = Session::create(this, attr);
    if (session == nullptr)
    {
        return nullptr;
    }

    int i;
    int32_t num = req.getSdpMediaNum();
    for (i = 0; i < num; i++)
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

        if (mattr.type == "video")
        {
            const char* streamnumber = req.getSdpMediaAttr(i, "streamnumber");
            if (strlen(streamnumber) > 0)
            {
                session->setStreamNumber(atoi(streamnumber));
            }
        }

        session->addMedia(mattr);
    }
    return session;
}

bool SessionAgent::dispatchINVITE(const SessionIdentifier& id, const SipUserMessage& req)
{
    std::string name = req.getSdpSessionName();
    auto sip = m_ua->m_sip;

    /* 附录G 当设备不能满足更多的呼叫请求时,向发送的Invite请求方发送486错误响应消息 */
    if (isSessionExist(name))
    {
        SipUserMessage res486;
        sip->makeSessionResponse(req, res486, 486);
        sip->sendSessionResponse(id, res486);
        return false;
    }

    std::shared_ptr<Session> session = createSession(req);
    if (session != nullptr)
    {
        SipUserMessage res200;
        sip->makeSessionResponse(req, res200, 200);
        if (session->getSdp(res200))
        {
            sip->sendSessionResponse(id, res200);
            m_session[id] = session;
            return true;
        }
    }
    
    /* 附录G 当设备收到无法满足的SDP时,向发送的Invite请求方发送488错误响应消息 */
    SipUserMessage res488;
    sip->makeSessionResponse(req, res488, 488);
    sip->sendSessionResponse(id, res488);
    return false;
}

bool SessionAgent::dispatchACK(const SessionIdentifier& id)
{
    auto session = m_session.find(id);
    if (session != m_session.end())
    {
        return session->second->start();
    }
    return false;
}

bool SessionAgent::dispatchBYE(const SessionIdentifier& id)
{
    auto session = m_session.find(id);
    if (session != m_session.end())
    {
        session->second->stop();
        m_session.erase(session);
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
    else if (strCaseCmp(str, "TCP/RTP/AVP")
                || strCaseCmp(str, "RTP/AVP/TCP")) // for《GB28181自动化测试工具》
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
    return false;
}

bool SessionAgent::agent(const SessionIdentifier& id, const SipUserMessage& message)
{
    const char* method = message.getMethod();

    if (strCaseCmp(method, "INVITE"))
    {
        return dispatchINVITE(id, message);
    }
    else if (strCaseCmp(method, "ACK"))
    {
        return dispatchACK(id);
    }
    else if (strCaseCmp(method, "BYE"))
    {
        return dispatchBYE(id);
    }
    else
    {
        return false;
    }
}

const std::pair<const SessionIdentifier, std::shared_ptr<SessionPlayback>> SessionAgent::getMANSRTSPSession() const
{
    for (auto& session : m_session)
    {
        if (strCaseCmp(session.second->getName(), "Playback"))
        {
            return std::make_pair(session.first, std::dynamic_pointer_cast<SessionPlayback>(session.second));
        }
    }
    return std::make_pair(0, nullptr);
}

bool SessionAgent::isSessionExist(const std::string& name) const
{
    for (auto& session : m_session)
    {
        if (strCaseCmp(session.second->getName(), name))
        {
            return true;
        }
    }
    return false;
}

bool SessionAgent::notifyFileEnd(const std::string& name, const std::string& uriId) const
{
    for (auto& session : m_session)
    {
        if (strCaseCmp(session.second->getName(), name))
        {
            const std::shared_ptr<MANSCDPAgent>& cdpAgent = m_ua->m_cdpAgent;
            MediaStatusNotify status(cdpAgent.get(), session.first);
            return status.notify(uriId, "121");
        }
    }
    return false;
}