#ifndef A_2_6_RESPONSE_H
#define A_2_6_RESPONSE_H

#include <vector>
#include "A.2.3Control.h"
#include "A.2.4Query.h"

/* A.2.6 应答命令 */

/* A.2.6.1 应答命令消息体 */
class CmdTypeResponse
{
protected:
    bool encode(XMLDocument *xmldocRes);
};

/*  A.2.6.2 设备控制应答 */
class DeviceControlResponse : public CmdTypeResponse
{
public:
    /* 〈! -- 命令类型:设备控制(必选)--〉 */
    stringType CmdType;
    /* 〈!-- 命令序列号(必选)--〉 */
    SNType SN;
    /* 〈!-- 目标设备/区域/系统编码(必选)--〉 */
    deviceIDType DeviceID;
    /* 〈!-- 执行结果标志(必选)--〉 */
    resultType Result;

public:
    DeviceControlResponse(const DeviceControlRequest::Request& req);
    bool encode(XMLDocument *xmldocRes);
};

/*  A.2.6.3 报警通知应答 */
class AlarmNotifyResponse : public CmdTypeResponse
{
public:
    /* 〈! -- 命令类型:报警通知(必选)--〉 */
    stringType CmdType;
    /* 〈!-- 命令序列号(必选)--〉 */
    SNType SN;
    /* 〈! -- 目标设备/区域/系统编码(必选)--〉 */
    deviceIDType DeviceID;
    /* 〈! -- 执行结果标志(必选)--〉 */
    resultType Result;
};

/*  A.2.6.4 设备目录信息查询应答 */
class CatalogQueryResponse : public CmdTypeResponse
{
public:
    /* 〈!-- 命令类型:设备目录查询(必选)--〉 */
    stringType CmdType;
    /* 〈!-- 命令序列号(必选)--〉 */
    SNType SN;
    /* 〈! -- 目标设备/区域/系统/业务分组/虚拟组织的编码,取值与目录查询请求相同(必选)--〉 */
    deviceIDType DeviceID;
    /* 〈! -- 查询结果总数(必选)--〉 */
    integerType SumNum;
    /* 〈! -- 设备目录项列表,Num 表示目录项个数--〉 */
    struct
    {
        std::vector<itemType> Item;
        integerType Num; //attribute
    } DeviceList;
    /* 〈! -- 扩展信息,可多项--〉 */

public:
    CatalogQueryResponse(const CatalogQuery::Request& req);
    bool encode(XMLDocument *xmldocRes);
};

/*  A.2.6.5 设备信息查询应答 */
class DeviceInfoQueryResponse : public CmdTypeResponse
{
public:
    /* 〈! -- 命令类型:设备信息查询(必选)--〉 */
    stringType CmdType;
    /* 〈!-- 命令序列号(必选)--〉 */
    SNType SN;
    /* 〈! -- 目标设备的编码(必选)--〉 */
    deviceIDType DeviceID;
    /* 〈! --目标设备的名称(可选)--〉 */
    stringType DeviceName;
    /* 〈! -- 查询结果(必选)--〉 */
    resultType Result;
    /* 〈! -- 设备生产商(可选)--〉 */
    stringType Manufacturer;
    /* 〈!-- 设备型号(可选)--〉 */
    stringType Model;
    /* 〈!-- 设备固件版本(可选)--〉 */
    stringType Firmware;
    /* 〈! -- 视频输入通道数(可选)--〉 */
    integerType Channel;
    /* 〈! -- 扩展信息,可多项--〉 */

public:
    DeviceInfoQueryResponse(const DeviceInfoQuery::Request& req);
    bool encode(XMLDocument *xmldocRes);
};

/*  A.2.6.6 设备状态查询应答 */

/*  A.2.6.7 文件目录检索应答 */
class RecordInfoQueryResponse : public CmdTypeResponse
{
public:
    /* 〈! -- 命令类型:文件目录查询(必选)--〉 */
    stringType CmdType;
    /* 〈! -- 命令序列号(必选)--〉 */
    SNType SN;
    /* 〈! -- 目标设备编码(必选)--〉 */
    deviceIDType DeviceID;
    /* 〈! -- 设备/区域名称(必选)--〉 */
    stringType Name;
    /* 〈! -- 查询结果总数(必选)--〉 */
    integerType SumNum;
    /* 〈! -- 文件目录项列表,Num 表示目录项个数--〉 */
    struct
    {
        std::vector<itemFileType> Item;
        integerType Num; //attribute
    } RecordList;

public:
    RecordInfoQueryResponse(const RecordInfoQuery::Request& req);
    bool encode(XMLDocument *xmldocRes);
};

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