#ifndef AGENT_H
#define AGENT_H

#include <string>

class UA;

class Agent
{
protected:
    UA *m_ua;
    std::string m_methodType;
    std::string m_contentType;

public:
    Agent(UA *ua) : m_ua(ua) {}
    virtual ~Agent() {}

public:
    /* UA先遍历所有agent，调用match，匹配成功则调用Agent的agent方法处理SIP数据 */
    virtual bool match(const std::string& methodType, const std::string& contentType) = 0;

    /* UA调用agent将raw数据传给Agent，Agent解析成自己的格式后处理 */
    virtual bool agent(const std::string& content) = 0;
};

#endif