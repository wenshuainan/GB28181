#ifndef REGISTRATION_AGENT_H
#define REGISTRATION_AGENT_H

#include "Agent.h"
#include "9.1Registration.h"

class RegistrationAgent : public Agent
{
    friend class UA;

private:
    Registration *registration;
    std::string GBVerName;
    std::string GBVerValue;

public:
    RegistrationAgent(UA *ua);
    ~RegistrationAgent();

public:
    bool match(const std::string& method, const std::string& contentType);
    bool match(const std::string& callID);
    bool agent(const SipMessageApp& message);
    bool start();
    bool stop();

private:
    void changeDevState(int code, const std::string& reasonPhrase);
};

#endif