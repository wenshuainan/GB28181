#ifndef A_2_3_CONTROL_H
#define A_2_3_CONTROL_H

#include <memory>
#include "A.2.2CmdType.h"

/* A.2.3 控制命令 */
class ControlReuest : public CmdTypeRequest
{
private:
    std::vector<std::shared_ptr<CmdTypeControl>> spec;

public:
    ControlReuest(MANSCDPAgent *agent, Control *control);
    virtual ~ControlReuest();

public:
    virtual bool match(const std::string& ReqType);
    virtual bool dispatch(const XMLElement *xmlReq);
};

/* A.2.3.1 设备控制命令 */

/* A.2.3.1.1 设备控制命令消息体 */
class DeviceControlRequest : public CmdTypeControl
{
public:
    struct Request
    {
        /* <! -- 命令类型：设备控制（必选） --> */
        stringType CmdType;
        /* <! -- 命令序列号（必选） --> */
        SNType SN;
        /* <! -- 目标设备编码（必选） --> */
        deviceIDType DeviceID;
        /* <! -- 设备控制请求命令序列见 A.2.3.1.2 ~ A.2.3.1.14 --> */
        /* <! -- 扩展信息，可多项 --> */
    };

private:
    std::vector<std::shared_ptr<CmdTypeSpecRequest>> spec;

public:
    DeviceControlRequest(MANSCDPAgent *agent, Control *control);
    virtual ~DeviceControlRequest();
    static bool deserialize(const XMLElement *xmlReq, Request& req);

public:
    virtual bool match(const std::string& CmdType);
    virtual bool dispatch(const XMLElement *xmlReq);
};

/* A.2.3.1.2 摄像机云台控制命令 */
class PTZCmdControl : public CmdTypeSpecRequest
{
public:
    struct Request : DeviceControlRequest::Request
    {
        /* <!-- 摄像机云台控制命令（可选，控制码应符合A.3的规定） --> */
        PTZCmdType PTZCmd;
        /* <!-- 摄像机云台控制命令附加参数（可选） --> */
        struct
        {
            /* <!-- 预置位名称（PTZCmd 为设置预置位命令时可选） --> */
            stringType PresetName;
            /* <!-- 巡航轨迹名称（最长32字节，PTZCmd为巡航指令命令时可选） --> */
            stringType CruiseTrackName;
        } PTZCmdParams;
    };

public:
    PTZCmdControl(MANSCDPAgent *agent, Control *control);
    virtual ~PTZCmdControl();

private:
    bool deserialize(const XMLElement *xmlReq, Request& req);

public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

/* A.2.3.1.3 远程启动控制命令 */
class TeleBootControl : public CmdTypeSpecRequest
{
public:
    enum EBoot
    {
        Boot
    };

    struct Request : DeviceControlRequest::Request
    {
        integerType Boot;
    };

public:
    TeleBootControl(MANSCDPAgent *agent, Control *control);
    virtual ~TeleBootControl();

private:
    bool deserialize(const XMLElement *xmlReq, Request& req);

public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

/* A.2.3.1.4 录像控制命令 */
class RecordControl : public CmdTypeSpecRequest
{
public:
    struct Request : DeviceControlRequest::Request
    {
        /* <!-- 录像控制命令（可选） --> */
        recordType RecordCmd;
        /* <!-- 码流类型：0-主码流，1-子码流，2-子码流2，以此类推（可选），缺省为0 --> */
        integerType StreamNumber;
    };

public:
    RecordControl(MANSCDPAgent *agent, Control *control);
    virtual ~RecordControl();

private:
    bool deserialize(const XMLElement *xmlReq, Request& req);

public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

/* A.2.3.1.5 报警布防/撤防控制命令 */

/* A.2.3.1.6 报警复位控制命令 */

/* A.2.3.1.7 强制关键帧控制命令 */

/* A.2.3.1.8 拉框放大控制命令 */

/* A.2.3.1.9 拉框缩小控制命令 */

/* A.2.3.1.10 看守位控制命令 */

/* A.2.3.1.11 PTZ精准控制控制命令 */

/* A.2.3.1.12 设备软件升级控制命令 */

/* A.2.3.1.13 存储卡格式化控制命令 */

/* A.2.3.1.14 目标跟踪控制命令 */

/* A.2.3.2 设备配置命令 */

/* A.2.3.2.1 设备配置命令消息体 */
class DeviceConfigRequest : public CmdTypeControl
{
public:
    struct Request
    {
        /* <! -- 命令类型：设备配置（必选） -> */
        stringType CmdType;
        /* <! -- 命令序列号（必选） -> */
        SNType SN;
        /* <! -- 目标设备编码（必选） -> */
        deviceIDType DeviceID;
    };

private:
    std::vector<std::shared_ptr<CmdTypeSpecRequest>> spec;
    
public:
    DeviceConfigRequest(MANSCDPAgent *agent, Control *control);
    virtual ~DeviceConfigRequest();
    static bool deserialize(const XMLElement *xmlReq, Request& req);

public:
    virtual bool match(const std::string& CmdType);
    virtual bool dispatch(const XMLElement *xmlReq);
};

/* A.2.3.2.2 基本参数配置命令 */

/* A.2.3.2.3 SVAC编码配置命令 */

/* A.2.3.2.4 SVAC解码配置命令 */

/* A.2.3.2.5 视频参数属性配置命令 */

/* A.2.3.2.6 录像计划配置命令 */

/* A.2.3.2.7 报警录像配置命令 */

/* A.2.3.2.8 视频画面遮挡配置命令 */

/* A.2.3.2.9 画面翻转配置命令 */

/* A.2.3.2.10 报警上报开关配置命令 */

/* A.2.3.2.11 前端OSD配置命令 */

/* A.2.3.2.12 图像抓拍配置命令 */

#endif