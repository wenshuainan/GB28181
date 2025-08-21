#ifndef DEV_STATUS_H
#define DEV_STATUS_H

#include "Interface/9.6Status.h"

class DevStatus : public Status
{
public:
    DevStatus(MANSCDPAgent *agent);
    ~DevStatus();

public:
    virtual bool getStatus(int32_t ch);
    virtual void onKeepaliveSuccess();
    virtual void onKeepaliveTimeout(int32_t count);
};

#endif