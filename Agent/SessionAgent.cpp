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
    if (m_rtpParticipant != nullptr)
    {
        m_rtpParticipant->transport(data, size);
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

bool Media::input(PES::ES_TYPE type, const uint8_t *data, int32_t size)
{
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

const std::shared_ptr<RtpParticipant>& Media::getRtpParticipant() const
{
    return m_rtpParticipant;
}

RtpPayload::Type Media::getRtpPaylodaType() const
{
    return m_rtpPayloadType;
}

Session::Session(SessionAgent *agent)
    : m_agent(agent)
    , m_bStarted(false)
    , m_buffer(nullptr)
    , m_size(0)
{}

Session::~Session()
{
    stop();
}

std::shared_ptr<Session> Session::create(SessionAgent *agent, const Attr& attr)
{
    if (attr.name == "Play")
    {
        return std::make_shared<SessionPlay>(agent);
    }
    else if (attr.name == "Playback")
    {
        return std::make_shared<SessionPlayback>(agent);
    }
    else if (attr.name == "Download")
    {
        return std::make_shared<SessionDownload>(agent);
    }
    else
    {
        return nullptr;
    }
}

void Session::proc()
{
    while (m_bStarted)
    {
        PES::ES_TYPE type;
        int32_t len = read(m_agent->m_ch, type, m_buffer, m_size);
        if (len > 0)
        {
            for (auto& m : m_media)
            {
                m->input(type, m_buffer, len);
            }
        }

        if (isFileEnd())
        {
            m_agent->notifyFileEnd(getName());
        }

        usleep(10000);
    }
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
    m_thread = std::make_shared<std::thread>(&Session::proc, this);

    return true;
}

bool Session::stop()
{
    if (!m_bStarted)
    {
        return false;
    }

    m_bStarted = false;
    m_thread->join();
    m_thread = nullptr;
    delete []m_buffer;
    return true;
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
        switch (media->getRtpPaylodaType())
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
        sdp.setSdpMediaPort(i, media->getRtpParticipant()->getLocalPort());
        sdp.setSdpMediaTransport(i, media->getRtpParticipant()->getTransportType());
        sdp.setSdpMediaPayloadType(i, media->getRtpPaylodaType());
        sdp.addSdpMediaAttr(i, "sendonly", "");
        sdp.setSdpMediaSSRC(i, media->getRtpParticipant()->getSSRC());
    }
    return true;
}

SessionPlay::SessionPlay(SessionAgent *agent)
    : Session(agent)
{
    m_devPlay = std::make_shared<DevPlay>();
}

SessionPlay::~SessionPlay()
{}

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
            RtpPayload::Type payloadType = RtpPayload::H264;
            // 获取设备当前视频编码类型
            // payloadType = xxx

            auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
            if (it != attr.payloadType.end())
            {
                media = std::make_shared<Media>(attr, payloadType);
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
            media = std::make_shared<Media>(attr, payloadType);
        }
        else // 不支持这个类型
        {
            media = nullptr;
        }
    }

    if (media != nullptr)
    {
        m_media.push_back(media);
        media->connect();
    }

    return media;
}

int32_t SessionPlay::read(int32_t ch, PES::ES_TYPE &type, uint8_t *data, int32_t size)
{
    type = PES::AVC;
    return m_devPlay->getVideo(ch, data, size);
}

SessionPlayback::SessionPlayback(SessionAgent *agent)
    : Session(agent)
{
    m_devPlayback = std::make_shared<DevPlayback>();
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
            RtpPayload::Type payloadType = RtpPayload::H264;
            // 获取录像文件视频编码类型
            // payloadType = xxx

            auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
            if (it != attr.payloadType.end())
            {
                media = std::make_shared<Media>(attr, payloadType);
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
        // 获取录像文件音频编码类型...
        // payloadType = xxx

        auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
        if (it != attr.payloadType.end())
        {
            media = std::make_shared<Media>(attr, payloadType);
        }
        else // 不支持这个类型
        {
            media = nullptr;
        }
    }

    if (media != nullptr)
    {
        m_media.push_back(media);
        media->connect();
    }

    return media;
}

int32_t SessionPlayback::read(int32_t ch, PES::ES_TYPE &type, uint8_t *data, int32_t size)
{
    type = PES::AVC;
    return m_devPlayback->read(ch, data, size);
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

SessionDownload::SessionDownload(SessionAgent *agent)
    : Session(agent)
{
    m_devDownload = std::make_shared<DevDownload>();
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
            RtpPayload::Type payloadType = RtpPayload::H264;
            // 获取录像文件视频编码类型
            // payloadType = xxx

            auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
            if (it != attr.payloadType.end())
            {
                media = std::make_shared<Media>(attr, payloadType);
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
        // 获取录像文件音频编码类型...
        // payloadType = xxx

        auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
        if (it != attr.payloadType.end())
        {
            media = std::make_shared<Media>(attr, payloadType);
        }
        else // 不支持这个类型
        {
            media = nullptr;
        }
    }

    if (media != nullptr)
    {
        m_media.push_back(media);
        media->connect();
    }

    return media;
}

int32_t SessionDownload::read(int32_t ch, PES::ES_TYPE &type, uint8_t *data, int32_t size)
{
    type = PES::AVC;
    return m_devDownload->read(ch, data, size);
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
        .uri = req.getSdpSessionUri(),
        .startTime = req.getSdpTimeStart(),
        .endTime = req.getSdpTimeEnd()
    };

    std::cout << "session:" << std::endl
                << "version=" << attr.version << " "
                << "owner=" << attr.owner.c_str() << " "
                << "name=" << attr.name.c_str() << " "
                << "uri=" << attr.uri.c_str() << " "
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

        session->addMedia(mattr);
    }
    return session;
}

bool SessionAgent::dispatchINVITE(const SessionIdentifier& id, const SipUserMessage& req)
{
    std::string name = req.getSdpSessionName();
    auto sip = m_ua->getSip();

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
        session->second->start();
        return true;
    }
    else
    {
        return false;
    }
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
    else
    {
        return false;
    }
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

bool SessionAgent::notifyFileEnd(const std::string& name) const
{
    for (auto& session : m_session)
    {
        if (strCaseCmp(session.second->getName(), name))
        {
            const std::shared_ptr<MANSCDPAgent>& cdpAgent = m_ua->m_cdpAgent;
            MediaStatusNotify notify(cdpAgent.get(), session.first);
            return notify.handle("64010000041310000345", "121");
        }
    }
    return false;
}