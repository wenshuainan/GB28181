#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

#include "9.3Control.h"

class ControlInterface : public Control
{
public:
    ControlInterface();
    ~ControlInterface();

public:
    bool process(const RequestPTZCmd::Request& req, ResponseDeviceControl::Response& res);
    bool process(const RequestTeleBoot::Request& req, ResponseDeviceControl::Response& res);
    bool process(const RequestRecord::Request& req, ResponseDeviceControl::Response& res);
};

#endif