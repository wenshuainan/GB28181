#include "9.1Registration.h"

class DevRegistration : public Registration
{
public:
    DevRegistration() : Registration() {}
    ~DevRegistration() {}

public:
    void promptState(State state, int sipCode, const std::string& sipReasonPhrase);
};