#ifndef CONTROL_H
#define CONTROL_H

#include "A.2.3Control.h"
#include "A.2.6Response.h"

class Control
{
public:
    Control() {}
    virtual ~Control() {}

public:
    virtual bool process(const RequestPTZCmd::Request& req, ResponseDeviceControl::Response& res) = 0;
    virtual bool process(const RequestTeleBoot::Request& req, ResponseDeviceControl::Response& res) = 0;
    virtual bool process(const RequestRecord::Request& req, ResponseDeviceControl::Response& res) = 0;
};

#endif