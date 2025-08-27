#include "AlarmOut.h"

AlarmOut::AlarmOut(const std::string &id, int32_t ch, Device *parent, MANSCDPAgent *agent)
    : Device(id, ch, parent)
    , MANSCDPDevice(agent)
{
    printf("++++++ AlarmOut\n");
}

AlarmOut::~AlarmOut()
{
    printf("----- AlarmOut\n");
}

bool AlarmOut::queryDeviceStatus(DeviceStatusQueryResponse& res)
{
    res.Online = DeviceStatusQueryResponse::ONLINE;
    res.Status = statusType::ON;
    return true;
}

bool AlarmOut::queryCatalog(itemType& item)
{
    item.DeviceID = m_id;
    item.Name = "AlarmOut 1";
    item.Manufacturer = "Manufacturer";
    item.Model = "Model";
    item.CivilCode = "CivilCode";
    item.Address = "Address";
    item.Parental = 1;
    item.ParentID = m_parent->getId();
    item.RegisterWay = 1;
    item.Secrecy = 0;
    item.Status = statusType::ON;

    return true;
}

bool AlarmOut::getStatus()
{
    return true;
}