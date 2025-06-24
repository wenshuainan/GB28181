#include <thread>
#include <unistd.h>
#include "RegistrationAgent.h"
#include "UA.h"

RegistrationAgent::RegistrationAgent(UA *ua) : Agent(ua)
{
    bThreadRun = false;
    GBVerName = "X-GB-Ver";
    GBVerValue = "3.0";
}

RegistrationAgent::~RegistrationAgent()
{}

bool RegistrationAgent::match(const std::string& method, const std::string& contentType)
{
    return method == "REGISTER";
}

bool RegistrationAgent::match(const std::string& callID)
{
    return m_callID == callID;
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

    auto sipUA = m_ua->getSipUA();

    SipGenericMessage message;
    sipUA->genReqMessage(message, "REGISTER", "");
    message.print();
    // message.addField("Expires", "3600");
    // message.addField("Authorization", "Basic <KEY>");
    // message.addField("X-GB-Ver", "3.0");
    message.addField(GBVerName, GBVerValue);
    message.print();

    // printf(">>>>>> callid=[%s]\n", message.getFieldValue("Call-ID").c_str());
    message.getFieldValue("Call-ID");

    /* if (sipUA->send(message))
    {
        m_callID = message.getFieldValue("Call-ID");
        return true;
    }
    else */
    {
        return false;
    }
}

bool RegistrationAgent::stop()
{
    auto sipUA = m_ua->getSipUA();

    SipGenericMessage message;
    sipUA->genReqMessage(message, "REGISTER", "");
    message.addField("Expires", "0");
    message.addField("Authorization", "Basic <KEY>");

    sipUA->send(message);

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
