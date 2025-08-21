#ifndef AGENT_H
#define AGENT_H

#include <string>
#include <string.h>
#include "SIP/Adapter/SipAdapter.h"

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
    /// @brief 匹配Request，UA遍历所有agent，调用match，匹配成功则调用Agent的agent方法处理请求消息
    /// @param method SIP method
    /// @param contentType SIP message header Content-Type
    /// @return 
    virtual bool match(const std::string& method, const std::string& contentType) = 0;
    /// @brief UA调用agent将请求消息传给Agent，Agent解析成自己的格式后处理
    /// @param message SIP message
    /// @return 
    virtual bool agent(const SipUserMessage& message) = 0;
};

#endif