#include "A.2.5Notify.h"
#include "Agent/MANSCDPAgent.h"

bool NotifyRequest::encode(const Notify& notify, XMLDocument *xmldocNotify)
{
    (void) notify;
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

KeepaliveNotify::KeepaliveNotify(MANSCDPAgent *agent, Status *status)
    : m_agent(agent), m_devStatus(status)
{}

KeepaliveNotify::~KeepaliveNotify()
{}

bool KeepaliveNotify::encode(const Notify& notify, XMLDocument *xmldocNotify)
{
    if (!NotifyRequest::encode(notify, xmldocNotify))
    {
        return false;
    }

    XMLElement *rootElement = xmldocNotify->LastChildElement();
    if (rootElement == nullptr)
    {
        return false;
    }

    XMLElement *xmlCmdType = xmldocNotify->NewElement("CmdType");
    xmlCmdType->SetText("Keepalive");
    rootElement->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = xmldocNotify->NewElement("SN");
    xmlSN->SetText(notify.SN.getValue());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocNotify->NewElement("DeviceID");
    xmlDeviceID->SetText(notify.DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlStatus = xmldocNotify->NewElement("Status");
    xmlStatus->SetText(notify.Status.getStr().c_str());
    rootElement->InsertEndChild(xmlStatus);

    XMLElement *xmlInfo = xmldocNotify->NewElement("Info");
    rootElement->InsertEndChild(xmlInfo);

    auto size = notify.Info.DeviceID.size();
    for (int i = 0; (std::size_t)i < size; i++)
    {
        XMLElement *xmlDeviceID = xmldocNotify->NewElement("DeviceID");
        xmlDeviceID->SetText(notify.Info.DeviceID[i].getStr().c_str());
        xmlInfo->InsertEndChild(xmlDeviceID);
    }

    return true;
}

bool KeepaliveNotify::handle()
{
    Notify notify;
    notify.CmdType = "Keepalive";
    notify.SN = "22";
    notify.DeviceID = m_agent->getDeviceID();
    notify.Status = "OK";

    const std::unordered_map<std::string, int32_t>& channels = m_agent->getChannels();
    for (auto i : channels)
    {
        if (!m_devStatus->getStatus(i.second))
        {
            notify.Info.DeviceID.push_back(i.first);
        }
    }

    XMLDocument xmldocNotify;
    if (encode(notify, &xmldocNotify))
    {
        if (m_agent->sendKeepaliveNotify(xmldocNotify))
        {
            m_devStatus->addSentCount();
            return true;
        }
    }
    return false;
}

MediaStatusNotify::MediaStatusNotify(MANSCDPAgent *agent, const SessionIdentifier& sessionId)
    : m_agent(agent), m_sessionId(sessionId)
{}

MediaStatusNotify::~MediaStatusNotify()
{}

bool MediaStatusNotify::encode(const Notify& notify, XMLDocument *xmldocNotify)
{
    if (!NotifyRequest::encode(notify, xmldocNotify))
    {
        return false;
    }

    XMLElement *rootElement = xmldocNotify->LastChildElement();
    if (rootElement == nullptr)
    {
        return false;
    }

    XMLElement *xmlCmdType = xmldocNotify->NewElement("CmdType");
    xmlCmdType->SetText("MediaStatus");
    rootElement->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = xmldocNotify->NewElement("SN");
    xmlSN->SetText(notify.SN.getValue());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocNotify->NewElement("DeviceID");
    xmlDeviceID->SetText(notify.DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlNotifyType = xmldocNotify->NewElement("NotifyType");
    xmlNotifyType->SetText(notify.NotifyType.getStr().c_str());
    rootElement->InsertEndChild(xmlNotifyType);

    return true;
}

bool MediaStatusNotify::handle(const std::string& deviceId, const std::string& notifyType)
{
    Notify notify;
    notify.CmdType = "MediaStatus";
    notify.SN = "22";
    notify.DeviceID = deviceId;
    notify.NotifyType = notifyType;

    XMLDocument xmldocNotify;
    if (encode(notify, &xmldocNotify))
    {
        return m_agent->sendMediaStatusNotify(m_sessionId, xmldocNotify);
    }
    return false;
}