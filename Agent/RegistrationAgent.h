#ifndef REGISTRATION_AGENT_H
#define REGISTRATION_AGENT_H

#include "Agent.h"
#include "9.1Registration.h"

class RegistrationAgent : public Agent
{
    friend class UA;

private:
    Registration *registration;

public:
    RegistrationAgent(UA *ua);
    ~RegistrationAgent();

public:
    bool match(std::string& methodType, std::string& contentType);
    bool agent(std::string& content);
};

#endif