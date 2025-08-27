#ifndef FRONT_ALARM_OUT_H
#define FRONT_ALARM_OUT_H

#include "../MANSCDPDevice.h"

class AlarmOut : public MANSCDPDevice
{
public:
    AlarmOut(const std::string &id, int32_t ch, Device *parent, MANSCDPAgent *agent);
    virtual ~AlarmOut();

public:
    virtual bool queryDeviceStatus(DeviceStatusQueryResponse& res); // 主设备/前端外围设备查询
    virtual bool queryCatalog(itemType& item); // 前端外围设备查询
    virtual bool getStatus(); //定时获取当前状态
};

#endif