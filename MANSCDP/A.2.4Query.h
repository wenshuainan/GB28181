#ifndef A_2_4_QUERY_H
#define A_2_4_QUERY_H

#include <vector>
#include <memory>
#include "A.2.2CmdType.h"

/* A.2.4 查询命令 */
class QueryRequest : public CmdTypeRequest
{
private:
    std::vector<std::unique_ptr<CmdTypeSpecRequest>> spec;

public:
    QueryRequest(MANSCDPAgent *agent);
    virtual ~QueryRequest();

public:
    virtual bool match(const std::string& ReqType);
    virtual bool dispatch(const XMLElement *xmlReq);
};

/* A.2.4.1 查询命令消息体 */

/* A.2.4.2 设备状态查询请求 */
class DeviceStatusQuery : public CmdTypeSpecRequest
{
public:
    struct Request
    {
        /* 〈! -- 命令类型:设备状态查询(必选)--〉 */
        stringType CmdType;
        /* 〈! -- 命令序列号(必选)--〉 */
        SNType SN;
        /* 〈! -- 目标设备编码(必选)--〉 */
        deviceIDType DeviceID;
    };

public:
    DeviceStatusQuery(MANSCDPAgent *agent);
    virtual ~DeviceStatusQuery();

private:
    bool parse(const XMLElement *xmlReq, Request& req);

public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

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
    CatalogQuery(MANSCDPAgent *agent);
    virtual ~CatalogQuery();

private:
    bool parse(const XMLElement *xmlReq, Request& req);

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
    DeviceInfoQuery(MANSCDPAgent *agent);
    virtual ~DeviceInfoQuery();

private:
    bool parse(const XMLElement *xmlReq, Request& req);
    
public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

/* A.2.4.5 文件目录检索请求 */
class RecordInfoQuery : public CmdTypeSpecRequest
{
public:
    struct Request
    {
        /* 〈!-- 命令类型:文件目录检索(必选)--〉 */
        stringType CmdType;
        /* 〈!-- 命令序列号(必选)--〉 */
        SNType SN;
        /* 〈!-- 目标设备编码(必选)--〉 */
        deviceIDType DeviceID;
        /* 〈! -- 录像检索起始时间(必选)--〉 */
        dateTimeType StartTime;
        /* 〈! -- 录像检索终止时间(必选)--〉 */
        dateTimeType EndTime;
        /* 〈! -- 文件路径名 (可选)--〉 */
        stringType FilePath;
        /* 〈! -- 录像地址(可选 支持不完全查询)--〉 */
        stringType Address;
        /* 〈! -- 保密属性(可选)缺省为0;0-不涉密;1-涉密--〉 */
        integerType Secrecy;
        /* 〈! -- 录像产生类型(可选)time或alarm 或 manual或all--〉 */
        stringType Type;
        /* 〈! -- 录像触发者ID(可选)--〉 */
        stringType RecorderID;
        /* 〈! --录像模糊查询属性(可选)缺省为0;0-不进行模糊查询,此时根据SIP消息中 To头域 URI中
            的ID值确定查询录像位置,若ID值为本域系统ID则进行中心历史记录检索,若为前端设备ID则
            进行前端设备历史记录检索;1-进行模糊查询,此时设备所在域应同时进行中心检索和前端检索并
            将结果统一返回。--〉 
         */
        stringType IndistinctQuery;
        /* 〈! --码流编号(可选):0-主码流;1-子码流1;2-子码流2;以此类推--〉 */
        integerType StreamNumber;
        /* 〈! -- 报警方式条件(可选)取值0-全部;1-电话报警;2-设备报警;3-短信报警;4-GPS报警;5-视
            频报警;6-设备故障报警;7-其他报警;可以为直接组合如1/2为电话报警或设备报警--〉
         */
        stringType AlarmMethod;
        /* 〈! --报警类型(可选)。报警类型。报警方式为2时,不携带 AlarmType为默认的报警设备报
            警,携带 AlarmType取值及对应报警类型如下:1-视频丢失报警;2-设备防拆报警;3-存储设备磁盘
            满报警;4-设备高温报警;5-设备低温报警。报警方式为5时,取值如下:1-人工视频报警;2-运动目
            标检测报警;3-遗留物检测报警;4-物体移除检测报警;5-绊线检测报警;6-入侵检测报警;7-逆行检
            测报警;8-徘徊检测报警;9-流量统计报警;10-密度检测报警;11-视频异常检测报警;12-快速移动
            报警;13-图像遮挡报警。报警方式为6时,取值如下:1-存储设备磁盘故障报警;2-存储设备风扇故
            障报警--〉
         */
        stringType AlarmType;
    };

public:
    RecordInfoQuery(MANSCDPAgent *agent);
    virtual ~RecordInfoQuery();

private:
    bool parse(const XMLElement *xmlReq, Request& req);

public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

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