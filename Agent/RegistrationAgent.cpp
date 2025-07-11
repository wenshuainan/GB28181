#include <iostream>
#include <unistd.h>
#include "UA.h"
#include "RegistrationAgent.h"
#include "DevRegistration.h"

RegistrationAgent::RegistrationAgent(UA *ua) : Agent(ua)
{
    registration = std::make_shared<DevRegistration>();
    GBVerName = "X-GB-Ver";
    GBVerValue = "3.0";
}

RegistrationAgent::~RegistrationAgent()
{}

bool RegistrationAgent::match(const std::string& method, const std::string& contentType)
{
    return false; //只作为Register Client，不处理Register请求
}

bool RegistrationAgent::agent(const SipUserMessage& message)
{
    changeDevState(message.getCode(), message.getReasonPhrase());
    return true;
}

bool RegistrationAgent::start()
{
    auto sip = m_ua->getSip();

    SipUserMessage message;
    sip->makeRegistrationRequest(message);

    /* 添加GB版本号扩展头域（附录I） */
    message.addExtensionField(GBVerName, GBVerValue);

    return sip->sendRegistration(message);
}

bool RegistrationAgent::stop()
{
    auto sip = m_ua->getSip();

    SipUserMessage message;
    sip->makeRegistrationRequest(message);

    message.setExpires(0);

    return sip->sendRegistration(message);
}

void RegistrationAgent::changeDevState(int code, const std::string& reasonPhrase)
{
    Registration::State newState;
    Registration::State oldState = registration->getState();

    switch (code)
    {
    case 200:
        m_ua->setOnline(); //注册成功，设置在线状态

        if (oldState == Registration::REGISTERED)
        {
            newState = Registration::UNREGISTERED;
        }
        else
        {
            newState = Registration::REGISTERED;
        }
        break;
    case 403:
        newState = Registration::REGISTER_FAILED;
        break;
    
    default:
        newState = Registration::REGISTER_FAILED;
        break;
    }

    registration->processState(newState, reasonPhrase);
}
