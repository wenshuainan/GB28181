#ifndef DEV_STATUS_H
#define DEV_STATUS_H

#include "9.6Status.h"

class DevStatus : public Status
{
public:
    DevStatus();
    ~DevStatus();

public:
    virtual void getStatus(KeepAliveNotify::Request& req);
    virtual void onKeepaliveSuccess();
    virtual void onKeepaliveTimeout(int32_t timeoutCount);
};

#endif