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
    friend Session;

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
    Media(const Attr& attr, RtpPayload::Type payloadType);
    virtual ~Media();

public:
    virtual void onProgramStream(const uint8_t *data, int32_t size);

public:
    bool connect();
    bool disconnect();
    bool input(PES::ES_TYPE type, const uint8_t *data, int32_t size);
};

class Session
{
    friend SessionAgent;

public:
    struct Attr
    {
        int32_t version;
        std::string owner;
        std::string name;
        std::string uriId;
        int32_t uriParam;
        time_t startTime;
        time_t endTime;
    };

private:
    Attr m_attr;
    std::shared_ptr<std::thread> m_thread;
    bool m_bStarted;
    uint8_t *m_buffer;
    int32_t m_size;

protected:
    SessionAgent *m_agent;
    std::vector<std::shared_ptr<Media>> m_media;
    PES::ES_TYPE m_vType;
    PES::ES_TYPE m_aType;

public:
    Session(SessionAgent *agent, const Attr& attr);
    virtual ~Session();
    static std::shared_ptr<Session> create(SessionAgent *agent, const Attr& attr);

private:
    void threadProc();
    bool getSdp(SipUserMessage& sdp);
    virtual void setStreamNumber(int32_t num) { return; }

protected:
    bool convertPayloadType(PES::ES_TYPE src, RtpPayload::Type& dst) const;

public:
    virtual bool start();
    virtual bool stop();
    virtual std::shared_ptr<Media> addMedia(const Media::Attr& attr) = 0;
    virtual int32_t fetchVideo(uint8_t *data, int32_t size) = 0;
    virtual int32_t fetchAudio(uint8_t *data, int32_t size) = 0;
    virtual bool isMANSRTSP() const = 0;
    virtual const char* getName() const = 0;
    virtual bool isFileEnd() = 0;
};

class SessionPlay : public Session
{
private:
    std::shared_ptr<Play> m_devPlay;

public:
    SessionPlay(SessionAgent *m_agent, const Attr& attr);
    virtual ~SessionPlay();

public:
    virtual bool start();
    virtual bool stop();
    virtual void setStreamNumber(int32_t num);
    virtual std::shared_ptr<Media> addMedia(const Media::Attr& attr);
    virtual int32_t fetchVideo(uint8_t *data, int32_t size);
    virtual int32_t fetchAudio(uint8_t *data, int32_t size);
    virtual bool isMANSRTSP() const { return false; } // not support MANSRTSP
    virtual const char* getName() const { return "Play"; }
    virtual bool isFileEnd() { return false; } // never end
};

class SessionPlayback : public Session
{
private:
    std::shared_ptr<Playback> m_devPlayback;

public:
    SessionPlayback(SessionAgent *m_agent, const Attr& attr);
    virtual ~SessionPlayback();

public:
    virtual std::shared_ptr<Media> addMedia(const Media::Attr& attr);
    virtual int32_t fetchVideo(uint8_t *data, int32_t size);
    virtual int32_t fetchAudio(uint8_t *data, int32_t size);
    virtual bool isMANSRTSP() const { return true; } // playback support MANSRTSP
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

public:
    SessionDownload(SessionAgent *m_agent, const Attr& attr);
    virtual ~SessionDownload();

public:
    virtual std::shared_ptr<Media> addMedia(const Media::Attr& attr);
    virtual int32_t fetchVideo(uint8_t *data, int32_t size);
    virtual int32_t fetchAudio(uint8_t *data, int32_t size);
    virtual bool isMANSRTSP() const { return false; } // download not support MANSRTSP
    virtual const char* getName() const { return "Download"; }
    virtual bool isFileEnd();
};

class SessionAgent : public Agent
{
    friend Session;
    friend SessionPlay;
    friend SessionPlayback;
    friend SessionDownload;
    
private:
    int32_t m_ch;
    std::map<const SessionIdentifier, std::shared_ptr<Session>> m_session;

public:
    SessionAgent(UA *ua, int32_t ch);
    virtual ~SessionAgent();

private:
    RtpNet::Type parseNetType(const std::string& str) const;
    std::shared_ptr<Session> createSession(const SipUserMessage& req);
    bool dispatchINVITE(const SessionIdentifier& id, const SipUserMessage& req);
    bool dispatchACK(const SessionIdentifier& id);
    bool dispatchBYE(const SessionIdentifier& id);
    bool isSessionExist(const std::string& name) const;
    bool notifyFileEnd(const std::string& name, const std::string& uriId) const;

public:
    bool match(const std::string& method, const std::string& contentType);
    bool agent(const SipUserMessage& message);
    bool agent(const SessionIdentifier& id, const SipUserMessage& message);
    const std::pair<const SessionIdentifier, std::shared_ptr<SessionPlayback>> getMANSRTSPSession() const;
};

#endif