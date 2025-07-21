#ifndef A_2_5_NOTIFY_H
#define A_2_5_NOTIFY_H

#include <vector>
#include <memory>
#include "A.2.2CmdType.h"

/* A.2.5 通知命令 */

/* A.2.5.1 通知命令消息体 */
class NotifyRequest : public CmdTypeRequest
{
public:
    struct Notify
    {};

private:
    std::vector<std::shared_ptr<CmdTypeSpecRequest>> spec;

public:
    NotifyRequest(MANSCDPAgent *agent, Status *status);
    virtual ~NotifyRequest();

public:
    static bool encode(const Notify& notify, XMLDocument *xmldocNotify);

public:
    virtual bool match(const std::string& ReqType);
    virtual bool dispatch(const XMLElement *xmlReq);
};

/* A.2.5.2 状态信息报送 */
class KeepAliveNotify : public CmdTypeSpecRequest
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
    };

public:
    KeepAliveNotify(MANSCDPAgent *agent, Status *status);
    virtual ~KeepAliveNotify();

public:
    static bool encode(const Notify& notify, XMLDocument *xmldocNotify);

public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

/* A.2.5.3 报警通知 */

/* A.2.5.4 媒体通知 */
class MediaStatusNotify : public CmdTypeSpecRequest
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

public:
    MediaStatusNotify(MANSCDPAgent *agent, Status *status);
    virtual ~MediaStatusNotify();

public:
    static bool encode(const Notify& notify, XMLDocument *xmldocNotify);

public:
    virtual bool match(const XMLElement *xmlReq);
    virtual bool handle(const XMLElement *xmlReq);
};

/* A.2.5.5 语音广播通知 */

/* A.2.5.6 移动设备位置数据通知 */

/* A.2.5.7 图像抓拍传输完成通知 */

/* A.2.5.8 设备实时视音频回传通知 */

/* A.2.5.9 设备软件升级结果通知 */

#endif