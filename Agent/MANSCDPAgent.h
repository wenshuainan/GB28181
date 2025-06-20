#ifndef MANSCDP_AGENT_H
#define MANSCDP_AGENT_H

#include "Agent.h"
#include "Process/9.3Control.h"

class MANSCDPAgent : public Agent
{
    friend class UA;

private:
    Control *control;

private:
    /* 
     * MANSCDP规定的所有请求命令
     * Control、Query、Notify
     * 遍历所有请求命令，匹配成功则调用其dispach方法处理
     */
    std::vector<RequestCmdType *> requests;

public:
    MANSCDPAgent(UA *ua);
    ~MANSCDPAgent();

public:
    bool match(std::string& methodType, std::string& contentType);
    bool agent(std::string& content);

public:
    /* 具体的请求类型处理方法 */
    bool agentControl(const RequestPTZCmd::Request& req);
    bool agentControl(const RequestTeleBoot::Request& req);
};

#endif