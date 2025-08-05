#ifndef DEV_CONTROL_H
#define DEV_CONTROL_H

#include "Interface/9.3Control.h"

class DevControl : public Control
{
public:
    DevControl(MANSCDPAgent *agent);
    ~DevControl();

public:
    virtual bool controlPTZ(int32_t ch, const PTZCommand *cmd);
    virtual bool controlFI(int32_t ch, const FICommand *cmd);
    virtual bool reboot();
    virtual bool startRecord(int32_t ch, int32_t stream = 0);
    virtual bool stopRecord(int32_t ch, int32_t stream = 0);
    virtual bool setGuard(int32_t ch);
    virtual bool resetGuard(int32_t ch);
    virtual bool resetAlarm(int32_t ch, int32_t method = 0, int32_t type = 0);
};

#endif