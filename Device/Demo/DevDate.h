#ifndef DEV_DATE_H
#define DEV_DATE_H

#include "Interface/9.10Date.h"

class DevDate : public Date
{
public:
    virtual bool setTime(time_t t);
};

#endif