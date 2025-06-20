#ifndef RESPONSE_H
#define RESPONSE_H

#include "A.2.1GlobalTypes.h"

/* A.2.6 应答命令 */

/*  A.2.6.1 应答命令消息体 */
class ResponseCmd : public ResponseCmdType
{
private:
    /* 〈! -- 查询请求的应答信息见 A.2.6.2~A.2.6.16-- */
    std::vector<ResponseSpecCmdType *> element;
};

/*  A.2.6.2 设备控制应答 */
class ResponseDeviceControl : public ResponseSpecCmdType
{
public:
    struct Response
    {
        /* 〈! -- 命令类型:设备控制(必选)--〉 */
        Type<std::string> CmdType;
        /* 〈!-- 命令序列号(必选)--〉 */
        SNType SN;
        /* 〈!-- 目标设备/区域/系统编码(必选)--〉 */
        deviceIDType DeviceID;
        /* 〈!-- 执行结果标志(必选)--〉 */
        resultType Result;
    };
};

/*  A.2.6.3 报警通知应答 */

/*  A.2.6.4 设备目录查询信息应答 */

/*  A.2.6.5 设备信息查询应答 */

/*  A.2.6.6 设备状态查询应答 */

/*  A.2.6.7 文件目录检索应答 */

/*  A.2.6.8 设备配置应答 */

/*  A.2.6.9 设备配置查询应答 */

/*  A.2.6.10 设备预置位查询应答 */

/*  A.2.6.11 语音广播应答 */

/*  A.2.6.12 看守位信息查询应答 */

/*  A.2.6.13 巡航轨迹列表查询应答 */

/*  A.2.6.14 巡航轨迹查询应答 */

/*  A.2.6.15 PTZ精确状态查询应答 */

/*  A.2.6.16 存储卡状态查询应答 */

#endif