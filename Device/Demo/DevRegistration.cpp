#include <iostream>
#include "DevRegistration.h"

void DevRegistration::onState(State state, const std::string& sipReasonPhrase)
{
    printf("DevRegistration state %d(%s)\n", state, sipReasonPhrase.c_str());
}