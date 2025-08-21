#ifndef CMD_TYPE_H
#define CMD_TYPE_H

#include "A.2.1GlobalTypes.h"

class MANSCDPAgent;
class SessionAgent;

/*  A.2.2.1 请求命令结构头文件定义
   〈elementref="tg:Control"/〉
   〈elementref="tg:Query"/〉
   〈elementref="tg:Notify"/〉 
*/
class CmdTypeRequest
{
public:
    CmdTypeRequest() {}
    virtual ~CmdTypeRequest() {}

public:
    virtual bool match(const std::string& ReqType) = 0;
    virtual bool dispatch(const XMLElement *xmlReq) = 0;
};

/* A.2.3 控制命令
    |-A.2.3.1 设备控制命令 DeviceControl
    |-A.2.3.2 设备配置命令 DeviceConfig
*/
class CmdTypeControl
{
public:
    CmdTypeControl() {}
    virtual ~CmdTypeControl() {}

public:
    virtual bool match(const std::string& CmdType) = 0;
    virtual bool dispatch(const XMLElement *xmlReq) = 0;
};

/*
   A.2.3.1.2~A.2.3.1.14:
   PTZCmd
   TeleBoot
   ...

   A.2.3.2.2~A.2.3.2.13
   BasicParam
   SVACEncOdeCOnfig
   ...

   A.2.4.2~A.2.4.14
   DeviceStatus
   Catalog
   ...
*/
class CmdTypeSpecRequest
{
protected:
    MANSCDPAgent *m_agent;

public:
    CmdTypeSpecRequest(MANSCDPAgent *agent) : m_agent(agent) {}
    virtual ~CmdTypeSpecRequest() {}

public:
    virtual bool match(const XMLElement *xmlReq) = 0;
    virtual bool handle(const XMLElement *xmlReq) = 0;
};

/* 消息响应结果处理器
 * 例如心跳保活：是否回复了200 OK
 * 例如多响应消息，文件目录应答回复了200 OK之后再传送下一条消息
 */
class MessageResultHandler
{
protected:
    MANSCDPAgent *m_agent;
    SessionAgent *m_sessionAgent; //会话内也会发送MANSCDP消息，回放、下载文件结束

public:
    MessageResultHandler(MANSCDPAgent *agent)
        : m_agent(agent)
        , m_sessionAgent(nullptr)
    {}
    MessageResultHandler(SessionAgent *sessionAgent)
        : m_agent(nullptr)
        , m_sessionAgent(sessionAgent)
    {}
    virtual ~MessageResultHandler() {}

public:
    virtual bool match(const XMLElement *cmd) = 0;
    virtual bool handle(int32_t code) = 0;
};

#endif