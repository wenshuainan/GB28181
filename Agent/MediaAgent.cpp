#include "MediaAgent.h"

MediaAgent::MediaAgent(UA *ua) : Agent(ua)
{
    play = nullptr;
}

MediaAgent::~MediaAgent()
{}

bool MediaAgent::match(const std::string& method, const std::string& contentType)
{
    return contentType == "application/sdp";
}

bool MediaAgent::match(const std::string& callID)
{
    return m_callID == callID;
}

bool MediaAgent::agent(const Header& header, const std::string& content)
{
    return false;
}