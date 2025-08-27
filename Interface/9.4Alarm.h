#ifndef ALARM_INTERFACE_H
#define ALARM_INTERFACE_H

#include "MANSCDP/A.2.5Notify.h"

class Alarm
{
public:
    virtual bool notifyAlarm(AlarmNotify &notify) = 0;
};

#endif