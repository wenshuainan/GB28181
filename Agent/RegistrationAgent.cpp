#include <iostream>
#include <unistd.h>
#include "UA.h"
#include "RegistrationAgent.h"
#include "DevRegistration.h"

RegistrationAgent::RegistrationAgent(UA *ua)
    : Agent(ua)
{
    m_devRegistration = std::make_shared<DevRegistration>();
    m_GBVerName = "X-GB-Ver";
    m_GBVerValue = "3.0";
    m_bStarted = false;
}

RegistrationAgent::~RegistrationAgent()
{}

bool RegistrationAgent::match(const std::string& method, const std::string& contentType)
{
    std::cout << "method=" << method << "content type=" << contentType << std::endl;
    return false; //只作为Register Client，不处理Register请求
}

bool RegistrationAgent::agent(const SipUserMessage& message)
{
    changeDevState(message.getCode(), message.getReasonPhrase());
    return true;
}

bool RegistrationAgent::start()
{
    if (m_bStarted)
    {
        return false;
    }
    else
    {
        m_bStarted = true;
    }

    SipUserMessage message;
    auto sip = m_ua->m_sip;
    sip->makeRegistrationRequest(message);

    /* 添加GB版本号扩展头域（附录I） */
    message.addExtensionField(m_GBVerName, m_GBVerValue);

    return sip->sendRegistration(message);
}

bool RegistrationAgent::stop()
{
    if (!m_bStarted)
    {
        return false;
    }
    else
    {
        m_bStarted = false;
    }

    SipUserMessage message;
    auto sip = m_ua->m_sip;
    sip->makeRegistrationRequest(message);

    message.setExpires(0);

    return sip->sendRegistration(message);
}

void RegistrationAgent::changeDevState(int code, const std::string& reasonPhrase)
{
    Registration::State newState;
    Registration::State oldState = m_devRegistration->getState();

    switch (code)
    {
    case 200:
        m_ua->setStatus(true); //注册成功，设置在线状态

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

    m_devRegistration->onState(newState, reasonPhrase);
}
