#ifndef AGENT_H
#define AGENT_H

#include <string>
#include "SipAdapter.h"

class UA;

class Agent
{
protected:
    UA *m_ua;
    std::string outCallID; // 外发请求的CallID，用于匹配Response

public:
    Agent(UA *ua) : m_ua(ua) {}
    virtual ~Agent() {}

public:
    virtual bool start() = 0;
    virtual bool stop() = 0;
    
    /* 匹配Request，UA先遍历所有agent，调用match，匹配成功则调用Agent的agent方法处理SIP数据 */
    virtual bool match(const std::string& method, const std::string& contentType) = 0;

    /* 匹配Response，UA先遍历所有agent，调用match，匹配成功则调用Agent的agent方法处理SIP数据 */
    virtual bool match(const std::string& callID) = 0;

    /* UA调用agent将raw数据传给Agent，Agent解析成自己的格式后处理 */
    virtual bool agent(const SipMessageApp& message) = 0;
};

#endif