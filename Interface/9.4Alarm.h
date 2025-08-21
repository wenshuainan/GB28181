#ifndef ALARM_INTERFACE_H
#define ALARM_INTERFACE_H

#include <stdint.h>
#include "MANSCDP/A.2.5Notify.h"

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
    virtual bool notifyAlarm(int32_t ch, AlarmNotify &notify) { return false; }
};

#endif