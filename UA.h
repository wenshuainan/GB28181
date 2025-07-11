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

#define DEBUG_LOG std::cout << "GB28181: " << __FILE__ << ":" << __LINE__ << " "

class UA
{
public:
    struct KeepaliveInfo
    {
        int32_t interval;
        int32_t timeoutCount;
    };
    
private:
    bool bOnline;
    std::vector<std::shared_ptr<Agent>> agents;
    std::shared_ptr<RegistrationAgent> regAgent;
    std::shared_ptr<MANSCDPAgent> mansAgent;
    std::shared_ptr<SessionAgent> sessionAgent;
    std::shared_ptr<SipUserAgent> sip;
    std::thread *keepaliveThread;
    bool bKeepaliveRunning;
    KeepaliveInfo keepaliveInfo;

public:
    UA();
    ~UA();

private:
    void keepaliveProc();

public:
    bool recvRegistrationResponse(const SipMessageApp& res);
    bool recvOutDialogRequest(const SipMessageApp& req);
    bool recvOutDialogResponse(const SipMessageApp& res, const SipMessageApp& req);
    bool recvSessionRequest(const SipMessageApp& req);
    const std::shared_ptr<SipUserAgent>& getSip() { return sip; }
    void setOnline();
    bool startKeepalive();
    bool stopKeepalive();

public:
    bool start(const SipUserAgent::ClientInfo& client, const SipUserAgent::ServerInfo& server, const KeepaliveInfo &keepalive);
    bool stop();

    // 需要设备主动向服务器发送的
    bool updateStatus(const KeepAliveNotify::Request &notify); // 9.6.1 立即发送状态信息
};

#endif