#ifndef DEV_CONTROL_PROCESS_H
#define DEV_CONTROL_PROCESS_H

#include "9.3Control.h"

class DevControl : public Control
{
public:
    DevControl();
    ~DevControl();

public:
    bool process(const PTZCmdControl::Request& req, DeviceControlResponse::Response& res);
    bool process(const TeleBootControl::Request& req, DeviceControlResponse::Response& res);
    bool process(const RecordControl::Request& req, DeviceControlResponse::Response& res);
};

#endif