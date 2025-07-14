#ifndef MANSCDP_AGENT_H
#define MANSCDP_AGENT_H

#include <memory>
#include "Agent.h"
#include "Process/9.3Control.h"
#include "Process/9.5Query.h"
#include "Process/9.6Status.h"

class MANSCDPAgent : public Agent
{
    friend class UA;

private:
    std::shared_ptr<Control> m_devControl;
    std::shared_ptr<Query> m_devQuery;
    std::shared_ptr<Status> m_devStatus;

private:
    /* 
     * MANSCDP规定的所有请求命令
     * Control、Query、Notify
     * 遍历所有请求命令，匹配成功则调用其dispach方法处理
     */
    std::vector<std::shared_ptr<CmdTypeRequest>> m_cmdRequests;

public:
    MANSCDPAgent(UA *ua);
    ~MANSCDPAgent();

public:
    bool match(const std::string& method, const std::string& contentType);
    bool agent(const SipUserMessage& message);
    bool agent(const XMLDocument &xmldocReq) const;
    int32_t getKeepaliveTimeoutCount() const;
    bool recvedKeepaliveResponse(int32_t code) const;

public:
    // bool sendResponse200(const SipUserMessage& req) const; //无应答命令
    bool sendResponseCmd(const XMLDocument& xmldocRes) const; //有应答命令
    // bool sendRequest(const XMLDocument& xmldocReq) const;
    bool sendKeepaliveRequest(const KeepAliveNotify::Request *notify = nullptr) const;
};

#endif