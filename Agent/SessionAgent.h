#ifndef SESSION_AGENT_H
#define SESSION_AGENT_H

#include <vector>
#include <memory>
#include <thread>
#include <map>
#include "Agent.h"
#include "Interface/9.2Play.h"
#include "PS/PSMux.h"
#include "PS/Packetizer/PacketizedES.h"
#include "RTP/RtpParticipant.h"

class SessionAgent;
class Session;

struct Connection
{
    std::string ipv4;
};

class Media : public PSCallback
{
    friend class SessionAgent;
    friend class Session;

public:
    struct Attr
    {
        std::string type;
        int32_t port;
        RtpNet::Type netType;
        std::vector<RtpPayload::Type> payloadType;
        Connection conInfo;
        uint32_t SSRC;
    };

private:
    std::shared_ptr<PES> m_videoPES; // 视频分包器
    std::shared_ptr<PES> m_audioPES; // 音频分包器
    std::shared_ptr<PSMux> m_psMux; // PS复用器
    std::shared_ptr<RtpParticipant> m_rtpParticipant; // RTP流发布
    RtpPayload::Type m_rtpPayloadType;
    std::shared_ptr<std::thread> m_thread;
    bool m_bPublish;
    Session *m_session;

public:
    Media(Session* session, const Attr& attr, RtpPayload::Type payloadType);
    virtual ~Media();

private:
    void proc();

public:
    void onProgramStream(const uint8_t *data, int32_t size);
    const std::shared_ptr<RtpParticipant>& getRtpParticipant() const;
    const std::string& getType() const;
    RtpPayload::Type getRtpPaylodaType() const;

public:
    bool connect();
    bool disconnect();
    bool publish();
    bool unpublish();
};

class Session
{
    friend class SessionAgent; // 允许MediaAgent访问Session的私有成员
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
    std::vector<std::shared_ptr<Media>> m_media;
    SessionAgent *m_agent;
    std::shared_ptr<Connection> m_conInfo;

public:
    Session(SessionAgent *agent, const Attr& attr);
    virtual ~Session();

private:
    std::shared_ptr<Media> addMedia(const Media::Attr& attr);

public:
    bool start();
    bool stop();
};

class SessionAgent : public Agent
{
    friend class UA;
    friend class Session; // 允许Session访问MediaAgent的私有成员play
    friend class Media; // 允许Media访问MediaAgent的私有成员play

private:
    std::shared_ptr<Play> m_devPlay;
    std::shared_ptr<Session> m_sessionPlay; //实时视音频点播

public:
    SessionAgent(UA *ua);
    ~SessionAgent();

private:
    bool dispatchINVITE(const SipUserMessage& req);
    bool dispatchACK();
    bool dispatchBYE();

    RtpNet::Type parseNetType(const std::string& str) const;

public:
    bool match(const std::string& method, const std::string& contentType);
    bool agent(const SipUserMessage& message);
};

#endif