#ifndef CMD_TYPE_H
#define CMD_TYPE_H

#include "A.2.1GlobalTypes.h"

class MANSCDPAgent;

/*  A.2.2.1 请求命令结构头文件定义
   〈elementref="tg:Control"/〉
   〈elementref="tg:Query"/〉
   〈elementref="tg:Notify"/〉 
*/
class CmdTypeRequest
{
protected:
    MANSCDPAgent *m_agent;

public:
    CmdTypeRequest(MANSCDPAgent *agent) : m_agent(agent) {}
    virtual ~CmdTypeRequest() {}

public:
    virtual bool match(const std::string& name) = 0;
    virtual bool dispatch(XMLElement *xml) = 0;
};

/* A.2.3 控制命令
    |-A.2.3.1 设备控制命令 DeviceControl
    |-A.2.3.2 设备配置命令 DeviceConfig
*/
class CmdTypeControl
{
protected:
    MANSCDPAgent *m_agent;

public:
    CmdTypeControl(MANSCDPAgent *agent) : m_agent(agent) {}
    virtual ~CmdTypeControl() {}

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
class CmdTypesSpecRequest
{
protected:
    MANSCDPAgent *m_agent;

public:
    CmdTypesSpecRequest(MANSCDPAgent *agent) : m_agent(agent) {}
    virtual ~CmdTypesSpecRequest() {}
    
public:
    virtual bool match(XMLElement *xml) = 0;
    virtual bool dispatch(XMLElement *xml) = 0;
};

/* A.2.2.2 应答命令结构头文件定义 */
class CmdTypeResponse
{};

class CmdTypeSpecResponse
{};

#endif