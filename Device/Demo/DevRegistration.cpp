#include <iostream>
#include "DevRegistration.h"

void DevRegistration::processState(State state, const std::string& sipReasonPhrase)
{
    std::cout << "DevRegistration::processState" << std::endl;
    std::cout << "state: " << state << std::endl;
    std::cout << "sipReasonPhrase: " << sipReasonPhrase << std::endl;
}