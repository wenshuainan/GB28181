#ifndef CONTROL_H
#define CONTROL_H

#include "A.2.3Control.h"
#include "A.2.6Response.h"

class Control
{
public:
    Control();
    virtual ~Control();

public:
    virtual bool process(const PTZCmdRequest::Request& req, DeviceControlResponse::Response& res) = 0;
};

#endif