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
        Header header;
        std::string body;
        if (adapter->recv(header, body))
        {
            auto type = header.getType();
            if (type == Header::Request)
            {
                const std::string method = header.getMethod();
                const std::string contentType = header.getField("Content-type").getValue();

                for (auto agent : agents)
                {
                    if (agent->match(method, contentType))
                    {
                        agent->agent(header, body);
                        break;
                    }
                }
            }
            else if (type == Header::Response)
            {
                const std::string callID = header.getField("Call-ID").getValue();

                for (auto agent : agents)
                {
                    if (agent->match(callID))
                    {
                        agent->agent(header, body);
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
    SIPAdapter::Info sipInfo;
    adapter = SIPAdapter::create(sipInfo);
    if (adapter == nullptr)
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
