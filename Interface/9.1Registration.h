#ifndef REGISTRATION_INTERFACE_H
#define REGISTRATION_INTERFACE_H

#include "MANSCDP/A.2.1GlobalTypes.h"

class RegistrationAgent;

class Registration
{
    friend RegistrationAgent;
    
public:
    enum State
    {
        UNREGISTERED,       // 未注册（已注销）
        REGISTER_FAILED,    // 注册失败
        REGISTERED,         // 注册成功
        REGISTER_EXPIRED,   // 注册过期
    };

protected:
    State m_state;

public:
    Registration() : m_state(UNREGISTERED) {}
    ~Registration() {}

    virtual void onState(State state, const std::string& sipReasonPhrase) = 0;
    virtual State getState() { return m_state; }
};

#endif