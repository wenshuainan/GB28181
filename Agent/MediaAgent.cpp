#include "MediaAgent.h"

MediaAgent::MediaAgent(UA *ua) : Agent(ua)
{
    play = nullptr;
}

MediaAgent::~MediaAgent()
{}

bool MediaAgent::start()
{
    return true;
}

bool MediaAgent::stop()
{
    return true;
}

bool MediaAgent::match(const std::string& method, const std::string& contentType)
{
    return contentType == "application/sdp";
}

bool MediaAgent::match(const std::string& callID)
{
    return outCallID == callID;
}

bool MediaAgent::agent(const SipMessageApp& message)
{
    return false;
}