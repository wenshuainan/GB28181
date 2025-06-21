#include <iostream>
#include "ControlInterface.h"

ControlInterface::ControlInterface()
{}

ControlInterface::~ControlInterface()
{}

bool ControlInterface::process(const PTZCmdRequest::Request& req, DeviceControlResponse::Response& res)
{
    std::cout << "ControlInterface::process" << std::endl;
    std::cout << "CMD:" << req.CmdType.getValue() << std::endl;
    std::cout << "SN:" << req.SN.getValue() << std::endl;
    std::cout << "DeviceID:" << req.DeviceID.getValue() << std::endl;
    std::cout << "PTZCmd:" << req.PTZCmd.getValue() << std::endl;
    std::cout << "PresetName:" << req.PTZCmdParams.PresetName.getValue() << std::endl;
    std::cout << "CruiseTrackName:" << req.PTZCmdParams.CruiseTrackName.getValue() << std::endl;

    res.CmdType = req.CmdType;
    res.SN = req.SN;
    res.DeviceID = req.DeviceID;
    res.Result = resultType::OK;

    return true;
}

bool ControlInterface::process(const TeleBootRequest::Request& req, DeviceControlResponse::Response& res)
{
    return false;
}

bool ControlInterface::process(const RecordRequest::Request& req, DeviceControlResponse::Response& res)
{
    return false;
}