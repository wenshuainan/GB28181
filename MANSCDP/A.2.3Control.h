#ifndef A_2_3_CONTROL_H
#define A_2_3_CONTROL_H

#include <vector>
#include <memory>
#include "A.2.2CmdType.h"

/* A.2.3 控制命令 */
class ControlReuest : public CmdTypeRequest
{
private:
    std::vector<std::unique_ptr<CmdTypeControl>> spec;

public:
    ControlReuest(MANSCDPAgent *agent);
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
    std::vector<std::unique_ptr<CmdTypeSpecRequest>> spec;

public:
    DeviceControlRequest(MANSCDPAgent *agent);
    virtual ~DeviceControlRequest();

public:
    static bool parse(const XMLElement *xmlReq, Request& req);

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
    PTZCmdControl(MANSCDPAgent *agent);
    virtual ~PTZCmdControl();

private:
    bool parse(const XMLElement *xmlReq, Request& req);

public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

/* A.2.3.1.3 远程启动控制命令 */
class TeleBootControl : public CmdTypeSpecRequest
{
public:
    struct Request : DeviceControlRequest::Request
    {
        /* <!-- 远程启动控制命令（可选） --> */
        stringType TeleBoot;
    };

public:
    TeleBootControl(MANSCDPAgent *agent);
    virtual ~TeleBootControl();

private:
    bool parse(const XMLElement *xmlReq, Request& req);

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
    RecordControl(MANSCDPAgent *agent);
    virtual ~RecordControl();

private:
    bool parse(const XMLElement *xmlReq, Request& req);

public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

/* A.2.3.1.5 报警布防/撤防控制命令 */
class GuardControl : public CmdTypeSpecRequest
{
public:
    struct Request : DeviceControlRequest::Request
    {
        /* <!-- 报警布防/撤防命令（可选） --> */
        guardType GuardCmd;
    };

public:
    GuardControl(MANSCDPAgent *agent);
    virtual ~GuardControl();
    
private:
    bool parse(const XMLElement *xmlReq, Request& req);
    
public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

/* A.2.3.1.6 报警复位控制命令 */
class AlarmControl : public CmdTypeSpecRequest
{
public:
    struct Request : DeviceControlRequest::Request
    {
        /* <!-- 报警复位命令（可选） --> */
        stringType AlarmCmd;
        /* 〈! --报警复位控制时,扩展此项,携带报警方式、报警类型。--〉 */
        struct
        {
            /* 〈! --复位报警的报警方式属性,取值0-全部;1-电话报警;2-设备报警;3-短信
                报警;4-GPS报警;5-视频报警;6-设备故障报警;7-其他报警;可以为直接组合如1/2
                为电话报警或设备报警--〉
            */
            stringType AlarmMethod;
            /* 〈! --复位报警的报警类型属性。报警类型。报警方式为2时,不携带 AlarmType
                为默认的报警设备报警,携带 AlarmType取值及对应报警类型如下:1-视频丢失报
                警;2-设备防拆报警;3-存储设备磁盘满报警;4-设备高温报警;5-设备低温报警。报
                警方式为5时,取值如下:1-人工视频报警;2-运动目标检测报警;3-遗留物检测报警;
                4-物体移除检测报警;5-绊线检测报警;6-入侵检测报警;7-逆行检测报警;8-徘徊检
                测报警;9-流量统计报警;10-密度检测报警;11-视频异常检测报警;12-快速移动报
                警;13-图像遮挡报警。报警方式为6时,取值如下:1-存储设备磁盘故障报警;2-存储
                设备风扇故障报警--〉
            */
            stringType AlarmType;
        } Info;
    };

public:
    AlarmControl(MANSCDPAgent *agent);
    virtual ~AlarmControl();
    
private:
    bool parse(const XMLElement *xmlReq, Request& req);
    
public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

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
    std::vector<std::unique_ptr<CmdTypeSpecRequest>> spec;
    
public:
    DeviceConfigRequest(MANSCDPAgent *agent);
    virtual ~DeviceConfigRequest();
    static bool parse(const XMLElement *xmlReq, Request& req);

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