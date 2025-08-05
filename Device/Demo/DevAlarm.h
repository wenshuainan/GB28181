#ifndef DEV_ALARM_H
#define DEV_ALARM_H

#include "Interface/9.4Alarm.h"

class MANSCDPAgent;

class DevAlarm : public Alarm
{
private:
    MANSCDPAgent *m_agent;
    
public:
    DevAlarm(MANSCDPAgent *agent);
    virtual ~DevAlarm();

public:
    virtual bool setGuard(int32_t ch);
    virtual bool resetGuard(int32_t ch);
    virtual bool postVideoAlarm(int32_t ch, int32_t type);
};

#endif