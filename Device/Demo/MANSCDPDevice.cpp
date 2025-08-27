#include "MANSCDPDevice.h"

MANSCDPDevice::MANSCDPDevice(MANSCDPAgent *agent)
    : Device("", -1)
    , m_agent(agent)
{
    printf("++++++ MANSCDPDevice\n");
}

MANSCDPDevice::~MANSCDPDevice()
{
    printf("------ MANSCDPDevice\n");
}

bool MANSCDPDevice::controlPTZ(const PTZCommand *cmd)
{
    return false;
}

bool MANSCDPDevice::controlFI(const FICommand *cmd)
{
    return false;
}

bool MANSCDPDevice::reboot()
{
    return false;
}

bool MANSCDPDevice::startRecord(int32_t stream)
{
    return false;
}

bool MANSCDPDevice::stopRecord(int32_t stream)
{
    return false;
}

bool MANSCDPDevice::setGuard()
{
    return false;
}

bool MANSCDPDevice::resetGuard()
{
    return false;
}

bool MANSCDPDevice::resetAlarm(int32_t method, int32_t type)
{
    return false;
}

bool MANSCDPDevice::notifyAlarm(AlarmNotify &notify)
{
    return false;
}

bool MANSCDPDevice::queryDeviceStatus(DeviceStatusQueryResponse& res)
{
    return false;
}

bool MANSCDPDevice::queryAlarmStatus(DeviceStatusQueryResponse::_AlarmStatus::_Item& item)
{
    return false;
}

bool MANSCDPDevice::queryCatalog(std::vector<itemType>& items)
{
    return false;
}

bool MANSCDPDevice::queryCatalog(itemType& item)
{
    return false;
}

bool MANSCDPDevice::queryDeviceInfo(DeviceInfoQueryResponse& res)
{
    return false;
}

bool MANSCDPDevice::getStatus(std::vector<std::string>& offDevices)
{
    return false;
}

bool MANSCDPDevice::getStatus()
{
    return false;
}

bool MANSCDPDevice::sendKeepalive()
{
    return false;
}

void MANSCDPDevice::onKeepaliveSuccess()
{}

void MANSCDPDevice::onKeepaliveTimeout(int32_t count)
{}

std::unique_ptr<RecordQuery::Handle> MANSCDPDevice::createHandle(const RecordInfoQuery::Request &req)
{
    return nullptr;
}