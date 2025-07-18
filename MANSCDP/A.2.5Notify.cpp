#include "A.2.5Notify.h"
#include "Agent/MANSCDPAgent.h"

NotifyRequest::NotifyRequest(MANSCDPAgent *agent, Status *status)
{
    spec.push_back(std::make_shared<KeepAliveNotify>(agent, status));
}

NotifyRequest::~NotifyRequest()
{
    spec.clear();
}

bool NotifyRequest::encode(const Request& req, XMLDocument *xmldocReq)
{
    (void) req;
    XMLDeclaration *dec = xmldocReq->NewDeclaration("xml version=\"1.0\"");
    if (dec != nullptr)
    {
        xmldocReq->InsertFirstChild(dec);
    }

    XMLElement *rootElement = xmldocReq->NewElement("Notify");
    if (rootElement != nullptr)
    {
        return xmldocReq->InsertEndChild(rootElement) != nullptr;
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

bool KeepAliveNotify::encode(const Request& req, XMLDocument *xmldocReq)
{
    if (!NotifyRequest::encode(req, xmldocReq))
    {
        return false;
    }

    XMLElement *rootElement = xmldocReq->LastChildElement();
    if (rootElement == nullptr)
    {
        return false;
    }

    XMLElement *xmlCmdType = xmldocReq->NewElement("CmdType");
    xmlCmdType->SetText("Keepalive");
    rootElement->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = xmldocReq->NewElement("SN");
    xmlSN->SetText(req.SN.getValue());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocReq->NewElement("DeviceID");
    xmlDeviceID->SetText(req.DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlStatus = xmldocReq->NewElement("Status");
    xmlStatus->SetText(req.Status.getStr().c_str());
    rootElement->InsertEndChild(xmlStatus);

    return true;
}

bool KeepAliveNotify::match(const XMLElement *xmlReq)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (xmlCmdType == nullptr)
    {
        return false;
    }
    if (xmlCmdType->GetText() != nullptr)
    {
        std::string CmdType = xmlCmdType->GetText();
        return CmdType == "Keepalive";
    }
    else
    {
        return false;
    }
}

bool KeepAliveNotify::handle(const XMLElement *xmlReq)
{
    (void) xmlReq;
    return false;
}
