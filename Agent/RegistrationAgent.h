#ifndef REGISTRATION_AGENT_H
#define REGISTRATION_AGENT_H

#include "Agent.h"
#include "9.1Registration.h"

class RegistrationAgent : public Agent
{
    friend class UA;

private:
    Registration *registration;
    int lastRegistered;
    int interval;
    int expire;
    bool bThreadRun;

public:
    RegistrationAgent(UA *ua);
    ~RegistrationAgent();

public:
    bool match(const std::string& method, const std::string& contentType);
    bool match(const std::string& callID);
    bool agent(const Header& header, const std::string& content);
    bool start();
    bool stop();

private:
    void stateProc();
};

#endif