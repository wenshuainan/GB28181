#include <iostream>
#include "DevStatus.h"

DevStatus::DevStatus()
{}

DevStatus::~DevStatus()
{}

void DevStatus::getStatus(KeepAliveNotify::Notify& notify)
{
    notify.CmdType = "Keepalive";
    notify.SN = "22";
    notify.DeviceID = "34020000001110000001";
    notify.Status = "OK";
}

void DevStatus::onKeepaliveSuccess()
{}

void DevStatus::onKeepaliveTimeout(int32_t count)
{
    std::cout << "Keepalive timeout: " << count << std::endl;
}