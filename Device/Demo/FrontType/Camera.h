#ifndef FRONT_CAMERA_H
#define FRONT_CAMERA_H

#include "../MANSCDPDevice.h"
#include "../SessionDevice.h"

class Camera : public MANSCDPDevice
             , public SessionDevice
{
private:
    bool m_bGuard;

public:
    Camera(const std::string &id, int32_t ch, Device *parent, MANSCDPAgent *agent);
    virtual ~Camera();

public:
    /* 摄像机需要实现的MANSCDP协议接口 */
    virtual bool controlPTZ(const PTZCommand *cmd);
    virtual bool controlFI(const FICommand *cmd);
    virtual bool startRecord(int32_t stream = 0);
    virtual bool stopRecord(int32_t stream = 0);
    virtual bool setGuard();
    virtual bool resetGuard();
    virtual bool queryDeviceStatus(DeviceStatusQueryResponse& res); // 主设备/前端外围设备查询
    virtual bool queryCatalog(itemType& item); // 前端外围设备查询
    virtual bool getStatus(); //定时获取当前状态
    virtual std::unique_ptr<RecordQuery::Handle> createHandle(const RecordInfoQuery::Request &req);
};

class RecordQueryHandle : public RecordQuery::Handle
{
    friend Camera;
    
private:
    MANSCDPDevice *m_device;

public:
    RecordQueryHandle(int32_t ch, MANSCDPDevice *device);
    virtual ~RecordQueryHandle();

public:
    virtual int32_t querySumNum();
    virtual bool queryRecordInfo(int32_t num, std::vector<itemFileType>& recordlist);
};

#endif