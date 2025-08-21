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
#include "MANSCDP/A.2.5Notify.h"
#include "MANSRTSPAgent.h"

Media::Media(const Attr& attr, RtpPayload::Type payloadType)
{
    printf("++++++ Media\n");
    m_rtpPayloadType = payloadType;
    if (m_rtpPayloadType == RtpPayload::PS)
    {
        m_psmux = std::move(std::unique_ptr<PSMux>(new PSMux(this)));
    }

    RtpParticipant::Participant participant;
    participant.destination.ipv4 = attr.conInfo.ipv4;
    participant.destination.port = attr.port;
    participant.netType = attr.netType;
    participant.payloadType = m_rtpPayloadType;
    participant.SSRC = attr.SSRC;
    m_rtpParticipant = std::move(std::unique_ptr<RtpParticipant>(new RtpParticipant(participant)));
}

Media::~Media()
{
    printf("------ Media\n");
    disconnect();
}

void Media::onProgramStream(const uint8_t *data, int32_t size)
{
#if 0
    static FILE *psfile = fopen("/nfsroot/stream.ps", "wb");
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
        printf("session media connect\n");
        return m_rtpParticipant->connect();
    }
    return false;
}

bool Media::disconnect()
{
    if (m_rtpParticipant)
    {
        printf("session media disconnect\n");
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
                        printf("failed to create video PES\n");
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
                printf("not implement RTP payload type %d\n", m_rtpPayloadType);
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
                        printf("failed to create audio PES\n");
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
                printf("not implement RTP payload type %d\n", m_rtpPayloadType);
                break;
        }
        break;
    }
    
    default:
        printf("not implement PES type %d\n", type);
        break;
    }

    return true;
}

void Media::finished()
{
    if (m_vpes)
    {
        m_vpes->finished();
    }
    if (m_apes)
    {
        m_apes->finished();
    }
    if (m_psmux)
    {
        m_psmux->finished();
    }
}

Session::Session(SessionAgent *agent, const Attr& attr, const std::string& subject)
    : m_attr(attr)
    , m_bStarted(false)
    , m_buffer(nullptr)
    , m_size(0)
    , m_subject(subject)
    , m_agent(agent)
{
    printf("++++++ Session\n");
}

Session::~Session()
{
    printf("------ Session\n");
    stop();
}

std::unique_ptr<Session> Session::create(SessionAgent *agent, const Attr& attr, const std::string& subject)
{
    printf("create session %s\n", attr.name.c_str());
    Session *session = nullptr;
    if (attr.name == "Play")
    {
        session = new SessionPlay(agent, attr, subject);
    }
    else if (attr.name == "Playback")
    {
        session = new SessionPlayback(agent, attr, subject);
    }
    else if (attr.name == "Download")
    {
        session = new SessionDownload(agent, attr, subject);
    }
    else
    {
        printf("unkownd session name\n");
        return nullptr;
    }
    return std::unique_ptr<Session>(session);
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
            printf("session %d:%s end\n", m_agent->getCh(), getName());
            for (auto& m : m_media)
            {
                m->finished();
            }
            m_agent->notifyFileEnd(getName(), m_attr.uriId);
            break;
        }

        usleep(10000);
    }
}

bool Session::getSdp(SipUserMessage& sdp)
{
    int num = m_media.size();
    printf("session get SDP media num=%d\n", num);
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
        auto& media = m_media[i];
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
        uint64_t filesize = getFileSize();
        if (filesize > 0)
        {
            std::string str = std::to_string(filesize);
            sdp.addSdpMediaAttr(i, "filesize", str.c_str());
        }
        sdp.setSdpMediaSSRC(i, media->m_rtpParticipant->getSSRC());
    }
    printf("get SDP:\n");
    sdp.print();
    return true;
}

bool Session::convPayloadType(PES::ES_TYPE src, RtpPayload::Type& dst) const
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
    printf("session start\n");
    if (m_bStarted)
    {
        printf("session start error: already started\n");
        return false;
    }

    m_size = 64 * 1024;
    m_buffer = new uint8_t[m_size];
    if (m_buffer == nullptr)
    {
        printf("session start error: malloc buffer failed\n");
        return false;
    }

    m_bStarted = true;
    m_thread = std::move(std::unique_ptr<std::thread>(new std::thread(&Session::threadProc, this)));

    return true;
}

