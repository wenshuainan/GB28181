#include <sys/time.h>
#include <iostream>
#include "DevAlarm.h"
#include "MANSCDP/A.2.5Notify.h"
#include "UA.h"

DevAlarm::DevAlarm(MANSCDPAgent *agent)
    : Alarm(agent)
{}

DevAlarm::~DevAlarm()
{}

bool DevAlarm::setGuard(int32_t ch)
{
    std::cout << "setGuard" << " channel=" << ch << std::endl;
    return true;
}

bool DevAlarm::resetGuard(int32_t ch)
{
    std::cout << "resetGuard" << " channel=" << ch << std::endl;
    return true;
}