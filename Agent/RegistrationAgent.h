#ifndef REGISTRATION_AGENT_H
#define REGISTRATION_AGENT_H

#include <memory>
#include "Agent.h"
#include "Interface/9.10Date.h"

class RegistrationAgent : public Agent
{
private:
    std::unique_ptr<Date> m_devDate; // 设备校时接口
    std::string m_GBVerName; // GB28181 附录I定义的协议版本标识
    std::string m_GBVerValue;
    bool m_bStarted;

public:
    RegistrationAgent(UA *ua);
    ~RegistrationAgent();

public:
    bool start();
    bool stop();
    bool match(const std::string& method, const std::string& contentType);
    bool agent(const SipUserMessage& message);
};

#endif