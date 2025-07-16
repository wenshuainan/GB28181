#ifndef STATUS_INTERFACE_H
#define STATUS_INTERFACE_H

#include <thread>
#include "MANSCDP/A.2.5Notify.h"

class Status
{
private:
    int32_t m_timeoutCount;

public:
    Status() : m_timeoutCount(0) {}
    virtual ~Status() {}

public:
    void addSentCount()
    {
        int32_t count = m_timeoutCount;
        if (count > 0)
        {
            onKeepaliveTimeout(count);
        }
        m_timeoutCount++;
    }

    void addRecvedCount()
    {
        m_timeoutCount = 0;
        onKeepaliveSuccess();
    }

    int32_t getTimeoutCount()
    {
        return m_timeoutCount;
    }

public:
    virtual void getStatus(KeepAliveNotify::Request& req) = 0; //定时获取当前状态
    virtual void onKeepaliveSuccess() = 0;
    virtual void onKeepaliveTimeout(int32_t count) = 0;
};

#endif