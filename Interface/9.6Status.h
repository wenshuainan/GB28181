#ifndef STATUS_INTERFACE_H
#define STATUS_INTERFACE_H

#include "MANSCDP/A.2.5Notify.h"
#include "Agent/MANSCDPAgent.h"

class Status
{
private:
    int32_t m_timeoutCount;
    MANSCDPAgent *m_agent;

public:
    Status(MANSCDPAgent *agent)
        : m_timeoutCount(0)
        , m_agent(agent)
    {}
    virtual ~Status() {}

public:
    bool sendKeepalive()
    {
        std::shared_ptr<KeepaliveNotify> keepalive = m_agent->createCmdMessage<KeepaliveNotify>(m_agent, this);
        if (keepalive && keepalive->notify(keepalive))
        {
            addSentCount();
            return true;
        }
        return false;
    }

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
    virtual bool getStatus(int32_t ch) = 0; //定时获取当前状态
    virtual void onKeepaliveSuccess() = 0;
    virtual void onKeepaliveTimeout(int32_t count) = 0;
};

#endif