#ifndef MANSCDP_AGENT_H
#define MANSCDP_AGENT_H

#include <vector>
#include <memory>
#include <list>
#include "Agent.h"
#include "MANSCDP/A.2.2CmdType.h"

class MANSCDPDevice;

class MANSCDPAgent : public Agent
{
private:
     /* MANSCDP规定的所有请求命令
     * Control、Query、Notify
     * 遍历所有请求命令，匹配成功则调用其dispach方法处理
     */
    std::vector<std::unique_ptr<CmdTypeRequest>> m_cmdRequests;

    /* 消息响应处理器链表
     * 所有需要处理响应结果的命令，例如心跳、多响应消息传输等
     */
    std::list<std::shared_ptr<MessageResultHandler>> m_handlers;

public:
    MANSCDPAgent(UA *ua);
    ~MANSCDPAgent();

public:
    template<typename T, typename A1, typename A2>
    std::shared_ptr<T> createCmdMessage(A1 a1, A2 a2)
    {
        return std::make_shared<T>(a1, a2);
    }

    bool match(const std::string& method, const std::string& contentType);
    bool agent(const SipUserMessage& message);
    bool agent(const XMLDocument &cmdReq);
    bool agent(int32_t code, const XMLDocument& cmd);
    bool sendCmd(const XMLDocument& cmd, std::shared_ptr<MessageResultHandler> handler = nullptr);
    MANSCDPDevice* getDevice(const std::string& id);
};

#endif