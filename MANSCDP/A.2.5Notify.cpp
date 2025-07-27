#include "A.2.5Notify.h"
#include "Agent/MANSCDPAgent.h"

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
        return false;
    }
}

KeepaliveNotify::KeepaliveNotify(MANSCDPAgent *agent, class Status *status)
    : m_agent(agent), m_devStatus(status)
{}

KeepaliveNotify::~KeepaliveNotify()
{}

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
    xmlSN->SetText(SN.getValue());
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

bool KeepaliveNotify::notify()
{
    CmdType = "Keepalive";
    SN = "22";
    DeviceID = m_agent->getMainDeviceId();
    Status = "OK";

    const std::unordered_map<std::string, int32_t>& channels = m_agent->getChannels();
    for (auto i : channels)
    {
        if (!m_devStatus->getStatus(i.second))
        {
            Info.DeviceID.push_back(i.first);
        }
    }

    XMLDocument xmldocNotify;
    if (encode(&xmldocNotify))
    {
        if (m_agent->sendKeepaliveNotify(xmldocNotify))
        {
            m_devStatus->addSentCount();
            return true;
        }
    }
    return false;
}

AlarmNotify::AlarmNotify(MANSCDPAgent *agent)
    : m_agent(agent)
{}

AlarmNotify::~AlarmNotify()
{}

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
    xmlSN->SetText(SN.getValue());
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

    XMLElement *xmlAlarmType = xmldocNotify->NewElement("AlarmType");
    xmlAlarmType->SetText(Info.AlarmType.getStr().c_str());
    xmlInfo->InsertEndChild(xmlAlarmType);

    return true;
}

bool AlarmNotify::notify(int32_t ch, int32_t method, int32_t type, time_t alarmTime)
{
    CmdType = "Alarm";
    SN = "22";
    DeviceID = m_agent->getDeviceId(ch);
    AlarmPriority = 4;
    AlarmMethod = method;
    AlarmTime = alarmTime;
    Info.AlarmType = type;

    XMLDocument xmldocNotify;
    if (encode(&xmldocNotify))
    {
        return m_agent->sendAlarmNotify(xmldocNotify);
    }
    return false;
}

MediaStatusNotify::MediaStatusNotify(MANSCDPAgent *agent, const SessionIdentifier& sessionId)
    : m_agent(agent), m_sessionId(sessionId)
{}

MediaStatusNotify::~MediaStatusNotify()
{}

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
    xmlSN->SetText(SN.getValue());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocNotify->NewElement("DeviceID");
    xmlDeviceID->SetText(DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlNotifyType = xmldocNotify->NewElement("NotifyType");
    xmlNotifyType->SetText(NotifyType.getStr().c_str());
    rootElement->InsertEndChild(xmlNotifyType);

    return true;
}

bool MediaStatusNotify::notify(const std::string& deviceId, const std::string& notifyType)
{
    CmdType = "MediaStatus";
    SN = "22";
    DeviceID = deviceId;
    NotifyType = notifyType;

    XMLDocument xmldocNotify;
    if (encode(&xmldocNotify))
    {
        return m_agent->sendMediaStatusNotify(m_sessionId, xmldocNotify);
    }
    return false;
}