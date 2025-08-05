#include <iostream>
#include "DevQuery.h"

DevQuery::DevQuery()
{}

DevQuery::~DevQuery()
{}

bool DevQuery::queryCatalog(int32_t ch, itemType& item)
{
    item.Name = "DemoDev";
    item.Manufacturer = "Manufacturer 1";
    item.Model = "Model 1";
    item.CivilCode = "CivilCode 1";
    item.Address = "Address 1";
    item.RegisterWay = 1;
    item.Secrecy = 0;
    item.Status = statusType::ON;

    return true;
}

bool DevQuery::queryDeviceInfo(DeviceInfoQueryResponse& res)
{
    res.DeviceName = "DemoDev";
    res.Manufacturer = "Manufacturer 1";
    res.Model = "Model 1";
    res.Firmware = "Firmware 1.0";

    return true;
}
