#ifndef AGENT_H
#define AGENT_H

#include <string>

class UA;

class Agent
{
protected:
    UA *m_ua;

public:
    Agent(UA *ua) : m_ua(ua) {}
    virtual ~Agent() {}

public:
    /* UA先遍历所有agent，调用match，匹配成功则调用Agent的agent方法处理SIP数据 */
    virtual bool match(std::string& methodType, std::string& contentType) = 0;

    /* UA调用agent将raw数据传给Agent，Agent解析成自己的格式后处理 */
    virtual bool agent(std::string& content) = 0;
};

#endif