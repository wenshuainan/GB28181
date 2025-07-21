#ifndef DEV_STATUS_H
#define DEV_STATUS_H

#include "Interface/9.6Status.h"

class DevStatus : public Status
{
public:
    DevStatus();
    ~DevStatus();

public:
    virtual void getStatus(KeepAliveNotify::Notify& notify);
    virtual void onKeepaliveSuccess();
    virtual void onKeepaliveTimeout(int32_t count);
};

#endif