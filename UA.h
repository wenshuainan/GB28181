#ifndef GB28181_USER_AGENT_H
#define GB28181_USER_AGENT_H

#include <vector>
#include <memory>
#include <thread>
#include "SipAdapter.h"
#include "MANSCDP/A.2.5Notify.h"
#include "MANSRTSP/B.1Message.h"

class Agent;
class RegistrationAgent;
class MANSCDPAgent;
class SessionAgent;
class MANSRTSPAgent;

class UA
{
    friend class SipUserAgent;
    friend class Agent;
    friend class RegistrationAgent;
    friend class MANSCDPAgent;
    friend class SessionAgent;
    friend class MANSRTSPAgent;

public:
    struct KeepaliveInfo
    {
        int32_t interval;
        int32_t timeoutCount;
    };
    
private:
    bool m_bStarted;
    bool m_bOnline;
    std::shared_ptr<SipUserAgent> m_sip;
    std::vector<std::shared_ptr<Agent>> m_agents;
    std::shared_ptr<RegistrationAgent> m_registAgent;
    std::shared_ptr<MANSCDPAgent> m_cdpAgent;
    std::shared_ptr<SessionAgent> m_sessionAgent;
    std::shared_ptr<MANSRTSPAgent> m_rtspAgent;
    std::shared_ptr<std::thread> m_thread;
    KeepaliveInfo m_kaInfo;

public:
    UA();
    ~UA();

private:
    void proc();

private:
    bool dispatchRegistrationResponse(const SipUserMessage& res);
    bool dispatchSessionRequest(const SipUserMessage& req);
    bool dispatchMANSCDPRequest(const XMLDocument &req);
    bool dispatchKeepaliveResponse(int32_t code);
    bool dispatchMANSRTSPRequest(const MANSRTSP::Message& req);
    const std::shared_ptr<SipUserAgent>& getSip() const { return m_sip; }
    void setStatus(bool online);

public:
    bool start(const SipUserAgent::ClientInfo& client,
                const SipUserAgent::ServerInfo& server,
                const KeepaliveInfo &keepalive
            );
    bool stop();
    bool getStatus() const;

    // 设备主动向服务器发送
    bool updateStatus(const KeepAliveNotify::Request &notify); // 9.6.1 立即发送状态信息
};

#endif