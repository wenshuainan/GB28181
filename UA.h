#ifndef GB28181_USER_AGENT_H
#define GB28181_USER_AGENT_H

#include <vector>
#include <memory>
#include <thread>
#include "SipAdapter.h"
#include "Agent.h"
#include "A.2.5Notify.h"
#include "RegistrationAgent.h"
#include "MANSCDPAgent.h"
#include "SessionAgent.h"

class UA
{
public:
    struct KeepaliveInfo
    {
        int32_t interval;
        int32_t timeoutCount;
    };
    
private:
    bool m_bOnline;
    std::vector<std::shared_ptr<Agent>> m_agents;
    std::shared_ptr<RegistrationAgent> m_registAgent;
    std::shared_ptr<MANSCDPAgent> m_mansAgent;
    std::shared_ptr<SessionAgent> m_sessionAgent;
    std::shared_ptr<SipUserAgent> m_sip;
    std::thread *m_keepaliveThread;
    bool m_bKeepaliveRunning;
    KeepaliveInfo m_keepaliveInfo;

public:
    UA();
    ~UA();

private:
    void keepaliveProc();

public:
    bool dispatchRegistrationResponse(const SipUserMessage& res);
    bool dispatchSessionRequest(const SipUserMessage& req);
    bool dispatchMANSCDPRequest(const XMLDocument &req);
    bool dispatchKeepaliveResponse(int32_t code);
    const std::shared_ptr<SipUserAgent>& getSip() { return m_sip; }
    void setOnline();
    bool startKeepalive();
    bool stopKeepalive();

public:
    bool start(const SipUserAgent::ClientInfo& client, const SipUserAgent::ServerInfo& server, const KeepaliveInfo &keepalive);
    bool stop();

    // 设备主动向服务器发送
    bool updateStatus(const KeepAliveNotify::Request &notify); // 9.6.1 立即发送状态信息
};

#endif