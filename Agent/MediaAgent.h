#ifndef MEDIA_AGENT_H
#define MEDIA_AGENT_H

#include "Agent.h"
#include "9.2Play.h"

class MediaAgent : public Agent
{
    friend class UA;

private:
    Play *play;

public:
    MediaAgent(UA *ua);
    ~MediaAgent();

public:
    bool match(std::string& methodType, std::string& contentType);
    bool agent(std::string& content);
};

#endif