#ifndef DEV_CONTROL_H
#define DEV_CONTROL_H

#include "Interface/9.3Control.h"

class DevControl : public Control
{
public:
    DevControl();
    ~DevControl();

public:
    bool handle(int32_t ch, const PTZCmdControl::Request& req);
    bool handle(int32_t ch, const TeleBootControl::Request& req);
    bool handle(int32_t ch, const RecordControl::Request& req);
};

#endif