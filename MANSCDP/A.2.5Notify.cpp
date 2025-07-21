#include "A.2.5Notify.h"
#include "Agent/MANSCDPAgent.h"

NotifyRequest::NotifyRequest(MANSCDPAgent *agent, Status *status)
{
    spec.push_back(std::make_shared<KeepAliveNotify>(agent, status));
    spec.push_back(std::make_shared<MediaStatusNotify>(agent, status));
}

NotifyRequest::~NotifyRequest()
{
    spec.clear();
}

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

bool NotifyRequest::match(const std::string& ReqType)
{
    return ReqType == "Notify";
}

bool NotifyRequest::dispatch(const XMLElement *xmlReq)
{
    for (auto i : spec)
    {
        if (i->match(xmlReq))
        {
            return i->handle(xmlReq);
        }
    }

    return false;
}

KeepAliveNotify::KeepAliveNotify(MANSCDPAgent *agent, Status *status)
    : CmdTypeSpecRequest(agent, status)
{}

KeepAliveNotify::~KeepAliveNotify()
{}

bool KeepAliveNotify::encode(const Notify& notify, XMLDocument *xmldocNotify)
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

    return true;
}

bool KeepAliveNotify::match(const XMLElement *xmlReq)
{
    (void)xmlReq;
    return false;
}

bool KeepAliveNotify::handle(const XMLElement *xmlReq)
{
    (void) xmlReq;
    return false;
}

MediaStatusNotify::MediaStatusNotify(MANSCDPAgent *agent, Status *status)
    : CmdTypeSpecRequest(agent, status)
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

bool MediaStatusNotify::match(const XMLElement *xmlReq)
{
    (void)xmlReq;
    return false;
}

bool MediaStatusNotify::handle(const XMLElement *xmlReq)
{
    (void)xmlReq;
    return false;
}