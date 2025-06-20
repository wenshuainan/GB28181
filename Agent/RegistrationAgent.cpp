#include "RegistrationAgent.h"

RegistrationAgent::RegistrationAgent(UA *ua) : Agent(ua)
{}

RegistrationAgent::~RegistrationAgent()
{}

bool RegistrationAgent::match(std::string& methodType, std::string& contentType)
{
    return methodType == "REGISTER";
}

bool RegistrationAgent::agent(std::string& content)
{
    return false;
}