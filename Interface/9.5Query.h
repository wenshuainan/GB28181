#ifndef QUERY_INTERFACE_H
#define QUERY_INTERFACE_H

#include "MANSCDP/A.2.4Query.h"
#include "MANSCDP/A.2.6Response.h"

class Query
{
public:
    virtual bool queryDeviceStatus(DeviceStatusQueryResponse& res) = 0; // 主设备/前端外围设备查询
    virtual bool queryAlarmStatus(DeviceStatusQueryResponse::_AlarmStatus::_Item& item) = 0; // 前端外围设备查询
    virtual bool queryCatalog(std::vector<itemType>& items) = 0; // 主设备查询
    virtual bool queryCatalog(itemType& item) = 0; // 前端外围设备查询
    virtual bool queryDeviceInfo(DeviceInfoQueryResponse& res) = 0; // 主设备/前端外围设备查询
};

#endif