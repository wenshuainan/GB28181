#include <thread>
#include <unistd.h>
#include "RegistrationAgent.h"

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

bool RegistrationAgent::agent(const Header& header, const std::string& content)
{
    return false;
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

bool RegistrationAgent::start()
{
    bThreadRun = true;
    std::thread t(&RegistrationAgent::stateProc, this);
    t.detach();
    
    return true;
}

bool RegistrationAgent::stop()
{
    bThreadRun = false;
    return true;
}