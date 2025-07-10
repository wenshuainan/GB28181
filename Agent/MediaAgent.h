#ifndef MEDIA_AGENT_H
#define MEDIA_AGENT_H

#include <memory>
#include <thread>
#include <map>
#include "Agent.h"
#include "9.2Play.h"
#include "PS/PSMux.h"
#include "PS/Packetizer/PacketizedES.h"
#include "RTP/RtpParticipant.h"

class MediaAgent;
class Session;

struct Connection
{
    std::string ipv4;
};

class Media : public PSCallback
{
    friend class MediaAgent;
    friend class Session;

public:
    struct Attr
    {
        int32_t port;
        RtpNet::Type netType;
        RtpPayload::Type payloadType;
        Connection *conInfo;
        uint32_t *SSRC;
    };

private:
    std::shared_ptr<PES> videoPES; // 视频分包器
    std::shared_ptr<PES> audioPES; // 音频分包器
    std::shared_ptr<PSMux> psMux; // PS复用器
    std::shared_ptr<RtpParticipant> rtpParticipant; // RTP流发布
    RtpPayload::Type rtpPayloadType;
    std::thread *thread;
    bool bRunning; // 是否正在运行
    Session *m_session;

public:
    Media(Session* session, const Attr& attr);
    virtual ~Media();

private:
    void proc();

public:
    void onProgramStream(const uint8_t *data, int32_t size);

public:
    bool connect();
    bool disconnect();
    bool publish();
    bool unpublish();
};

class Session
{
    friend class MediaAgent; // 允许MediaAgent访问Session的私有成员
    friend class Media;

public:
    struct Attr
    {
        int32_t version;
        std::string owner;
        std::string name;
        Connection *conInfo;
    };

private:
    std::vector<std::shared_ptr<Media>> media;
    MediaAgent *m_agent;
    std::shared_ptr<Connection> m_conInfo;

public:
    Session(MediaAgent *agent, const Attr& attr);
    virtual ~Session();

private:
    bool addMedia(const Media::Attr& attr);

public:
    bool start();
    bool stop();
};

class MediaAgent : public Agent
{
    friend class UA;
    friend class Session; // 允许Session访问MediaAgent的私有成员play
    friend class Media; // 允许Media访问MediaAgent的私有成员play

private:
    std::shared_ptr<Play> play;
    std::shared_ptr<Session> m_sessionPlay; //实时视音频点播

public:
    MediaAgent(UA *ua);
    ~MediaAgent();

private:
    bool agentReqINVITE(const SipMessageApp& req);
    bool agentReqACK(const SipMessageApp& req);
    bool agentReqBYE(const SipMessageApp& req);

    RtpNet::Type parseNetType(const std::string& str) const;

public:
    bool match(const std::string& method, const std::string& contentType);
    bool agent(const SipMessageApp& message);
};

#endif