#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

#include "MANSCDP/A.2.3Control.h"

class MANSCDPAgent;
class PTZCommand;
class FICommand;

class Control
{
protected:
    MANSCDPAgent *m_agent;

public:
    Control(MANSCDPAgent *agent) : m_agent(agent) {}
    virtual ~Control() {}

public:
    virtual bool controlPTZ(int32_t ch, const PTZCommand *cmd) = 0;
    virtual bool controlFI(int32_t ch, const FICommand *cmd) = 0;
    virtual bool reboot() = 0;
    virtual bool startRecord(int32_t ch, int32_t stream = 0) = 0;
    virtual bool stopRecord(int32_t ch, int32_t stream = 0) = 0;
    virtual bool setGuard(int32_t ch) = 0;
    virtual bool resetGuard(int32_t ch) = 0;
    virtual bool resetAlarm(int32_t ch, int32_t method = 0, int32_t type = 0) = 0;
};

#endif