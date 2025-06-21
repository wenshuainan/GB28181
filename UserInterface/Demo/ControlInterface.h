#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

#include "9.3Control.h"

class ControlInterface : public Control
{
public:
    ControlInterface();
    ~ControlInterface();

public:
    bool process(const PTZCmdRequest::Request& req, DeviceControlResponse::Response& res);
    bool process(const TeleBootRequest::Request& req, DeviceControlResponse::Response& res);
    bool process(const RecordRequest::Request& req, DeviceControlResponse::Response& res);
};

#endif