#ifndef SIP_ADAPTER_H
#define SIP_ADAPTER_H

#include <string>
#include <memory>
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;

struct SipAdapterMessage;

class UA;

namespace MANSRTSP
{
    class Message;
}

typedef uint64_t SessionIdentifier;

class SipUserMessage
{
private:
    std::shared_ptr<SipAdapterMessage> m_adapter;

public:
    SipUserMessage();
    ~SipUserMessage();
    void print() const;

    bool setAdapter(const SipAdapterMessage& adapter);
    const std::shared_ptr<SipAdapterMessage>& getAdapter() const;

    const char* getMethod() const;
    int getCode() const;
    const char* getReasonPhrase() const;
    const char* getRequestUser() const;
    const char* getHeaderDate() const;

    int32_t getSdpSessionVersion() const;
    const char* getSdpSessionOwner() const;
    const char* getSdpSessionName() const;
    const char* getSdpSessionIpv4() const;
    const char* getSdpSessionUriId() const;
    int32_t getSdpSessionUriParam() const;
    time_t getSdpTimeStart() const;
    time_t getSdpTimeEnd() const;
    int32_t getSdpMediaNum() const;
    const char* getSdpMediaType(int32_t index) const;
    int32_t getSdpMediaPort(int32_t index) const;
    const char* getSdpMediaTransport(int32_t index) const;
    int32_t getSdpMediaPayloadType(int32_t index, uint16_t type[10]) const;
    const char* getSdpMediaIpv4(int32_t index) const;
    uint32_t getSdpMediaSSRC(int32_t index) const;
    const char* getSdpMediaAttr(int32_t index, const char *key) const;

    const MANSRTSP::Message* getMANSRTSP() const;

    bool setSdpMediaNum(int32_t num);
    bool setSdpMediaType(int32_t index, const char *type);
    bool setSdpMediaPort(int32_t index, int32_t port);
    bool setSdpMediaTransport(int32_t index, const char *transport);
    bool setSdpMediaPayloadType(int32_t index, int32_t type);
    bool addSdpMediaAttr(int32_t index, const char *key, const char *value);
    bool setSdpMediaSSRC(int32_t index, uint32_t value);

    bool setExpires(int32_t expires);
    bool addExtensionField(const std::string& name, const std::string& value);
};

class SipUserAgent
{
public:
    enum Transport
    {
        TRANSPORT_UDP,
        TRANSPORT_TCP,
        TRANSPORT_TLS
    };

    /* SIP UA */
    struct ClientInfo
    {
        std::string id; // 设备国标编码，GB28181附录E
        std::string ipv4; // 设备IPV4，可以不填，默认使用第一张网卡
        int32_t port; // 绑定端口，可以不填，默认5060
        std::string passwd; // 服务器注册密码
        Transport transport; // SIP信令传输方式
        int32_t expire; // 注册过期时间，单位：S
        int32_t interval; // 注册失败间隔，失败后将在间隔时间后重新发起注册，单位：S
    };

    /* SIP服务器 */
    struct ServerInfo
    {
        std::string id; // 国标编码
        std::string domain; // SIP域
        std::string ipv4; // 服务器IPV4
        int32_t port; // 服务器端口
    };

protected:
    UA *m_user;

public:
    SipUserAgent() {}
    virtual ~SipUserAgent() {}

public:
    /// @brief 启动SIP协议栈
    /// @return 
    virtual bool start() = 0;
    /// @brief 停止SIP协议栈
    /// @return 
    virtual bool stop() = 0;
    /// @brief SIP协议栈线程体
    /// @param interval 休眠/超时时间，单位：ms
    virtual bool threadFunc(int32_t interval) = 0;
    /// @brief 获取SIP用户ID
    /// @return 
    virtual const char* getSipUser() = 0;
    /// @brief 创建注册请求消息
    /// @param req 
    /// @return 
    virtual bool makeRegistration(SipUserMessage& req) = 0;
    /// @brief  发送注册请求
    /// @param req 
    /// @return 
    virtual bool sendRegistration(const SipUserMessage& req) = 0;
    /// @brief 发送MANSCDP命令
    /// @param cmd 心跳状态、报警事件通知、查询应答、控制应答等
    /// @return 
    virtual bool sendMANSCDP(const XMLDocument& cmd) = 0;
    /// @brief 创建INVITE回复
    /// @param req INVITE请求消息
    /// @param res INVITE回复消息
    /// @param code SIP状态码
    /// @return 
    virtual bool makeSessionResponse(const SipUserMessage& req, SipUserMessage& res, int32_t code) = 0;
    /// @brief 发送INVITE回复
    /// @param id session标识
    /// @param res 
    /// @return 
    virtual bool sendSessionResponse(const SessionIdentifier& id, const SipUserMessage& res) = 0;
    /// @brief 发送会话内消息
    /// @param id session标识
    /// @param cmd 回放、下载文件结束
    /// @return 
    virtual bool sendSessionMessage(const SessionIdentifier& id, const XMLDocument& notify) = 0;
    /// @brief 发送MANSRTSP命令
    /// @param id session标识
    /// @param cmd 回放控制命令应答
    /// @return 
    virtual bool sendMANSRTSP(const SessionIdentifier& id, const MANSRTSP::Message& res) = 0;

protected:
    /// @brief 向UA投递注册回复
    /// @param res 回复消息
    /// @return 
    bool postRegistrationResponse(const SipUserMessage& res);
    /// @brief 向UA投递MANSCDP请求命令
    /// @param req 请求命令
    /// @return 
    bool postMANSCDPRequest(const XMLDocument& req);
    /// @brief 向UA投递MANSCDP命令结果
    /// @param code SIP状态码
    /// @param cmd 
    /// @return 
    bool postMANSCDPResult(int32_t code, const XMLDocument& cmd);
    /// @brief 向UA投递会话请求
    /// @param id session标识
    /// @param req 
    /// @return 
    bool postSessionRequest(const SessionIdentifier& id, const SipUserMessage& req);
    /// @brief 向UA投递MANSRTSP请求消息
    /// @param req 请求消息
    /// @return 
    bool postMANSRTSPRequest(const SipUserMessage& req);

public:
    static std::shared_ptr<SipUserAgent> create(UA *user, const ClientInfo& client, const ServerInfo& server);
};

#endif