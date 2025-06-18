#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include "MANSCDP/A.2.1GlobalTypes.h"

/* A.2.3.2 设备配置命令 */

/* A.2.3.2.1 设备配置命令消息体 */
class DeviceConfig
{
protected:
    /* <! -- 命令类型：设备配置（必选） -> */
    const std::string CmdType = "DeviceConfig";
    /* <! -- 命令序列号（必选） -> */
    SNType SN;
    /* <! -- 目标设备编码（必选） -> */
    deviceIDType DeviceID;
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