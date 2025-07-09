#ifndef A_2_4_QUERY_H
#define A_2_4_QUERY_H

#include <memory>
#include "A.2.2CmdType.h"

/* A.2.4 查询命令 */
class QueryRequest : public CmdTypeRequest
{
private:
    std::vector<std::shared_ptr<CmdTypeSpecRequest>> spec;

public:
    QueryRequest(MANSCDPAgent *agent, Query *query);
    virtual ~QueryRequest();

public:
    virtual bool match(const std::string& ReqType);
    virtual bool dispatch(const XMLElement *xmlReq);
};

/* A.2.4.1 查询命令消息体 */

/* A.2.4.2 设备状态查询请求 */

/* A.2.4.3 设备目录查询或订阅请求 */
class CatalogQuery : public CmdTypeSpecRequest
{
public:
    struct Request
    {
        /* 〈! -- 命令类型:设备目录查询(必选)--〉 */
        stringType CmdType;
        /* 〈! -- 命令序列号(必选)--〉 */
        SNType SN;
        /* 〈! -- 目标设备/区域/系统/业务分组/虚拟组织编码(必选)--〉 */
        deviceIDType DeviceID;
        /* 〈! -- 增加设备的起始时间(可选)空表示不限--〉 */
        /* 〈elementname="StartTime"type="dateTime"minOccurS= "0"/〉 */
        /* 〈! -- 增加设备的终止时间(可选)空表示到当前时间--〉 */
        /* 〈elementname="EndTime"type="dateTime"minOccurS= "0"/〉 */
    };

public:
    CatalogQuery(MANSCDPAgent *agent, Query *query);
    virtual ~CatalogQuery();

private:
    bool deserialize(const XMLElement *xmlReq, Request& req);

public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

/* A.2.4.4 设备信息查询请求 */
class DeviceInfoQuery : public CmdTypeSpecRequest
{
public:
    struct Request
    {
        /* 〈!-- 命令类型:设备信息查询(必选)--〉 */
        stringType CmdType;
        /* 〈!-- 命令序列号(必选)--〉 */
        SNType SN;
        /* 〈! -- 目标设备编码(必选)--〉 */
        deviceIDType DeviceID;
    };
    
public:
    DeviceInfoQuery(MANSCDPAgent *agent, Query *query);
    virtual ~DeviceInfoQuery();

private:
    bool deserialize(const XMLElement *xmlReq, Request& req);
    
public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

/* A.2.4.5 文件目录检索请求 */

/* A.2.4.6 报警订阅请求 */

/* A.2.4.7 设备配置查询请求 */

/* A.2.4.8 设备预置位查询请求 */

/* A.2.4.9 移动设备位置数据订阅请求 */

/* A.2.4.10 看守位信息查询请求 */

/* A.2.4.11 巡航轨迹列表查询请求 */

/* A.2.4.12 巡航轨迹查询请求 */

/* A.2.4.13 PTZ精准状态查询或订阅请求 */

/* A.2.4.14 存储卡状态查询请求 */

#endif