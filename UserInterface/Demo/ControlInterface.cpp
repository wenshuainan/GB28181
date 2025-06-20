#include "ControlInterface.h"

ControlInterface::ControlInterface()
{}

ControlInterface::~ControlInterface()
{}

bool ControlInterface::process(const RequestPTZCmd::Request& req, ResponseDeviceControl::Response& res)
{
    return false;
}

bool ControlInterface::process(const RequestTeleBoot::Request& req, ResponseDeviceControl::Response& res)
{
    return false;
}

bool ControlInterface::process(const RequestRecord::Request& req, ResponseDeviceControl::Response& res)
{
    return false;
}