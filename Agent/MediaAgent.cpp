#include "MediaAgent.h"

MediaAgent::MediaAgent(UA *ua) : Agent(ua)
{
    play = nullptr;
}

MediaAgent::~MediaAgent()
{}

bool MediaAgent::match(const std::string& methodType, const std::string& contentType)
{
    return contentType == "application/sdp";
}

bool MediaAgent::agent(const std::string& content)
{
    return false;
}