bool Session::stop()
{
    printf("session stop\n");
    if (!m_bStarted)
    {
        printf("session stop error: not started\n");
        return false;
    }

    printf("session wait thread join\n");
    m_bStarted = false;
    if (m_thread != nullptr)
    {
        m_thread->join();
        printf("session thread joined\n");
        m_thread = nullptr;
    }
    delete []m_buffer;
    return true;
}

bool Session::addMedia(const Media::Attr& attr)
{
    Media *media = nullptr;

    if (attr.type == "video")
    {
        /* 优先查找PS，如果不存在则查找当前视频编码类型 */
        auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), RtpPayload::PS);
        if (it != attr.payloadType.end())
        {
            media = new Media(attr, RtpPayload::PS);
        }
        else
        {
            /* 获取当前视频编码类型 */
            RtpPayload::Type payloadType;
            if (!convPayloadType(m_vType, payloadType))
            {
                printf("add media error: failed conv type %d->%d\n", m_vType, payloadType);
                return false;
            }

            /* 查询当前视频编码是否匹配SDP请求 */
            auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
            if (it != attr.payloadType.end())
            {
                media = new Media(attr, payloadType);
            }
            else // 不支持这个类型
            {
                printf("add media error: %d not supported\n", payloadType);
                return false;
            }
        }
    }
    else if (attr.type == "audio")
    {
        /* 获取当前音频编码类型 */
        RtpPayload::Type payloadType;
        if (!convPayloadType(m_aType, payloadType))
        {
            printf("add media error: failed conv type %d->%d\n", m_aType, payloadType);
            return false;
        }

        /* 查询当前音频编码是否匹配SDP请求 */
        auto it = std::find(attr.payloadType.begin(), attr.payloadType.end(), payloadType);
        if (it != attr.payloadType.end())
        {
            media = new Media(attr, payloadType);
        }
        else // 不支持这个类型
        {
            printf("add media error: %d not supported\n", payloadType);
            return false;
        }
    }

    if (media != nullptr)
    {
        printf("add media success: payloadType=%d\n", media->m_rtpPayloadType);
        m_media.push_back(std::move(std::unique_ptr<Media>(media)));
        m_media.back()->connect();
        return true;
    }
    return false;
}

SessionPlay::SessionPlay(SessionAgent *agent, const Attr& attr, const std::string& subject)
    : Session(agent, attr, subject)
{
    printf("++++++ SessionPlay %d:%p\n", m_agent->getCh(), this);
    m_devPlay = std::move(std::unique_ptr<DevPlay>(new DevPlay(m_agent->getCh())));
    if (m_devPlay != nullptr)
    {
        m_vType = m_devPlay->getVideoType();
        m_aType = m_devPlay->getAudioType();
    }
}

SessionPlay::~SessionPlay()
{
    printf("------ SessionPlay %d:%p\n", m_agent->getCh(), this);
    stop();
}

bool SessionPlay::start()
{
    printf("sessionPlay start\n");
    if (m_devPlay == nullptr || !m_devPlay->start())
    {
        return false;
    }
    
    return Session::start();
}

