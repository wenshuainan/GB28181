#ifndef CONTROL_PROCESS_H
#define CONTROL_PROCESS_H

#include "A.2.3Control.h"
#include "A.2.6Response.h"

class Control
{
public:
    Control() {}
    virtual ~Control() {}

public:
    virtual bool process(const PTZCmdRequest::Request& req, DeviceControlResponse::Response& res) = 0;
    virtual bool process(const TeleBootRequest::Request& req, DeviceControlResponse::Response& res) = 0;
    virtual bool process(const RecordRequest::Request& req, DeviceControlResponse::Response& res) = 0;
};

#endif