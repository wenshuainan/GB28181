#include <thread>
#include <unistd.h>
#include "UA.h"
#include "MANSCDPAgent.h"
#include "RegistrationAgent.h"
#include "MediaAgent.h"
#include "DevControl.h"
#include "DevRegistration.h"

UA::UA()
{}

UA::~UA()
{}

void UA::threadProc()
{
    while (bThreadRun)
    {
        SipGenericMessage message;
        if (sipUA->recv(message))
        {
            auto type = message.getType();
            if (type == SipGenericMessage::Request)
            {
                const std::string method = message.getMethod();
                const std::string contentType = message.getFieldValue("Content-Type");

                for (auto agent : agents)
                {
                    if (agent->match(method, contentType))
                    {
                        agent->agent(message);
                        break;
                    }
                }
            }
            else if (type == SipGenericMessage::Response)
            {
                const std::string callID = message.getFieldValue("Call-ID");

                for (auto agent : agents)
                {
                    if (agent->match(callID))
                    {
                        agent->agent(message);
                        break;
                    }
                }
            }
            else
            {
            }
        }

        sleep(10);
    }
}

bool UA::start(const Info& info)
{
    sipUA = SipUserAgent::create(info.sipInfo);
    if (sipUA == nullptr)
    {
        return false;
    }

    RegistrationAgent *registrationAgent = new RegistrationAgent(this);
    MANSCDPAgent *manscdpAgent = new MANSCDPAgent(this);
    MediaAgent *mediaAgent = new MediaAgent(this);

    if (manscdpAgent != nullptr)
    {
        manscdpAgent->control = new DevControl();
    }
    
    if (registrationAgent != nullptr)
    {
        registrationAgent->registration = new DevRegistration();
    }

    agents.push_back(registrationAgent);
    agents.push_back(manscdpAgent);
    agents.push_back(mediaAgent);

    bThreadRun = true;
    std::thread t(&UA::threadProc, this);
    t.detach();

    registrationAgent->start();

    return true;
}

bool UA::stop()
{
    bThreadRun = false;
    return true;

    for (auto agent : agents)
    {
        delete agent;
    }
}
