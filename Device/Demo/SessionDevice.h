#ifndef SESSION_DEVICE_H
#define SESSION_DEVICE_H

#include <memory>
#include "Device.h"
#include "Agent/SessionAgent.h"

class SessionDevice : public virtual Device
{
private:
    std::unique_ptr<SessionAgent> m_agent;

public:
    SessionDevice();
    virtual ~SessionDevice();

public:
    virtual SessionAgent* getSessionAgent() const;
};

#endif