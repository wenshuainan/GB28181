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
    bool match(const XMLElement *cmd);
};

/*  A.2.6.2 设备控制应答 */
class DeviceControlResponse : public CmdTypeResponse, public MessageResultHandler
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
    DeviceControlResponse(MANSCDPAgent *agent, const DeviceControlRequest::Request& req);
    virtual ~DeviceControlResponse();

private:
    bool encode(XMLDocument *xmldocRes);

public:
    bool response(std::shared_ptr<MessageResultHandler> handler = nullptr);

public:
    virtual bool match(const XMLElement *cmd);
    virtual bool handle(int32_t code);
};

/*  A.2.6.3 报警通知应答 */

/*  A.2.6.4 设备目录信息查询应答 */
class CatalogQueryResponse : public CmdTypeResponse, public MessageResultHandler
{
    friend CatalogQuery;

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

private:
    std::shared_ptr<CatalogQueryResponse> m_next; // 多响应消息，指向下一条，等收到上一条SIP响应后再发送 GB28181 附录M

public:
    CatalogQueryResponse(MANSCDPAgent *agent, const CatalogQuery::Request& req);
    virtual ~CatalogQueryResponse();

private:
    bool encode(XMLDocument *xmldocRes);

public:
    bool response(std::shared_ptr<MessageResultHandler> handler = nullptr);

public:
    virtual bool match(const XMLElement *cmd);
    virtual bool handle(int32_t code);
};

/*  A.2.6.5 设备信息查询应答 */
class DeviceInfoQueryResponse : public CmdTypeResponse, public MessageResultHandler
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
    DeviceInfoQueryResponse(MANSCDPAgent *agent, const DeviceInfoQuery::Request& req);
    virtual ~DeviceInfoQueryResponse();

private:
    bool encode(XMLDocument *xmldocRes);

public:
    bool response(std::shared_ptr<MessageResultHandler> handler = nullptr);

public:
    virtual bool match(const XMLElement *cmd);
    virtual bool handle(int32_t code);
};

/*  A.2.6.6 设备状态查询应答 */
class DeviceStatusQueryResponse : public CmdTypeResponse, public MessageResultHandler
{
public:
    /* 〈! -- 命令类型:设备状态查询(必选)--〉 */
    stringType CmdType;
    /* 〈!-- 命令序列号(必选)--〉 */
    SNType SN;
    /* 〈! -- 目标设备/区域/系统/业务分组/虚拟组织的编码,取值与目录查询请求相同(必选)--〉 */
    deviceIDType DeviceID;
    /* 〈! -- 查询结果(必选)--〉 */
    resultType Result;
    /* 〈! -- 是否在线(必选)--〉 */
    enum
    {
        ONLINE,
        OFFLINE,
    } Online;
    /* 〈! -- 是否正常工作(必选)--〉 */
    resultType Status;
    /* 〈! -- 不正常工作原因(可选)--〉 */
    stringType Reason;
    /* 〈! -- 是否编码(可选)--〉 */
    statusType Encode;
    /* 〈! -- 是否录像(可选)--〉 */
    statusType Record;
    /* 〈! -- 设备时间和日期(可选)--〉 */
    dateTimeType DeviceTime;
    /* 〈! --报警设备状态列表,num 表示列表项个数(可选)--〉 */
    struct _AlarmStatus
    {
        struct _Item
        {
            deviceIDType DeviceID;
            enum
            {
                ONDUTY,
                OFFDUTY,
                ALARM,
            } DutyStatus;
        };
        std::vector<_Item> Item;
        integerType Num; //attribute
    } AlarmStatus;
    /* 〈! -- 扩展信息,可多项--〉 */

public:
    DeviceStatusQueryResponse(MANSCDPAgent *agent, const DeviceStatusQuery::Request& req);
    virtual ~DeviceStatusQueryResponse();

private:
    bool encode(XMLDocument *xmldocRes);

public:
    bool response(std::shared_ptr<MessageResultHandler> handler = nullptr);

public:
    virtual bool match(const XMLElement *cmd);
    virtual bool handle(int32_t code);
};

/*  A.2.6.7 文件目录检索应答 */
class RecordInfoQueryResponse : public CmdTypeResponse, public MessageResultHandler
{
    friend RecordInfoQuery;

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

private:
    std::shared_ptr<RecordInfoQueryResponse> m_next; // 多响应消息，指向下一条，等收到上一条SIP响应后再发送 GB28181 附录M

public:
    RecordInfoQueryResponse(MANSCDPAgent *agent, const RecordInfoQuery::Request& req);
    virtual ~RecordInfoQueryResponse();

private:
    bool encode(XMLDocument *xmldocRes);

public:
    bool response(std::shared_ptr<MessageResultHandler> handler = nullptr);

public:
    virtual bool match(const XMLElement *cmd);
    virtual bool handle(int32_t code);
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