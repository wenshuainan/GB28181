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
    bool agent(const XMLDocument& xmldocReq);

public:
    bool sendResponse200(const SipMessageApp& req) const; //无应答命令
    bool sendResponseCmd(const XMLDocument& xmldocRes) const; //有应答命令
    bool sendRequest(const XMLDocument& xmldocReq) const;
};

#endif