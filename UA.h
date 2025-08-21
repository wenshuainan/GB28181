#ifndef GB28181_USER_AGENT_H
#define GB28181_USER_AGENT_H

#include <vector>
#include <memory>
#include <thread>
#include <unordered_map>
#include "SIP/Adapter/SipAdapter.h"

class RegistrationAgent;
class MANSCDPAgent;
class SessionAgent;

class UA
{
public:
    struct KeepaliveInfo
    {
        int32_t interval;       //保活间隔，以秒为单位
        int32_t timeoutCount;   //保活超时次数，超出认为离线
    };
    
private:
    bool m_bStarted;
    bool m_bOnline; // 在线状态，在线：注册成功，离线：未注册/已注销/心跳失败
    std::shared_ptr<SipUserAgent> m_sip; // sip用户代理
    std::unique_ptr<RegistrationAgent> m_regAgent; // 注册功能代理
    std::unique_ptr<MANSCDPAgent> m_cdpAgent; // MANSCDP协议代理
    std::vector<std::unique_ptr<SessionAgent>> m_sessionAgent; // INVITE会话代理，每个视频编码通道一个
    std::unique_ptr<std::thread> m_thread;
    KeepaliveInfo m_keepaliveInfo; // 用户配置保活信息
    std::unordered_map<std::string, int32_t> m_channels; // 用户配置目录id与视频编码通道映射关系

public:
    UA();
    virtual ~UA();

public:
    void threadProc();
    /// @brief 注册响应消息处理
    /// @param res 响应消息
    /// @return 
    bool dispatchRegistrationResponse(const SipUserMessage& res);
    /// @brief MANSCDP请求命令处理
    /// @param cmd 请求命令
    /// @return 
    bool dispatchMANSCDPRequest(const XMLDocument &cmd);
    /// @brief MANSCDP命令结果处理
    /// @param code SIP状态码
    /// @param cmd UA发送的命令
    /// @return 
    bool dispatchMANSCDPResult(int32_t code, const XMLDocument &cmd);
    /// @brief INVITE会话请求处理
    /// @param id session标识
    /// @param req INVITE请求消息
    /// @return 
    bool dispatchSessionRequest(const SessionIdentifier& id, const SipUserMessage& req);
    /// @brief MANSRTSP请求消息处理
    /// @param req 请求消息
    /// @return 
    bool dispatchMANSRTSPRequest(const SipUserMessage& req);
    void setOnline(bool online);
    const std::unordered_map<std::string, int32_t>& getChannels() const;
    int32_t getChannel(const std::string& id) const;
    std::shared_ptr<SipUserAgent> getSip() const;

public:
    bool start(const SipUserAgent::ClientInfo& client,
               const SipUserAgent::ServerInfo& server,
               const KeepaliveInfo &keepalive,
               const std::vector<std::string>& catalogIds);
    bool stop();
    bool getOnline() const;
};

#endif