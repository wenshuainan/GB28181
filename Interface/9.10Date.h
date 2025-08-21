#ifndef DATE_INTERFACE_H
#define DATE_INTERFACE_H

#include <sys/time.h>

class Date
{
public:
    virtual bool setTime(time_t t) = 0;
};

#endif