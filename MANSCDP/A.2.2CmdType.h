#ifndef CMD_TYPE_H
#define CMD_TYPE_H

#include "A.2.1GlobalTypes.h"

class MANSCDPAgent;

/*  A.2.2.1 请求命令结构头文件定义
   〈elementref="tg:Control"/〉
   〈elementref="tg:Query"/〉
   〈elementref="tg:Notify"/〉 
*/
class RequestCmdType
{
protected:
    MANSCDPAgent *m_agent;

public:
    RequestCmdType(MANSCDPAgent *agent) : m_agent(agent) {}
    virtual ~RequestCmdType() {}

public:
    virtual bool match(const std::string& name) = 0;
    virtual bool dispatch(XMLElement *xml) = 0;
};

/* A.2.3 控制命令
    |-A.2.3.1 设备控制命令 DeviceControl
    |-A.2.3.2 设备配置命令 DeviceConfig
*/
class RequestControlCmdType
{
protected:
    MANSCDPAgent *m_agent;

public:
    RequestControlCmdType(MANSCDPAgent *agent) : m_agent(agent) {}
    virtual ~RequestControlCmdType() {}

public:
    virtual bool match(const std::string& name) = 0;
    virtual bool dispatch(XMLElement *xml) = 0;
};

/*
   A.2.3.1.2~A.2.3.1.14:
   PTZCmd
   TeleBoot
   ...
*/
class RequestSpecCmdTypes
{
protected:
    MANSCDPAgent *m_agent;

public:
    RequestSpecCmdTypes(MANSCDPAgent *agent) : m_agent(agent) {}
    virtual ~RequestSpecCmdTypes() {}
    
public:
    virtual bool match(XMLElement *xml) = 0;
    virtual bool dispatch(XMLElement *xml) = 0;
};

/* A.2.2.2 应答命令结构头文件定义 */
class ResponseCmdType
{};

class ResponseSpecCmdType
{};

#endif