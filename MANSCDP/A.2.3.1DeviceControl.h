#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include "MANSCDP/A.2.1GlobalTypes.h"

/* A.2.3.1 设备控制命令 */

/* A.2.3.1.1 设备控制命令消息体 */
class DeviceControl
{
protected:
    /* <! -- 命令类型：设备控制（必选） --> */
    const std::string CmdType = "DeviceControl";
    /* <! -- 命令序列号（必选） --> */
    SNType SN;
    /* <! -- 目标设备编码（必选） --> */
    deviceIDType DeviceID;
    /* <! -- 设备控制请求命令序列见 A.2.3.1.2 ~ A.2.3.1.14 --> */
    /* <! -- 扩展信息，可多项 --> */
};

/* A.2.3.1.2 摄像机云台控制命令 */
class PTZControl : public DeviceControl
{
public:
    /* <!-- 摄像机云台控制命令（可选，控制码应符合A.3的规定） --> */
    PTZCmdType PTZCmd;
    /* <!-- 摄像机云台控制命令附加参数（可选） --> */
    class
    {
    public:
        /* <!-- 预置位名称（PTZCmd 为设置预置位命令时可选） --> */
        std::string PresetName;
        /* <!-- 巡航轨迹名称（最长32字节，PTZCmd为巡航指令命令时可选） --> */
        std::string CruiseTrackName;
    } PTZCmdParams;
};

/* A.2.3.1.3 远程启动控制命令 */
class TeleBoot : public DeviceControl
{
public:
    enum
    {
        Boot
    };

public:
    std::string boot;
};

/* A.2.3.1.4 录像控制命令 */
class RecordControl
{
public:
    /* <!-- 录像控制命令（可选） --> */
    recordType RecordCmd;
    /* <!-- 码流类型：0-主码流，1-子码流，2-子码流2，以此类推（可选），缺省为0 --> */
    int StreamNumber;
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

#endif