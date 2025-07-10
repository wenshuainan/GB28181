#ifndef AGENT_H
#define AGENT_H

#include <string>
#include <string.h>
#include "SipAdapter.h"

class UA;

class Agent
{
protected:
    UA *m_ua;

public:
    Agent(UA *ua) : m_ua(ua) {}
    virtual ~Agent() {}

protected:
    bool strCaseCmp(const std::string& str1, const std::string& str2) const
    {
        return str1.length() == str2.length()
                && strcasecmp(str1.c_str(), str2.c_str()) == 0;
    }

public:
    /* 匹配Request，UA先遍历所有agent，调用match，匹配成功则调用Agent的agent方法处理SIP数据 */
    virtual bool match(const std::string& method, const std::string& contentType) = 0;

    /* UA调用agent将raw数据传给Agent，Agent解析成自己的格式后处理 */
    virtual bool agent(const SipMessageApp& message) = 0;
};

#endif