#ifndef MANSCDP_AGENT_H
#define MANSCDP_AGENT_H

#include <memory>
#include "Agent.h"
#include "Process/9.3Control.h"

class MANSCDPAgent : public Agent
{
    friend class UA;

private:
    Control *control;
    std::shared_ptr<const Header> reqHeader;

private:
    /* 
     * MANSCDP规定的所有请求命令
     * Control、Query、Notify
     * 遍历所有请求命令，匹配成功则调用其dispach方法处理
     */
    std::vector<CmdTypeRequest *> requests;

public:
    MANSCDPAgent(UA *ua);
    ~MANSCDPAgent();

public:
    bool start();
    bool stop();
    bool match(const std::string& method, const std::string& contentType);
    bool match(const std::string& callID);
    bool agent(const Header& header, const std::string& content);

private:
    bool sendResponse(int code, const XMLDocument& doc);

public:
    /* 具体的请求类型处理方法 */
    bool agentControl(const PTZCmdRequest::Request& req);
    bool agentControl(const TeleBootRequest::Request& req);
};

#endif