#ifndef STATUS_PROCESS_H
#define STATUS_PROCESS_H

#include <thread>
#include "A.2.5Notify.h"

class Status
{
public:
    Status() {}
    virtual ~Status() {}

public:
    virtual void getStatus(KeepAliveNotify::Request& status) = 0; //定时获取当前状态
    virtual void onKeepaliveSuccess() = 0;
    virtual void onKeepaliveTimeout(int32_t retryCount) = 0;
};

#endif