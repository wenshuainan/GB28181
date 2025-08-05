#ifndef ALARM_INTERFACE_H
#define ALARM_INTERFACE_H

#include <stdint.h>

class MANSCDPAgent;

class Alarm
{
protected:
    MANSCDPAgent *m_agent;
    
public:
    Alarm(MANSCDPAgent *agent) : m_agent(agent) {}
    virtual ~Alarm() {}

public:
    virtual bool setGuard(int32_t ch) = 0;
    virtual bool resetGuard(int32_t ch) = 0;
    virtual bool postVideoAlarm(int32_t ch, int32_t type) = 0;
};

#endif