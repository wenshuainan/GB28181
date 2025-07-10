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

bool RegistrationAgent::agent(const SipMessageApp& message)
{
    changeDevState(message.getCode(), message.getReasonPhrase());
    return true;
}

bool RegistrationAgent::start()
{
    auto sip = m_ua->getSip();

    SipMessageApp message;
    sip->genReqMessage(message, "REGISTER");

    /* 添加GB版本号扩展头域（附录I） */
    message.addField(GBVerName, GBVerValue);

    if (sip->send(message))
    {
        outCallID = message.getCallID();
        DEBUG_LOG << "Call-ID: " << outCallID << std::endl;

        return true;
    }
    else
    {
        outCallID.clear();
        return false;
    }
}

bool RegistrationAgent::stop()
{
    auto sip = m_ua->getSip();

    SipMessageApp message;
    sip->genReqMessage(message, "REGISTER");

    message.addField("Expires", "0");

    if (sip->send(message))
    {
        outCallID = message.getCallID();
    }

    return true;
}

void RegistrationAgent::changeDevState(int code, const std::string& reasonPhrase)
{
    Registration::State newState;
    Registration::State oldState = registration->getState();

    switch (code)
    {
    case 200:
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
        DEBUG_LOG << "Unkown code: " << code << std::endl;
        newState = Registration::REGISTER_FAILED;
        break;
    }

    registration->processState(newState, reasonPhrase);
}