bool SessionPlay::stop()
{
    printf("sessionPlay stop\n");
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

int32_t SessionPlay::fetchVideo(uint8_t *data, int32_t size)
{
    return m_devPlay->getVideo(data, size);
}

int32_t SessionPlay::fetchAudio(uint8_t *data, int32_t size)
{
    return m_devPlay->getAudio(data, size);
}

SessionPlayback::SessionPlayback(SessionAgent *agent, const Attr& attr, const std::string& subject)
    : Session(agent, attr, subject)
    , m_rtsp(new MANSRTSPAgent(m_agent->getUA(), this))
{
    printf("++++++ SessionPlayback %d:%p\n", m_agent->getCh(), this);
    m_devPlayback = std::move(std::unique_ptr<DevPlayback>(new DevPlayback(m_agent->getCh())));
    if (m_devPlayback != nullptr)
    {
        m_devPlayback->setTime(attr.startTime, attr.endTime);
        m_vType = m_devPlayback->getVideoType();
        m_aType = m_devPlayback->getAudioType();
    }
}

SessionPlayback::~SessionPlayback()
{
    printf("------ SessionPlayback %d:%p\n", m_agent->getCh(), this);
    stop();
}

bool SessionPlayback::start()
{
    printf("sessionPlayback start\n");
    if (m_devPlayback == nullptr || !m_devPlayback->start())
    {
        return false;
    }

    return Session::start();
}

bool SessionPlayback::stop()
{
    printf("sessionPlayback stop\n");
    if (m_devPlayback != nullptr)
    {
        Session::stop();
        m_devPlayback->stop();
    }
    return true;
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
    return m_devPlayback->play();
}

bool SessionPlayback::pause()
{
    return m_devPlayback->pause();
}

bool SessionPlayback::scalePlay(float scale)
{
    return m_devPlayback->setSpeed(scale);
}

bool SessionPlayback::rangePlay(int32_t start)
{
    return m_devPlayback->play(start);
}

bool SessionPlayback::rangePlay(int32_t start, int32_t end)
{
    return m_devPlayback->play(start, end);
}

bool SessionPlayback::teardown()
{
    return m_devPlayback->teardown();
}

bool SessionPlayback::isFileEnd()
{
    return m_devPlayback->isFileEnd();
}

SessionDownload::SessionDownload(SessionAgent *agent, const Attr& attr, const std::string& subject)
    : Session(agent, attr, subject)
{
    printf("++++++ SessionDownload %d:%p\n", m_agent->getCh(), this);
    m_devDownload = std::move(std::unique_ptr<DevDownload>(new DevDownload(m_agent->getCh())));
    if (m_devDownload != nullptr)
    {
        m_devDownload->setTime(attr.startTime, attr.endTime);
        m_vType = m_devDownload->getVideoType();
        m_aType = m_devDownload->getAudioType();
    }
}

SessionDownload::~SessionDownload()
{
    printf("------ SessionDownload %d:%p\n", m_agent->getCh(), this);
    stop();
}

bool SessionDownload::start()
{
    printf("sessionDownload start\n");
    if (m_devDownload == nullptr || !m_devDownload->start())
    {
        return false;
    }

    return Session::start();
}

bool SessionDownload::stop()
{
    printf("sessionDownload stop\n");
    if (m_devDownload != nullptr)
    {
        Session::stop();
        return m_devDownload->stop();
    }
    return false;
}

void SessionDownload::setSpeed(int32_t speed)
{
    if (m_devDownload)
    {
        m_devDownload->setSpeed(speed);
    }
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
    if (m_devDownload)
    {
        return m_devDownload->isFileEnd();
    }
    return true;
}

uint64_t SessionDownload::getFileSize() const
{
    return m_devDownload->getFileSize();
}

SessionAgent::SessionAgent(UA *ua, int32_t ch)
    : Agent(ua), m_ch(ch)
{
    printf("++++++ SessionAgent %d\n", m_ch);
}

SessionAgent::~SessionAgent()
{
    printf("------ SessionAgent %d\n", m_ch);
}

std::string SessionAgent::parseSubject(const std::string& subject) const
{
    if (subject.empty())
    {
        return "";
    }
    
    /* 附录 L
     * Subject字段的格式如下:
     * Subject:媒体流发送者ID:发送方媒体流序列号,媒体流接收者ID:接收方媒体流序列号o
     */
    std::size_t pos = subject.find(","); // 取媒体流发送者
    if (pos == std::string::npos)
    {
        return "";
    }
    
    return subject.substr(0, pos);
}

std::unique_ptr<Session> SessionAgent::createSession(const SipUserMessage& req, const std::string& subject)
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
    printf("session:\n");
    printf("  version=%u\n", attr.version);
    printf("  owner=%s\n", attr.owner.c_str());
    printf("  name=%s\n", attr.name.c_str());
    printf("  uriId=%s\n", attr.uriId.c_str());
    printf("  uriParam=%d\n", attr.uriParam);
    printf("  start=%ld\n", attr.startTime);
    printf("  end=%ld\n", attr.endTime);

    std::unique_ptr<Session> session = Session::create(this, attr, subject);
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
        printf("media:\n");
        printf("  type=%s\n", mattr.type.c_str());
        printf("  port=%u\n", mattr.port);
        printf("  netType=%d\n", mattr.netType);
        printf("  payload=");
        for (int32_t j = 0; (uint32_t)j < mattr.payloadType.size(); j++)
        {
            printf(" %d", mattr.payloadType[j]);
        }
        printf("\n");
        printf("  ipv4=%s\n", mattr.conInfo.ipv4.c_str());
        printf("  ssrc=%u\n", mattr.SSRC);

        if (mattr.type == "video")
        {
            const char *streamnumber = req.getSdpMediaAttr(i, "streamnumber");
            if (strlen(streamnumber) > 0)
            {
                printf("set session streamnumber=%s\n", streamnumber);
                session->setStreamNumber(atoi(streamnumber));
            }

            const char *speed = req.getSdpMediaAttr(i, "speed");
            if (speed && strlen(speed) > 0)
            {
                printf("set session speed=%s\n", speed);
                session->setSpeed(atoi(speed));
            }
        }

        session->addMedia(mattr);
    }
    return session;
}

