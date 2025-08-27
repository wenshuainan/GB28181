#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

#include <memory>
#include "MANSCDP/A.2.3Control.h"

class PTZCommand;
class FICommand;

class Control
{
public:
    virtual bool controlPTZ(const PTZCommand *cmd) = 0;
    virtual bool controlFI(const FICommand *cmd) = 0;
    virtual bool reboot() = 0;
    virtual bool startRecord(int32_t stream = 0) = 0;
    virtual bool stopRecord(int32_t stream = 0) = 0;
    virtual bool setGuard() = 0;
    virtual bool resetGuard() = 0;
    virtual bool resetAlarm(int32_t method = 0, int32_t type = 0) = 0;
};

#endif