#include "9.1Registration.h"

class DevRegistration : public Registration
{
public:
    DevRegistration() : Registration() {}
    ~DevRegistration() {}

public:
    void processState(State state, const std::string& sipReasonPhrase);
};