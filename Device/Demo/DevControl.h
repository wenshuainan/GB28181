#ifndef DEV_CONTROL_H
#define DEV_CONTROL_H

#include "Interface/9.3Control.h"

class DevControl : public Control
{
public:
    DevControl();
    ~DevControl();

public:
    bool handle(const PTZCmdControl::Request& req);
    bool handle(const TeleBootControl::Request& req);
    bool handle(const RecordControl::Request& req, DeviceControlResponse::Response& res);
};

#endif