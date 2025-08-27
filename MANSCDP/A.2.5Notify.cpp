#include "A.2.5Notify.h"
#include "Agent/MANSCDPAgent.h"
#include "Agent/SessionAgent.h"
#include "Interface/9.6Status.h"
#include "MANSCDPDevice.h"

bool NotifyRequest::encode(XMLDocument *xmldocNotify)
{
    XMLDeclaration *dec = xmldocNotify->NewDeclaration("xml version=\"1.0\"");
    if (dec != nullptr)
    {
        xmldocNotify->InsertFirstChild(dec);
    }

    XMLElement *rootElement = xmldocNotify->NewElement("Notify");
    if (rootElement != nullptr)
    {
        return xmldocNotify->InsertEndChild(rootElement) != nullptr;
    }
    else
    {
        printf("xml new <Notify> element failed\n");
        return false;
    }
}

bool NotifyRequest::match(const XMLElement *cmd)
{
    const char *name = cmd->Name();
    return name && strcmp(name, "Notify") == 0;
}

SNType KeepaliveNotify::SN;

KeepaliveNotify::KeepaliveNotify(MANSCDPAgent *agent, class Status *devStatus)
    : MessageResultHandler(agent)
    , m_devStatus(devStatus)
{
    printf("++++++ KeepaliveNotify\n");
    CmdType = "Keepalive";
    SN++;
    DeviceID = dynamic_cast<MANSCDPDevice*>(m_devStatus)->getId();
    Status = "OK";
}

KeepaliveNotify::~KeepaliveNotify()
{
    printf("------ KeepaliveNotify\n");
}

