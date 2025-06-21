#include "RegistrationAgent.h"

RegistrationAgent::RegistrationAgent(UA *ua) : Agent(ua)
{}

RegistrationAgent::~RegistrationAgent()
{}

bool RegistrationAgent::match(const std::string& methodType, const std::string& contentType)
{
    return methodType == "REGISTER";
}

bool RegistrationAgent::agent(const std::string& content)
{
    return false;
}