#ifndef CMD_TYPE_H
#define CMD_TYPE_H

#include "A.2.1GlobalTypes.h"

class MANSCDPAgent;
class Control;
class Query;
class Status;
class RecordQuery;

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
    Control *m_control;
    Query *m_query;
    Status *m_status;
    RecordQuery *m_recordQuery;

public:
    CmdTypeSpecRequest(MANSCDPAgent *agent, Control *control)
        : m_agent(agent)
        , m_control(control)
        , m_query(nullptr)
        , m_status(nullptr)
    {}
    CmdTypeSpecRequest(MANSCDPAgent *agent, Query *query)
        : m_agent(agent)
        , m_control(nullptr)
        , m_query(query)
        , m_status(nullptr)
    {}
    CmdTypeSpecRequest(MANSCDPAgent *agent, Status *status)
        : m_agent(agent)
        , m_control(nullptr)
        , m_query(nullptr)
        , m_status(status)
    {}
    CmdTypeSpecRequest(MANSCDPAgent *agent, RecordQuery *recordQuery)
        : m_agent(agent)
        , m_control(nullptr)
        , m_query(nullptr)
        , m_status(nullptr)
        , m_recordQuery(recordQuery)
    {}
    virtual ~CmdTypeSpecRequest() {}

public:
    virtual bool match(const XMLElement *xmlReq) = 0;
    virtual bool handle(const XMLElement *xmlReq) = 0;
};

#endif