bool SessionAgent::dispatchINVITE(const SessionIdentifier& id, const SipUserMessage& req)
{
    std::string name = req.getSdpSessionName();

    auto sip = m_ua->getSip();
    if (!sip)
    {
        return false;
    }

    /* GB28181 附录 K
     * 下级平台、设备在接收到呼叫请求后,应判断是否在发送以此媒体源标识的码流,
     * 若已经在发送,则应释放现有媒体流发送链路并按照请求建立新的媒体流发送链路
     */
    std::string subject = parseSubject(req.getHeaderSubject());
    printf("parsed subject source identifier: [%s]\n", subject.c_str());
    if (!subject.empty())
    {
        for (auto& it : m_session)
        {
            if (it.second->m_subject == subject)
            {
                printf("session %d(%s):%s gonna destroy and recreate\n", m_ch, name.c_str(), subject.c_str());
                it.second->stop();
                m_session.erase(it.first);
                break;
            }
        }
    }

    /* 附录G 当设备不能满足更多的呼叫请求时,向发送的Invite请求方发送486错误响应消息 */
    if (isSessionExist(name))
    {
        printf("ch %d session %s existed, response 486\n", m_ch, name.c_str());
        SipUserMessage res486;
        sip->makeSessionResponse(req, res486, 486);
        sip->sendSessionResponse(id, res486);
        return false;
    }

    std::unique_ptr<Session> session = createSession(req, subject);
    if (session != nullptr)
    {
        printf("session %d:%s create succeed\n", m_ch, name.c_str());
        SipUserMessage res200;
        sip->makeSessionResponse(req, res200, 200);
        if (session->getSdp(res200))
        {
            printf("send session response 200\n");
            sip->sendSessionResponse(id, res200);
            m_session[id] = std::move(session);
            return true;
        }
    }
    
    /* 附录G 当设备收到无法满足的SDP时,向发送的Invite请求方发送488错误响应消息 */
    printf("ch %d session failed to meet SDP, name=%s, response 488\n", m_ch, name.c_str());
    SipUserMessage res488;
    sip->makeSessionResponse(req, res488, 488);
    sip->sendSessionResponse(id, res488);
    return false;
}

bool SessionAgent::dispatchACK(const SessionIdentifier& id)
{
    printf("session %d ACK\n", m_ch);
    auto session = m_session.find(id);
    if (session != m_session.end())
    {
        return session->second->start();
    }
    return false;
}

bool SessionAgent::dispatchBYE(const SessionIdentifier& id)
{
    printf("session %d BYE\n", m_ch);
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
        printf("unknown sip message:\n");
        message.print();
        return false;
    }
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

bool SessionAgent::notifyFileEnd(const std::string& name, const std::string& uriId)
{
    for (auto& session : m_session)
    {
        if (strCaseCmp(session.second->getName(), name))
        {
            MediaStatusNotify status(this, session.first);
            return status.notify(uriId, "121");
        }
    }
    return false;
}

bool SessionAgent::sendMediaStatus(const SessionIdentifier& id, const XMLDocument& notify)
{
    auto sip = m_ua->getSip();
    if (!sip)
    {
        return false;
    }

    return sip->sendSessionMessage(id, notify);
}

bool SessionAgent::dispatchMANSRTSP(const MANSRTSP::Message& message)
{
    for (auto& session : m_session)
    {
        if (session.second->isMANSRTSP())
        {
            SessionPlayback *playback = dynamic_cast<SessionPlayback *>(session.second.get());
            if (playback && playback->m_rtsp)
            {
                return playback->m_rtsp->agent(session.first, message);
            }
        }
    }
    return false;
}