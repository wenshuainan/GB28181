#include <sys/time.h>
#include "DevAlarm.h"
#include "MANSCDP/A.2.5Notify.h"
#include "UA.h"

DevAlarm::DevAlarm(MANSCDPAgent *agent)
    : m_agent(agent)
{}

DevAlarm::~DevAlarm()
{}

bool DevAlarm::postVideoAlarm(int32_t ch, int32_t type)
{
    AlarmNotify alarm(m_agent);
    return alarm.notify(ch, 5, type, time(NULL));
}