#include "A.2.5Notify.h"
#include "MANSCDPAgent.h"

NotifyRequest::NotifyRequest(MANSCDPAgent *agent)
{
    spec.push_back(std::make_shared<KeepAliveNotify>(agent));
}

NotifyRequest::~NotifyRequest()
{
    spec.clear();
}

bool NotifyRequest::serialize(const Request& req, XMLDocument *xmldocReq)
{
    XMLElement *rootElement = xmldocRes->NewElement("Notify");
    if (rootElement != nullptr)
    {
        return xmldocRes->InsertEndChild(rootElement) != nullptr;
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
}

KeepAliveNotify::KeepAliveNotify(MANSCDPAgent *agent)
    : CmdTypeSpecRequest(agent)
{}

KeepAliveNotify::~KeepAliveNotify()
{}

bool KeepAliveNotify::serialize(const Request& req, XMLDocument *xmldocReq)
{
    if (!NotifyRequest::serialize(req, xmldocReq))
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
    XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (xmlCmdType == nullptr)
    {
        return false;
    }
    if (xmlCmdType->GetText() != nullptr)
    {
        return xmlCmdType->GetText() == "Keepalive";
    }
    else
    {
        return false;
    }
}

bool KeepAliveNotify::handle(const XMLElement *xmlReq)
{
    m_agent->sendRequest(xmlReq);
}

bool KeepAliveNotify::handle(int32_t code)
{
    m_status;
}