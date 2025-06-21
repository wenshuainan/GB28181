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
    bool match(const std::string& methodType, const std::string& contentType);
    bool agent(const std::string& content);
};

#endif