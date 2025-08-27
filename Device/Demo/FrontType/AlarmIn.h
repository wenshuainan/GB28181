#ifndef FRONT_ALRM_IN_H
#define FRONT_ALRM_IN_H

#include "../MANSCDPDevice.h"

class AlarmIn : public MANSCDPDevice
{
public:
    AlarmIn(const std::string &id, int32_t ch, Device *parent, MANSCDPAgent *agent);
    virtual ~AlarmIn();

public:
    virtual bool queryDeviceStatus(DeviceStatusQueryResponse& res); // 主设备/前端外围设备查询
    virtual bool queryAlarmStatus(DeviceStatusQueryResponse::_AlarmStatus::_Item& item); // 前端外围设备查询
    virtual bool queryCatalog(itemType& item); // 前端外围设备查询
    virtual bool getStatus(); //定时获取当前状态
};

#endif