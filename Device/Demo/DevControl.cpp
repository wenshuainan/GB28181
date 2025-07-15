#include <iostream>
#include "DevControl.h"

DevControl::DevControl()
{}

DevControl::~DevControl()
{}

bool DevControl::handle(const PTZCmdControl::Request& req)
{
    std::cout << "DevControl::handle" << std::endl;
    std::cout << "CMD:" << req.CmdType.getValue() << std::endl;
    std::cout << "SN:" << req.SN.getValue() << std::endl;
    std::cout << "DeviceID:" << req.DeviceID.getValue() << std::endl;
    std::cout << "PTZCmd:" << req.PTZCmd.getValue() << std::endl;
    std::cout << "PresetName:" << req.PTZCmdParams.PresetName.getValue() << std::endl;
    std::cout << "CruiseTrackName:" << req.PTZCmdParams.CruiseTrackName.getValue() << std::endl;

    return true;
}

bool DevControl::handle(const TeleBootControl::Request& req)
{
    std::cout << "DevControl::handle" << std::endl;
    std::cout << "CMD:" << req.CmdType.getValue() << std::endl;
    
    return false;
}

bool DevControl::handle(const RecordControl::Request& req, DeviceControlResponse::Response& res)
{
    std::cout << "DevControl::handle" << std::endl;
    std::cout << "CMD:" << req.CmdType.getValue() << std::endl;

    res.CmdType = req.CmdType;

    return false;
}