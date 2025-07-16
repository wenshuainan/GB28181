#include <iostream>
#include "DevStatus.h"

DevStatus::DevStatus()
{}

DevStatus::~DevStatus()
{}

void DevStatus::getStatus(KeepAliveNotify::Request& req)
{
    req.CmdType = "Keepalive";
    req.SN = "22";
    req.DeviceID = "34020000001110000001";
    req.Status = "OK";
}

void DevStatus::onKeepaliveSuccess()
{}

void DevStatus::onKeepaliveTimeout(int32_t count)
{
    std::cout << "Keepalive timeout: " << count << std::endl;
}