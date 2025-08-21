#include <iostream>
#include "DevQuery.h"
#include "Agent/MANSCDPAgent.h"

DevQuery::DevQuery(MANSCDPAgent *agent)
    : Query(agent)
{}

DevQuery::~DevQuery()
{}

bool DevQuery::queryCatalog(int32_t ch, itemType& item)
{
    item.DeviceID = m_agent->getDeviceId(ch);
    item.Name = "Channel 1";
    item.Manufacturer = "Manufacturer";
    item.Model = "Model";
    item.CivilCode = "CivilCode";
    item.Address = "Address";
    item.Parental = 1;
    item.ParentID = m_agent->getMainDeviceId();
    item.RegisterWay = 1;
    item.Secrecy = 0;
    item.Status = statusType::ON;

    return true;
}

bool DevQuery::queryDeviceInfo(DeviceInfoQueryResponse& res)
{
    res.DeviceName = "DemoDev";
    res.Manufacturer = "Manufacturer";
    res.Model = "Model";
    res.Firmware = "Firmware 1.0";

    return true;
}
