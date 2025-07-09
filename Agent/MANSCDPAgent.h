#ifndef MANSCDP_AGENT_H
#define MANSCDP_AGENT_H

#include <memory>
#include "Agent.h"
#include "Process/9.3Control.h"
#include "Process/9.5Query.h"

class MANSCDPAgent : public Agent
{
    friend class UA;

private:
    std::shared_ptr<Control> control;
    std::shared_ptr<Query> query;
    std::shared_ptr<const SipMessageApp> lastReqMessage;

private:
    /* 
     * MANSCDP规定的所有请求命令
     * Control、Query、Notify
     * 遍历所有请求命令，匹配成功则调用其dispach方法处理
     */
    std::vector<std::shared_ptr<CmdTypeRequest>> requests;

public:
    MANSCDPAgent(UA *ua);
    ~MANSCDPAgent();

public:
    bool match(const std::string& method, const std::string& contentType);
    bool match(const std::string& callID);
    bool agent(const SipMessageApp& message);

public:
    bool sendResponse(const XMLDocument& xmldocRes) const;
};

#endif