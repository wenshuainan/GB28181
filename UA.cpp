#include "UA.h"
#include "MANSCDPAgent.h"
#include "RegistrationAgent.h"
#include "MediaAgent.h"
#include "ControlInterface.h"

UA::UA()
{
    RegistrationAgent *registrationAgent = new RegistrationAgent(this);
    MANSCDPAgent *manscdpAgent = new MANSCDPAgent(this);
    MediaAgent *mediaAgent = new MediaAgent(this);

    if (manscdpAgent != nullptr)
    {
        manscdpAgent->control = new ControlInterface();
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

bool UA::read(std::string& data)
{
    std::string methodType;
    std::string contentType;

    for (auto agent : agents)
    {
        if (agent->match(methodType, contentType))
        {
            return agent->agent(data);
        }
    }

    return false;
}

bool UA::write(std::string& data)
{
    return false;
}
