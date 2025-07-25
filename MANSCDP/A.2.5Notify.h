#ifndef A_2_5_NOTIFY_H
#define A_2_5_NOTIFY_H

#include <vector>
#include <memory>
#include "A.2.2CmdType.h"
#include "SIP/Adapter/SipAdapter.h"

/* A.2.5 通知命令 */

/* A.2.5.1 通知命令消息体 */
class NotifyRequest
{
protected:
    bool encode(XMLDocument *xmldocNotify);
};

/* A.2.5.2 状态信息报送 */
class KeepaliveNotify : public NotifyRequest
{
public:
    /* 〈! -- 命令类型:设备状态信息报送(必选)--〉 */
    stringType CmdType;
    /* 〈! -- 命令序列号(必选)--〉 */
    SNType SN;
    /* 〈! -- 源设备/系统编码(必选)--〉 */
    deviceIDType DeviceID;
    /* 〈! -- 是否正常工作(必选)--〉 */
    resultType Status;
    /* 〈! --故障设备列表--〉 */
    struct
    {
        std::vector<deviceIDType> DeviceID;
    } Info;

private:
    MANSCDPAgent *m_agent;
    class Status *m_devStatus;

public:
    KeepaliveNotify(MANSCDPAgent *agent, class Status *status);
    virtual ~KeepaliveNotify();

private:
    bool encode(XMLDocument *xmldocNotify);

public:
    virtual bool notify();
};

/* A.2.5.3 报警通知 */
class AlarmNotify : public NotifyRequest
{
public:
    /* 〈! -- 命令类型:报警通知(必选)--〉 */
    stringType CmdType;
    /* 〈! -- 命令序列号(必选)--〉 */
    SNType SN;
    /* 〈! -- 报警设备编码或报警中心编码(10位)(必选)--〉 */
    deviceIDType DeviceID;
    /* 〈! -- 报警级别(必选),1-一级警情;2-二级警情;3-三级警情;4-四级警情--〉 */
    stringType AlarmPriority;
    /* 〈! -- 报警方式(必选),取值1-电话报警;2-设备报警;3-短信报警;4-GPS报警;5-视频报警;6-设
        备故障报警;7-其他报警--〉
    */
    stringType AlarmMethod;
    /* 〈! --报警时间(必选)--〉 */
    dateTimeType AlarmTime;
    /* 〈! --报警内容描述(可选)--〉 */
    stringType AlarmDescription;
    /* 〈! -- 经纬度信息可选--〉 */
    doubleType Longitude;
    doubleType Latitude;
    /* 〈! -- 扩展Info项携带报警类型、报警类型参数字段--〉 */
    struct
    {
        /* 〈! -- 报警类型。报警方式为2时,不携带 AlarmType为默认的报警设备报警,携带 Ala-
            rmType取值及对应报警类型如下:1-视频丢失报警;2-设备防拆报警;3-存储设备磁盘满报
            警;4-设备高温报警;5-设备低温报警。报警方式为5时,取值如下:1-人工视频报警;2-运动
            目标检测报警;3-遗留物检测报警;4-物体移除检测报警;5-绊线检测报警;6-入侵检测报警;
            7-逆行检测报警;8-徘徊检测报警;9-流量统计报警;10-密度检测报警;11-视频异常检测报
            警;12-快速移动报警;13-图像遮挡报警。报警方式为6时,取值如下:1-存储设备磁盘故障
            报警;2-存储设备风扇故障报警。--〉
        */
        integerType AlarmType;
        /* 〈! —报 警 类 型 扩 展 参 数。 在 入 侵 检 测 报 警 时 可 携 带 〈EventType〉事 件 类 型 〈/
            EventType〉,事件类型取值:1-进入区域;2-离开区域。--〉
        */
        struct
        {
            integerType EventType;
        } AlarmTypeParam;
    } Info;
    /* 〈! -- 扩展信息,可多项--〉 */

private:
    MANSCDPAgent *m_agent;

public:
    AlarmNotify(MANSCDPAgent *agent);
    virtual ~AlarmNotify();

private:
    bool encode(XMLDocument *xmldocNotify);

public:
    virtual bool notify();
};

/* A.2.5.4 媒体通知 */
class MediaStatusNotify : public NotifyRequest
{
public:
    /* 〈! --命令类型:媒体通知(必选)--〉 */
    stringType CmdType;
    /* 〈! --命令序列号(必选)--〉 */
    SNType SN;
    /* 〈! --媒体发送设备编码(必选)--〉 */
    deviceIDType DeviceID;
    /* 〈! --媒体发送设备编码(必选)--〉 */
    stringType NotifyType;

private:
    MANSCDPAgent *m_agent;
    const SessionIdentifier& m_sessionId;

public:
    MediaStatusNotify(MANSCDPAgent *agent, const SessionIdentifier& sessionId);
    virtual ~MediaStatusNotify();

private:
    bool encode(XMLDocument *xmldocNotify);

public:
    virtual bool notify(const std::string& deviceId, const std::string& notifyType);
};

/* A.2.5.5 语音广播通知 */

/* A.2.5.6 移动设备位置数据通知 */

/* A.2.5.7 图像抓拍传输完成通知 */

/* A.2.5.8 设备实时视音频回传通知 */

/* A.2.5.9 设备软件升级结果通知 */

#endif