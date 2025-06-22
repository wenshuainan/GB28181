#include <thread>
#include <unistd.h>
#include "UA.h"
#include "MANSCDPAgent.h"
#include "RegistrationAgent.h"
#include "MediaAgent.h"
#include "DevControl.h"

UA::UA()
{
    RegistrationAgent *registrationAgent = new RegistrationAgent(this);
    MANSCDPAgent *manscdpAgent = new MANSCDPAgent(this);
    MediaAgent *mediaAgent = new MediaAgent(this);

    if (manscdpAgent != nullptr)
    {
        manscdpAgent->control = new DevControl();
    }

    agents.push_back(registrationAgent);
    agents.push_back(manscdpAgent);
    agents.push_back(mediaAgent);
}

UA::~UA()
{
    for (auto agent : agents)
    {
        delete agent;
    }
}

void UA::threadProc()
{
    adapter = SIPAdapter::create();

    while (bThreadRun)
    {
        Header header;
        std::string body;
        if (adapter->recv(header, body))
        {
            Header::Type type = adapter->getType();
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

bool UA::start()
{
    bThreadRun = true;
    std::thread t(&UA::threadProc, this);
    t.detach();
    return true;
}

bool UA::stop()
{
    bThreadRun = false;
    return true;
}
