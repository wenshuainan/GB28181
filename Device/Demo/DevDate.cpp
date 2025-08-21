#include <stdio.h>
#include "DevDate.h"

bool DevDate::setTime(time_t t)
{
    printf("DevDate set current time to %ld\n", t);
    return true;
}