#ifndef REGISTRATION_PROCESS_H
#define REGISTRATION_PROCESS_H

#include "A.2.1GlobalTypes.h"

class Registration
{
public:
    enum Status
    {
        OFFLINE,
        ONLINE,
    };

    enum State
    {
        UNREGISTERED,       // 未注册（已注销）
        REGISTERING,        // 注册中
        REGISTER_FAILED,    // 注册失败
        REGISTERED,         // 注册成功
        REGISTER_EXPIRED,   // 注册过期
        UNREGISTERING,      // 注销中
    };

protected:
    Status status;
    State state;

public:
    Registration() : status(OFFLINE), state(UNREGISTERED) {}
    ~Registration() {}

    virtual void promptState(State state, int sipCode, const std::string& sipReasonPhrase) = 0;

    virtual State getState() { return state; }

    virtual Status getStatus() { return status; }
};

#endif