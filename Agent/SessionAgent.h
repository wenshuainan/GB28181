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
class MANSRTSPAgent;

struct Connection
{
    std::string ipv4;
};

/* SDP媒体流类型 */
class Media : public PSCallback
{
    friend Session;

public:
    /* SDP媒体描述参数 */
    struct Attr
    {
        std::string type; // m=行的媒体类型，video/audio
        int32_t port; // m=行的媒体传输端口，例如51372
        RtpNet::Type netType; // m=行的传输协议类型，例如RTP/AVP
        std::vector<RtpPayload::Type> payloadType; // m=行的payload类型列表，例如96
        Connection conInfo; // c=行的媒体连接信息，如果没有则使用session的连接信息
        uint32_t SSRC; // y=行的SSRC值
    };

private:
    std::unique_ptr<PES> m_vpes; // 视频分包器
    std::unique_ptr<PES> m_apes; // 音频分包器
    std::unique_ptr<PSMux> m_psmux; // PS复用器
    std::unique_ptr<RtpParticipant> m_rtpParticipant; // RTP流发布
    RtpPayload::Type m_rtpPayloadType;
    std::unique_ptr<std::thread> m_thread; // 连接线程
    bool m_bConnect; // 连接线程运行标志

public:
    Media(const Attr& attr, RtpPayload::Type payloadType);
    virtual ~Media();

private:
    void connectProc();

public:
    virtual void onProgramStream(const uint8_t *data, int32_t size);

public:
    bool connect();
    bool disconnect();
    bool input(PES::ES_TYPE type, const uint8_t *data, int32_t size);
    void finished();
};

/* SDP媒体会话基类 */
class Session
{
    friend SessionAgent;

public:
    /* SDP会话描述信息 */
    struct Attr
    {
        int32_t version; // v=
        std::string owner; // o=
        std::string name; // s=
        std::string uriId; // u=
        int32_t uriParam; // u=行参数，0表示all;1表示 manual;2表示alarm;3表示time
        time_t startTime; // t=起始时间
        time_t endTime; // t=结束时间
    };

private:
    Attr m_attr;
    std::unique_ptr<std::thread> m_thread;
    bool m_bStarted;
    uint8_t *m_buffer; // 媒体源数据缓存
    int32_t m_size; // 缓存大小
    std::string m_subject;

protected:
    SessionAgent *m_agent; // 会话所属代理
    std::vector<std::unique_ptr<Media>> m_media; // 会话中的媒体流
    PES::ES_TYPE m_vType; // 会话源视频流格式
    PES::ES_TYPE m_aType; // 会话源音频流格式

public:
    Session(SessionAgent *agent, const Attr& attr, const std::string& subject);
    virtual ~Session();
    static std::unique_ptr<Session> create(SessionAgent *agent, const Attr& attr, const std::string& subject);

private:
    void threadProc();
    bool getSdp(SipUserMessage& sdp);
    bool convPayloadType(PES::ES_TYPE src, RtpPayload::Type& dst) const;

public:
    virtual bool start();
    virtual bool stop();
    virtual bool addMedia(const Media::Attr& attr);
    virtual void setStreamNumber(int32_t num) = 0;
    virtual void setSpeed(int32_t speed) = 0;
    virtual int32_t fetchVideo(uint8_t *data, int32_t size) = 0;
    virtual int32_t fetchAudio(uint8_t *data, int32_t size) = 0;
    virtual bool isMANSRTSP() const = 0;
    virtual const char* getName() const = 0;
    virtual bool isFileEnd() = 0;
    virtual uint64_t getFileSize() const = 0;
};

/*  SDP s=Play 会话类，实时视音频点播 */
class SessionPlay : public Session
{
private:
    std::unique_ptr<Play> m_devPlay; // 设备实时视音频播放适配类

public:
    SessionPlay(SessionAgent *m_agent, const Attr& attr, const std::string& subject);
    virtual ~SessionPlay();

public:
    virtual bool start();
    virtual bool stop();
    virtual void setStreamNumber(int32_t num);
    virtual void setSpeed(int32_t speed) { return; } // not support
    virtual int32_t fetchVideo(uint8_t *data, int32_t size);
    virtual int32_t fetchAudio(uint8_t *data, int32_t size);
    virtual bool isMANSRTSP() const { return false; } // not support MANSRTSP
    virtual const char* getName() const { return "Play"; }
    virtual bool isFileEnd() { return false; } // never end
    virtual uint64_t getFileSize() const { return 0; } // no file
};