bool KeepaliveNotify::encode(XMLDocument *xmldocNotify)
{
    if (!NotifyRequest::encode(xmldocNotify))
    {
        return false;
    }

    XMLElement *rootElement = xmldocNotify->LastChildElement();
    if (rootElement == nullptr)
    {
        return false;
    }

    XMLElement *xmlCmdType = xmldocNotify->NewElement("CmdType");
    xmlCmdType->SetText(CmdType.getStr().c_str());
    rootElement->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = xmldocNotify->NewElement("SN");
    xmlSN->SetText(SN.getInt());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocNotify->NewElement("DeviceID");
    xmlDeviceID->SetText(DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlStatus = xmldocNotify->NewElement("Status");
    xmlStatus->SetText(Status.getStr().c_str());
    rootElement->InsertEndChild(xmlStatus);

    XMLElement *xmlInfo = xmldocNotify->NewElement("Info");
    rootElement->InsertEndChild(xmlInfo);

    auto size = Info.DeviceID.size();
    for (int i = 0; (std::size_t)i < size; i++)
    {
        XMLElement *xmlDeviceID = xmldocNotify->NewElement("DeviceID");
        xmlDeviceID->SetText(Info.DeviceID[i].getStr().c_str());
        xmlInfo->InsertEndChild(xmlDeviceID);
    }

    return true;
}

bool KeepaliveNotify::notify(std::shared_ptr<MessageResultHandler> handler)
{
    MANSCDPDevice *dev = dynamic_cast<MANSCDPDevice*>(m_devStatus);
    if (!dev)
    {
        return false;
    }

    std::vector<std::string> offDevices;
    dev->getStatus(offDevices);
    for (auto& i : offDevices)
    {
        Info.DeviceID.push_back(i);
    }

    XMLDocument xmldocNotify;
    if (encode(&xmldocNotify))
    {
        return m_agent->sendCmd(xmldocNotify, handler);
    }
    return false;
}

bool KeepaliveNotify::match(const XMLElement *cmd)
{
    if (!NotifyRequest::match(cmd))
    {
        return false;
    }

    const XMLElement *xmlCmdType = cmd->FirstChildElement("CmdType");
    const XMLElement *xmlSN = cmd->FirstChildElement("SN");
    return xmlCmdType && this->CmdType == xmlCmdType->GetText()
        && xmlSN && this->SN == xmlSN->IntText();
}

bool KeepaliveNotify::handle(int32_t code)
{
    printf("KeepaliveNotify::handle code=%d\n", code);
    if (code == 200)
    {
        m_devStatus->addRecvedCount();
    }
    return true;
}

SNType AlarmNotify::SN;

AlarmNotify::AlarmNotify(MANSCDPAgent *agent, Alarm *devAlarm)
    : MessageResultHandler(agent)
    , m_devAlarm(devAlarm)
{
    printf("++++++ AlarmNotify\n");
    CmdType = "Alarm";
    SN++;
    AlarmPriority = 4;
}

AlarmNotify::~AlarmNotify()
{
    printf("------ AlarmNotify\n");
}

bool AlarmNotify::encode(XMLDocument *xmldocNotify)
{
    if (!NotifyRequest::encode(xmldocNotify))
    {
        return false;
    }

    XMLElement *rootElement = xmldocNotify->LastChildElement();
    if (rootElement == nullptr)
    {
        return false;
    }

    XMLElement *xmlCmdType = xmldocNotify->NewElement("CmdType");
    xmlCmdType->SetText(CmdType.getStr().c_str());
    rootElement->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = xmldocNotify->NewElement("SN");
    xmlSN->SetText(SN.getInt());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocNotify->NewElement("DeviceID");
    xmlDeviceID->SetText(DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlAlarmPriority = xmldocNotify->NewElement("AlarmPriority");
    xmlAlarmPriority->SetText(AlarmPriority.getStr().c_str());
    rootElement->InsertEndChild(xmlAlarmPriority);

    XMLElement *xmlAlarmMethod = xmldocNotify->NewElement("AlarmMethod");
    xmlAlarmMethod->SetText(AlarmMethod.getStr().c_str());
    rootElement->InsertEndChild(xmlAlarmMethod);

    XMLElement *xmlAlarmTime = xmldocNotify->NewElement("AlarmTime");
    xmlAlarmTime->SetText(AlarmTime.getStr().c_str());
    rootElement->InsertEndChild(xmlAlarmTime);

    XMLElement *xmlInfo = xmldocNotify->NewElement("Info");
    rootElement->InsertEndChild(xmlInfo);

    if (Info.AlarmType.isValid())
    {
        XMLElement *xmlAlarmType = xmldocNotify->NewElement("AlarmType");
        xmlAlarmType->SetText(Info.AlarmType.getStr().c_str());
        xmlInfo->InsertEndChild(xmlAlarmType);
    }

    XMLElement *xmlAlarmTypeParam = xmldocNotify->NewElement("AlarmTypeParam");
    xmlInfo->InsertEndChild(xmlAlarmTypeParam);

    if (Info.AlarmTypeParam.EventType.isValid())
    {
        XMLElement *xmlEventType = xmldocNotify->NewElement("EventType");
        xmlEventType->SetText(Info.AlarmTypeParam.EventType.getStr().c_str());
        xmlAlarmTypeParam->InsertEndChild(xmlEventType);
    }

    return true;
}

bool AlarmNotify::notify(const std::string& deviceId, std::shared_ptr<MessageResultHandler> handler)
{
    DeviceID = deviceId;

    XMLDocument xmldocNotify;
    if (encode(&xmldocNotify))
    {
        return m_agent->sendCmd(xmldocNotify, handler);
    }
    return false;
}

bool AlarmNotify::match(const XMLElement *cmd)
{
    if (!NotifyRequest::match(cmd))
    {
        return false;
    }
    
    const XMLElement *xmlCmdType = cmd->FirstChildElement("CmdType");
    const XMLElement *xmlSN = cmd->FirstChildElement("SN");
    return xmlCmdType && this->CmdType == xmlCmdType->GetText()
        && xmlSN && this->SN == xmlSN->IntText();
}

bool AlarmNotify::handle(int32_t code)
{
    return true;
}

SNType MediaStatusNotify::SN;

MediaStatusNotify::MediaStatusNotify(SessionAgent *agent, const SessionIdentifier& sessionId)
    : MessageResultHandler(agent)
    , m_sessionId(sessionId)
{
    printf("++++++ MediaStatusNotify\n");
    CmdType = "MediaStatus";
    SN++;
}

MediaStatusNotify::~MediaStatusNotify()
{
    printf("------ MediaStatusNotify\n");
}

bool MediaStatusNotify::encode(XMLDocument *xmldocNotify)
{
    if (!NotifyRequest::encode(xmldocNotify))
    {
        return false;
    }

    XMLElement *rootElement = xmldocNotify->LastChildElement();
    if (rootElement == nullptr)
    {
        return false;
    }

    XMLElement *xmlCmdType = xmldocNotify->NewElement("CmdType");
    xmlCmdType->SetText(CmdType.getStr().c_str());
    rootElement->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = xmldocNotify->NewElement("SN");
    xmlSN->SetText(SN.getInt());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocNotify->NewElement("DeviceID");
    xmlDeviceID->SetText(DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlNotifyType = xmldocNotify->NewElement("NotifyType");
    xmlNotifyType->SetText(NotifyType.getStr().c_str());
    rootElement->InsertEndChild(xmlNotifyType);

    return true;
}

bool MediaStatusNotify::notify(const std::string& deviceId, const std::string& notifyType, std::shared_ptr<MessageResultHandler> handler)
{
    DeviceID = deviceId;
    NotifyType = notifyType;

    XMLDocument xmldocNotify;
    if (encode(&xmldocNotify))
    {
        return m_sessionAgent->sendMediaStatus(m_sessionId, xmldocNotify);
    }
    return false;
}

bool MediaStatusNotify::match(const XMLElement *cmd)
{
    if (!NotifyRequest::match(cmd))
    {
        return false;
    }
    
    const XMLElement *xmlCmdType = cmd->FirstChildElement("CmdType");
    const XMLElement *xmlSN = cmd->FirstChildElement("SN");
    return xmlCmdType && this->CmdType == xmlCmdType->GetText()
        && xmlSN && this->SN == xmlSN->IntText();
}

bool MediaStatusNotify::handle(int32_t code)
{
    return true;
}