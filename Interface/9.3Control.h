#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

#include "MANSCDP/A.2.3Control.h"

class Control
{
public:
    Control() {}
    virtual ~Control() {}

public:
    virtual bool handle(int32_t ch, const PTZCmdControl::Request& req) = 0;
    virtual bool handle(int32_t ch, const TeleBootControl::Request& req) = 0;
    virtual bool handle(int32_t ch, const RecordControl::Request& req) = 0;
};

#endif