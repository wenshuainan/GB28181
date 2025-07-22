#ifndef REGISTRATION_AGENT_H
#define REGISTRATION_AGENT_H

#include <memory>
#include "Agent.h"
#include "Interface/9.1Registration.h"

class RegistrationAgent : public Agent
{
private:
    std::shared_ptr<Registration> m_devRegistration;
    std::string m_GBVerName;
    std::string m_GBVerValue;
    bool m_bStarted;

public:
    RegistrationAgent(UA *ua);
    ~RegistrationAgent();

public:
    bool match(const std::string& method, const std::string& contentType);
    bool agent(const SipUserMessage& message);

private:
    void changeDevState(int code, const std::string& reasonPhrase);

public:
    bool start();
    bool stop();
};

#endif