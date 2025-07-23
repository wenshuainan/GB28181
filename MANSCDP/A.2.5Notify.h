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
public:
    struct Notify
    {};

public:
    static bool encode(const Notify& notify, XMLDocument *xmldocNotify);
};

/* A.2.5.2 状态信息报送 */
class KeepaliveNotify
{
public:
    struct Notify : NotifyRequest::Notify
    {
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
    };

private:
    MANSCDPAgent *m_agent;
    Status *m_devStatus;

public:
    KeepaliveNotify(MANSCDPAgent *agent, Status *status);
    virtual ~KeepaliveNotify();

private:
    bool encode(const Notify& notify, XMLDocument *xmldocNotify);

public:
    virtual bool handle();
};

/* A.2.5.3 报警通知 */

/* A.2.5.4 媒体通知 */
class MediaStatusNotify
{
public:
    struct Notify : NotifyRequest::Notify
    {
        /* 〈! --命令类型:媒体通知(必选)--〉 */
        stringType CmdType;
        /* 〈! --命令序列号(必选)--〉 */
        SNType SN;
        /* 〈! --媒体发送设备编码(必选)--〉 */
        deviceIDType DeviceID;
        /* 〈! --媒体发送设备编码(必选)--〉 */
        stringType NotifyType;
    };

private:
    MANSCDPAgent *m_agent;
    const SessionIdentifier& m_sessionId;

public:
    MediaStatusNotify(MANSCDPAgent *agent, const SessionIdentifier& sessionId);
    virtual ~MediaStatusNotify();

private:
    bool encode(const Notify& notify, XMLDocument *xmldocNotify);

public:
    virtual bool handle(const std::string& deviceId, const std::string& notifyType);
};

/* A.2.5.5 语音广播通知 */

/* A.2.5.6 移动设备位置数据通知 */

/* A.2.5.7 图像抓拍传输完成通知 */

/* A.2.5.8 设备实时视音频回传通知 */

/* A.2.5.9 设备软件升级结果通知 */

#endif