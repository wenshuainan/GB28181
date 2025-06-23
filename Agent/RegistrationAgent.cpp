#include <thread>
#include <unistd.h>
#include "RegistrationAgent.h"
#include "UA.h"

RegistrationAgent::RegistrationAgent(UA *ua) : Agent(ua)
{
    bThreadRun = false;
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

bool RegistrationAgent::agent(const Header& header, const std::string& content)
{
    return false;
}

bool RegistrationAgent::start()
{
    bThreadRun = true;
    std::thread t(&RegistrationAgent::stateProc, this);
    t.detach();

    Header header;
    auto adapter = m_ua->getAdapter();

    adapter->genReqHeader("REGISTER", "", header);
    header.addField("Expires", "3600");
    header.addField("Authorization", "Basic <KEY>");
    header.addField("X-GB-Ver", "3.0");

    if (adapter->send(header, ""))
    {
        m_callID = header.getField("Call-ID").getValue();
        return true;
    }
    else
    {
        return false;
    }
}

bool RegistrationAgent::stop()
{
    Header header;
    auto adapter = m_ua->getAdapter();

    adapter->genReqHeader("REGISTER", "", header);
    header.addField("Expires", "0");
    header.addField("Authorization", "Basic <KEY>");

    adapter->send(header, "");

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
