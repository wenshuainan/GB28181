#ifndef ALARM_INTERFACE_H
#define ALARM_INTERFACE_H

#include <stdint.h>

class Alarm
{
public:
    Alarm() {}
    virtual ~Alarm() {}

public:
    virtual bool postVideoAlarm(int32_t ch, int32_t type) = 0;
};

#endif