#ifndef DEV_CONTROL_H
#define DEV_CONTROL_H

#include "9.3Control.h"

class DevControl : public Control
{
public:
    DevControl();
    ~DevControl();

public:
    bool process(const PTZCmdRequest::Request& req, DeviceControlResponse::Response& res);
    bool process(const TeleBootRequest::Request& req, DeviceControlResponse::Response& res);
    bool process(const RecordRequest::Request& req, DeviceControlResponse::Response& res);
};

#endif