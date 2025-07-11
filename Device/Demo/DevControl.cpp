#include <iostream>
#include "DevControl.h"

DevControl::DevControl()
{}

DevControl::~DevControl()
{}

bool DevControl::process(const PTZCmdControl::Request& req)
{
    std::cout << "DevControl::process" << std::endl;
    std::cout << "CMD:" << req.CmdType.getValue() << std::endl;
    std::cout << "SN:" << req.SN.getValue() << std::endl;
    std::cout << "DeviceID:" << req.DeviceID.getValue() << std::endl;
    std::cout << "PTZCmd:" << req.PTZCmd.getValue() << std::endl;
    std::cout << "PresetName:" << req.PTZCmdParams.PresetName.getValue() << std::endl;
    std::cout << "CruiseTrackName:" << req.PTZCmdParams.CruiseTrackName.getValue() << std::endl;

    return true;
}

bool DevControl::process(const TeleBootControl::Request& req)
{
    return false;
}

bool DevControl::process(const RecordControl::Request& req, DeviceControlResponse::Response& res)
{
    return false;
}