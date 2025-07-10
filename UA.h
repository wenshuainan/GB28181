#ifndef GB28181_USER_AGENT_H
#define GB28181_USER_AGENT_H

#include <vector>
#include <memory>
#include <thread>
#include "SipAdapter.h"
#include "Agent.h"
#include "A.2.5Notify.h"

#define DEBUG_LOG std::cout << "GB28181: " << __FILE__ << ":" << __LINE__ << " "

class UA
{
public:
    struct KeepaliveInfo
    {
        int32_t interval;
        int32_t retryCount;
    };
    
private:
    std::vector<std::shared_ptr<Agent>> agents;
    std::shared_ptr<SipUserAgent> sip;
    std::thread *keepaliveThread;
    bool bKeepaliveRunning;

public:
    UA();
    ~UA();

private:
    void keepaliveProc();

public:
    bool postRegistrationResponse(const SipMessageApp& message);
    bool postInviteSessionRequest(const SipMessageApp& message);
    bool postOutDialogRequest(const SipMessageApp& request);
    bool postOutDialogResponse(const SipMessageApp& response, const SipMessageApp& request);
    const std::shared_ptr<SipUserAgent>& getSip() { return sip; }
    bool startKeepalive();
    bool stopKeepalive();

public:
    bool start(const SipUserAgent::ClientInfo& client, const SipUserAgent::ServerInfo& server);
    bool stop();

    // 需要设备主动向服务器发送的
    bool updateStatus(const KeepAliveNotify &notify); // 9.6.1 立即发送状态信息
};

#endif