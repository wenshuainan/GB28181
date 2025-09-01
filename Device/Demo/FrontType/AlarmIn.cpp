#include "AlarmIn.h"

AlarmIn::AlarmIn(const std::string &id, int32_t ch, Device *parent, MANSCDPAgent *agent)
    : Device(id, ch, parent)
    , MANSCDPDevice(agent)
{
    printf("++++++ AlarmIn\n");
}

AlarmIn::~AlarmIn()
{
    printf("----- AlarmIn\n");
}

bool AlarmIn::queryDeviceStatus(DeviceStatusQueryResponse& res)
{
    res.Online = DeviceStatusQueryResponse::ONLINE;
    res.Status = resultType::OK;
    res.AlarmStatus.Num = 1;
    res.AlarmStatus.Item.resize(1);
    queryAlarmStatus(res.AlarmStatus.Item[0]);
    return true;
}

bool AlarmIn::queryAlarmStatus(DeviceStatusQueryResponse::_AlarmStatus::_Item& item)
{
    item.DeviceID = m_id;
    item.DutyStatus = DeviceStatusQueryResponse::_AlarmStatus::_Item::OFFDUTY;
    return true;
}

bool AlarmIn::queryCatalog(itemType& item)
{
    item.DeviceID = m_id;
    item.Name = "AlarmIn 1";
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

bool AlarmIn::getStatus()
{
    return true;
}