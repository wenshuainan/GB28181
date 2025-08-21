#ifndef REGISTRATION_AGENT_H
#define REGISTRATION_AGENT_H

#include <memory>
#include "Agent.h"
#include "Interface/9.1Registration.h"
#include "Interface/9.10Date.h"

class RegistrationAgent : public Agent
{
    friend UA;

private:
    std::unique_ptr<Registration> m_devRegistration; // 设备注册接口
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
    void changeDevState(int code, const std::string& reasonPhrase);
};

#endif