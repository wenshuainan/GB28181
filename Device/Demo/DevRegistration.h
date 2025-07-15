#ifndef DEV_REGISTRATION_H
#define DEV_REGISTRATION_H

#include "9.1Registration.h"

class DevRegistration : public Registration
{
public:
    DevRegistration() : Registration() {}
    ~DevRegistration() {}

public:
    void onState(State state, const std::string& sipReasonPhrase);
};

#endif