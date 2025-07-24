#ifndef GB28181_USER_AGENT_H
#define GB28181_USER_AGENT_H

#include <vector>
#include <memory>
#include <thread>
#include <unordered_map>
#include "SipAdapter.h"

class RegistrationAgent;
class MANSCDPAgent;
class SessionAgent;
class MANSRTSPAgent;

class UA
{
    friend class SipUserAgent;
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
    std::shared_ptr<RegistrationAgent> m_registAgent;
    std::shared_ptr<MANSCDPAgent> m_cdpAgent;
    std::vector<std::shared_ptr<SessionAgent>> m_sessionAgent;
    std::shared_ptr<MANSRTSPAgent> m_rtspAgent;
    std::shared_ptr<std::thread> m_thread;
    KeepaliveInfo m_kaInfo;
    std::unordered_map<std::string, int32_t> m_channels;

public:
    UA();
    virtual ~UA();

private:
    void threadProc();
    bool dispatchRegistrationResponse(const SipUserMessage& res);
    bool dispatchKeepaliveResponse(int32_t code);
    bool dispatchMANSCDPRequest(const XMLDocument &req);
    bool dispatchSessionRequest(const SessionIdentifier& id, const SipUserMessage& req);
    bool dispatchMANSRTSPRequest(const SipUserMessage& req);
    void setOnline(bool online);
    int32_t getChNum(const std::string& id) const;

public:
    bool start(const SipUserAgent::ClientInfo& client,
        const SipUserAgent::ServerInfo& server,
        const KeepaliveInfo &keepalive,
        const std::vector<std::string>& catalogIds
    );
    bool stop();
    bool getOnline() const;

    /* 设备主动向服务器发送 */
    bool sendStatusNotify(); // 9.6.1 立即发送状态信息
};

#endif