/*  SDP s=Playback 会话类，历史视音频回放 */
class SessionPlayback : public Session
{
    friend SessionAgent;

private:
    std::unique_ptr<Playback> m_devPlayback; // 设备回放适配类
    std::unique_ptr<MANSRTSPAgent> m_rtsp; // 回放控制协议代理

public:
    SessionPlayback(SessionAgent *m_agent, const Attr& attr, const std::string& subject);
    virtual ~SessionPlayback();

public:
    virtual bool start();
    virtual bool stop();
    virtual void setStreamNumber(int32_t num) { return; } // not support
    virtual void setSpeed(int32_t speed) { return; } // no need
    virtual int32_t fetchVideo(uint8_t *data, int32_t size);
    virtual int32_t fetchAudio(uint8_t *data, int32_t size);
    virtual bool isMANSRTSP() const { return true; } // playback support MANSRTSP
    virtual const char* getName() const { return "Playback"; }
    virtual bool isFileEnd();
    virtual uint64_t getFileSize() const { return 0; } // no need file size

public:
    bool play();
    bool pause();
    bool scalePlay(float scale);
    bool rangePlay(int32_t start);
    bool rangePlay(int32_t start, int32_t end);
    bool teardown();
};

/*  SDP s=Download 会话类，视音频文件下载 */
class SessionDownload : public Session
{
private:
    std::unique_ptr<Download> m_devDownload; // 设备文件下载适配类

public:
    SessionDownload(SessionAgent *m_agent, const Attr& attr, const std::string& subject);
    virtual ~SessionDownload();

public:
    virtual bool start();
    virtual bool stop();
    virtual void setStreamNumber(int32_t num) { return; } // not support
    virtual void setSpeed(int32_t speed);
    virtual int32_t fetchVideo(uint8_t *data, int32_t size);
    virtual int32_t fetchAudio(uint8_t *data, int32_t size);
    virtual bool isMANSRTSP() const { return false; } // download not support MANSRTSP
    virtual const char* getName() const { return "Download"; }
    virtual bool isFileEnd();
    virtual uint64_t getFileSize() const;
};

/* 媒体会话代理
 * 各自视频编码通道的代理，处理INVITE、ACK、BYE等请求
 * 根据请求开始/停止session，并创建/销毁session的Media媒体流
 */
class SessionAgent : public Agent
{
private:
    int32_t m_ch; // 代理的通道
    std::map<const SessionIdentifier, std::unique_ptr<Session>> m_session; // 会话标识与session的映射关系

public:
    SessionAgent(UA *ua, int32_t ch);
    virtual ~SessionAgent();

private:
    std::string parseSubject(const std::string& subject) const;

public:
    RtpNet::Type parseNetType(const std::string& str) const;
    std::unique_ptr<Session> createSession(const SipUserMessage& req, const std::string& subject);
    bool dispatchINVITE(const SessionIdentifier& id, const SipUserMessage& req);
    bool dispatchACK(const SessionIdentifier& id);
    bool dispatchBYE(const SessionIdentifier& id);
    bool isSessionExist(const std::string& name) const;
    bool notifyFileEnd(const std::string& name, const std::string& uriId);
    UA* getUA() const { return m_ua; }
    int32_t getCh() const { return m_ch; }
    bool match(const std::string& method, const std::string& contentType);
    bool agent(const SipUserMessage& message);
    bool agent(const SessionIdentifier& id, const SipUserMessage& message);
    bool sendMediaStatus(const SessionIdentifier& id, const XMLDocument& notify);
    bool dispatchMANSRTSP(const MANSRTSP::Message& message);
};

#endif