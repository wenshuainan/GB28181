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
    bool start();
    bool stop();
    bool match(const std::string& method, const std::string& contentType);
    bool match(const std::string& callID);
    bool agent(const SipMessageApp& message);
};

#endif