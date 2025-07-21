#ifndef SESSION_AGENT_H
#define SESSION_AGENT_H

#include <vector>
#include <memory>
#include <thread>
#include <map>
#include "Agent.h"
#include "PS/PSMux.h"
#include "PS/Packetizer/PacketizedES.h"
#include "RTP/RtpParticipant.h"
#include "Interface/9.2Play.h"
#include "Interface/9.8Playback.h"
#include "Interface/9.9Download.h"

class SessionAgent;
class Session;

struct Connection
{
    std::string ipv4;
};

class Media : public PSCallback
{
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
    std::shared_ptr<PES> m_vpes; // 视频分包器
    std::shared_ptr<PES> m_apes; // 音频分包器
    std::shared_ptr<PSMux> m_psmux; // PS复用器
    std::shared_ptr<RtpParticipant> m_rtpParticipant; // RTP流发布
    RtpPayload::Type m_rtpPayloadType;

public:
    Media(Session* session, const Attr& attr, RtpPayload::Type payloadType);
    virtual ~Media();

public:
    bool input(PES::ES_TYPE type, const uint8_t *data, int32_t size);
    void onProgramStream(const uint8_t *data, int32_t size);
    const std::shared_ptr<RtpParticipant>& getRtpParticipant() const;
    RtpPayload::Type getRtpPaylodaType() const;

public:
    bool connect();
    bool disconnect();
};

class Session
{
public:
    struct Attr
    {
        int32_t version;
        std::string owner;
        std::string name;
        std::string uri;
        time_t startTime;
        time_t endTime;
    };

protected:
    SessionAgent *m_agent;
    std::string m_name;
    std::vector<std::shared_ptr<Media>> m_media;

private:
    std::shared_ptr<std::thread> m_thread;
    bool m_bStarted;
    uint8_t *m_buffer;
    int32_t m_bufferSize;

public:
    Session(SessionAgent *agent) : m_agent(agent) {}
    static std::shared_ptr<Session> create(SessionAgent *agent, const Attr& attr);

private:
    void proc();

public:
    virtual std::shared_ptr<Media> addMedia(const Media::Attr& attr) = 0;
    virtual int32_t read(PES::ES_TYPE &type, uint8_t *data, int32_t size) = 0;
    virtual bool isMANSRTSP() = 0;
    virtual const char* getName() const = 0;
    virtual bool isFileEnd() = 0;

public:
    bool start();
    bool stop();
    bool getSdp(SipUserMessage& sdp);
};

class SessionPlay : public Session
{
private:
    std::shared_ptr<Play> m_devPlay;

public:
    SessionPlay(SessionAgent *m_agent);
    virtual ~SessionPlay();

private:
    virtual std::shared_ptr<Media> addMedia(const Media::Attr& attr);
    virtual int32_t read(PES::ES_TYPE &type, uint8_t *data, int32_t size);
    virtual bool isMANSRTSP() { return false; }
    virtual const char* getName() const { return "Play"; }
    virtual bool isFileEnd() { return false; }
};

class SessionPlayback : public Session
{
private:
    std::shared_ptr<Playback> m_devPlayback;
    time_t m_startTime;
    time_t m_endTime;

public:
    SessionPlayback(SessionAgent *m_agent);
    virtual ~SessionPlayback();

private:
    virtual std::shared_ptr<Media> addMedia(const Media::Attr& attr);
    virtual int32_t read(PES::ES_TYPE &type, uint8_t *data, int32_t size);
    virtual bool isMANSRTSP() { return true; }
    virtual const char* getName() const { return "Playback"; }
    virtual bool isFileEnd();

public:
    bool play();
    bool pause();
    bool scalePlay();
    bool rangePlay();
    bool teardown();
};

class SessionDownload : public Session
{
private:
    std::shared_ptr<Download> m_devDownload;
    time_t m_startTime;
    time_t m_endTime;

public:
    SessionDownload(SessionAgent *m_agent);
    virtual ~SessionDownload();

private:
    virtual std::shared_ptr<Media> addMedia(const Media::Attr& attr);
    virtual int32_t read(PES::ES_TYPE &type, uint8_t *data, int32_t size);
    virtual bool isMANSRTSP() { return false; }
    virtual const char* getName() const { return "Download"; }
    virtual bool isFileEnd();
};

class SessionAgent : public Agent
{
    friend class UA;

private:
    std::map<const SessionIdentifier, std::shared_ptr<Session>> m_session;

public:
    SessionAgent(UA *ua) : Agent(ua) {}
    virtual ~SessionAgent() {}

private:
    RtpNet::Type parseNetType(const std::string& str) const;
    std::shared_ptr<Session> createSession(const SipUserMessage& req);
    bool dispatchINVITE(const SessionIdentifier& id, const SipUserMessage& req);
    bool dispatchACK(const SessionIdentifier& id);
    bool dispatchBYE(const SessionIdentifier& id);
    bool isSessionExist(const std::string& name) const;

public:
    bool match(const std::string& method, const std::string& contentType);
    bool agent(const SipUserMessage& message);
    bool agent(const SessionIdentifier& id, const SipUserMessage& message);
    const std::shared_ptr<SessionPlayback> getMANSRTSPSession() const;
    bool notifyFileEnd() const;
};

#endif