#include "MediaAgent.h"

MediaAgent::MediaAgent(UA *ua) : Agent(ua)
{
    play = nullptr;
}

MediaAgent::~MediaAgent()
{}

bool MediaAgent::match(std::string& methodType, std::string& contentType)
{
    return contentType == "application/sdp";
}

bool MediaAgent::agent(std::string& content)
{
    return false;
}