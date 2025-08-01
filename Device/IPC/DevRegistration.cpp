#include <iostream>
#include "DevRegistration.h"

void DevRegistration::onState(State state, const std::string& sipReasonPhrase)
{
    std::cout << "DevRegistration::onState" << std::endl;
    std::cout << "state: " << state << std::endl;
    std::cout << "sipReasonPhrase: " << sipReasonPhrase << std::endl;
}