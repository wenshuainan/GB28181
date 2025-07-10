#ifndef REGISTRATION_AGENT_H
#define REGISTRATION_AGENT_H

#include <memory>
#include "Agent.h"
#include "9.1Registration.h"

class RegistrationAgent : public Agent
{
    friend class UA;

private:
    std::shared_ptr<Registration> registration;
    std::string GBVerName;
    std::string GBVerValue;

public:
    RegistrationAgent(UA *ua);
    ~RegistrationAgent();

public:
    bool match(const std::string& method, const std::string& contentType);
    bool agent(const SipMessageApp& message);

private:
    void changeDevState(int code, const std::string& reasonPhrase);

public:
    bool start();
    bool stop();
};

#endif