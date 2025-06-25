#include <thread>
#include <unistd.h>
#include "RegistrationAgent.h"
#include "UA.h"

RegistrationAgent::RegistrationAgent(UA *ua) : Agent(ua)
{
    interval = 60;
    bThreadRun = false;
    GBVerName = "X-GB-Ver";
    GBVerValue = "3.0";
}

RegistrationAgent::~RegistrationAgent()
{}

bool RegistrationAgent::match(const std::string& method, const std::string& contentType)
{
    return false; //只做为Register Client，不处理Register请求
}

bool RegistrationAgent::match(const std::string& callID)
{
    return outCallID == callID;
}

bool RegistrationAgent::agent(const SipGenericMessage& message)
{
    return false;
}

bool RegistrationAgent::start()
{
    bThreadRun = true;
    std::thread t(&RegistrationAgent::stateProc, this);
    t.detach();

    auto sip = m_ua->getSip();

    SipGenericMessage message;
    sip->genReqMessage(message, "REGISTER");
    // message.print(); //debug

    /* 添加扩展GB版本号头域（附录I） */
    message.addField(GBVerName, GBVerValue);
    // message.print(); //debug

    if (sip->send(message))
    {
        outCallID = message.getFieldValue("Call-ID");
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

    SipGenericMessage message;
    sip->genReqMessage(message, "REGISTER");

    message.addField("Expires", "0");

    /* 添加扩展GB版本号头域（附录I） */
    message.addField(GBVerName, GBVerValue);
    message.print(); //debug

    if (sip->send(message))
    {
        outCallID = message.getFieldValue("Call-ID");
    }

    bThreadRun = false;
    return true;
}

void RegistrationAgent::stateProc()
{
    unsigned long long tick = 0;

    while (bThreadRun)
    {
        switch (registration->getState())
        {
        case Registration::UNREGISTERED:
            break;
        case Registration::REGISTERING:
            break;
        case Registration::REGISTER_FAILED:
            break;
        case Registration::REGISTERED:
            break;
        case Registration::REGISTER_EXPIRED:
            break;
        case Registration::UNREGISTERING:
            break;
        
        default:
            break;
        }

        sleep(1);
        tick++;
    }
}
