#ifndef MANSCDP_DEVICE_H
#define MANSCDP_DEVICE_H

#include <memory>
#include "Device.h"
#include "Interface/9.3Control.h"
#include "Interface/9.4Alarm.h"
#include "Interface/9.5Query.h"
#include "Interface/9.6Status.h"
#include "Interface/9.7RecordQuery.h"

class MANSCDPAgent;
class PTZCommand;
class FICommand;

class MANSCDPDevice : public virtual Device
                    , public Control
                    , public Alarm
                    , public Query
                    , public Status
                    , public RecordQuery
{
protected:
    MANSCDPAgent *m_agent;

public:
    MANSCDPDevice(MANSCDPAgent *agent);
    virtual ~MANSCDPDevice();

public:
    /* 所有MANSCDP协议类接口
     * 全部返回false，表示默认不支持这项协议功能，
     * 如果派生类需要支持哪种功能则重写其中的接口
     */
    virtual bool controlPTZ(const PTZCommand *cmd);
    virtual bool controlFI(const FICommand *cmd);
    virtual bool reboot();
    virtual bool startRecord(int32_t stream = 0);
    virtual bool stopRecord(int32_t stream = 0);
    virtual bool setGuard();
    virtual bool resetGuard();
    virtual bool resetAlarm(int32_t method = 0, int32_t type = 0);

    virtual bool notifyAlarm(AlarmNotify &notify);

    virtual bool queryDeviceStatus(DeviceStatusQueryResponse& res); // 主设备/前端外围设备查询
    virtual bool queryAlarmStatus(DeviceStatusQueryResponse::_AlarmStatus::_Item& item); // 前端外围设备查询
    virtual bool queryCatalog(std::vector<itemType>& items); // 主设备查询
    virtual bool queryCatalog(itemType& item); // 前端外围设备查询
    virtual bool queryDeviceInfo(DeviceInfoQueryResponse& res); // 主设备/前端外围设备查询

    virtual bool getStatus(std::vector<std::string>& offDevices);
    virtual bool getStatus(); //定时获取当前状态
    virtual bool sendKeepalive();
    virtual void onKeepaliveSuccess();
    virtual void onKeepaliveTimeout(int32_t count);

    virtual std::unique_ptr<RecordQuery::Handle> createHandle(const RecordInfoQuery::Request &req);
};

#endif