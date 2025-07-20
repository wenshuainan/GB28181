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
    std::string name;
    std::vector<std::shared_ptr<Media>> m_media;

public:
    static std::shared_ptr<Session> create(const Attr& attr);

public:
    virtual std::shared_ptr<Media> addMedia(const Media::Attr& attr) = 0;
    virtual int32_t readVideo(uint8_t *data, int32_t size) = 0;
    virtual int32_t readAudio(uint8_t *data, int32_t size) = 0;
    virtual bool isMANSRTSP() = 0;
    virtual const std::string& getName() const = 0;

public:
    bool start();
    bool stop();
};

class SessionPlay : public Session
{
private:
    std::shared_ptr<Play> m_devPlay;

private:
    virtual std::shared_ptr<Media> addMedia(const Media::Attr& attr);
    virtual int32_t readVideo(uint8_t *data, int32_t size);
    virtual int32_t readAudio(uint8_t *data, int32_t size);
    virtual bool isMANSRTSP() { return false; }
    const std::string& getName() const { return "Play"; }
};

class SessionPlayback : public Session
{
private:
    std::shared_ptr<Playback> m_devPlayback;

private:
    virtual std::shared_ptr<Media> addMedia(const Media::Attr& attr);
    virtual int32_t readVideo(uint8_t *data, int32_t size);
    virtual int32_t readAudio(uint8_t *data, int32_t size);
    virtual bool isMANSRTSP() { return true; }
    const std::string& getName() const { return "Playback"; }

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

private:
    virtual std::shared_ptr<Media> addMedia(const Media::Attr& attr);
    virtual int32_t readVideo(uint8_t *data, int32_t size);
    virtual int32_t readAudio(uint8_t *data, int32_t size);
    virtual bool isMANSRTSP() { return false; }
    const std::string& getName() const { return "Download"; }
};

class SessionAgent : public Agent
{
    friend class UA;

    typedef uint64_t SessionIdentifier;

private:
    std::map<const SessionIdentifier&, std::shared_ptr<Session>> m_session;

public:
    SessionAgent(UA *ua) : Agent(ua) {}
    virtual ~SessionAgent() {}

private:
    RtpNet::Type parseNetType(const std::string& str) const;
    std::shared_ptr<Session> createSession(const SipUserMessage& req);
    bool dispatchINVITE(const SessionIdentifier& id, const SipUserMessage& req);
    bool dispatchACK(const SessionIdentifier& id);
    bool dispatchBYE(const SessionIdentifier& id);

public:
    bool match(const std::string& method, const std::string& contentType);
    bool agent(const SipUserMessage& message);
    const std::shared_ptr<Session>& getMANSRTSPSession();
    bool isSessionExist(const std::string& name) const;
};

#endif