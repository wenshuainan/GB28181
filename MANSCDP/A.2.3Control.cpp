#include "A.2.3Control.h"
#include "MANSCDPAgent.h"

ControlReuest::ControlReuest(MANSCDPAgent *agent, Control *control)
{
    spec.push_back(std::make_shared<DeviceControlRequest>(agent, control));
    spec.push_back(std::make_shared<DeviceConfigRequest>(agent, control));
}

ControlReuest::~ControlReuest()
{
    spec.clear();
}

bool ControlReuest::match(const std::string& ReqType)
{
    return ReqType == "Control";
}

bool ControlReuest::dispatch(const XMLElement *xmlReq)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (!xmlCmdType)
    {
        return false;
    }

    std::string CmdType = xmlCmdType->GetText();
    for (auto& i : spec)
    {
        if (i->match(CmdType))
        {
            return i->dispatch(xmlReq);
        }
    }

    return false;
}

DeviceControlRequest::DeviceControlRequest(MANSCDPAgent *agent, Control *control)
{
    spec.push_back(std::make_shared<PTZCmdControl>(agent, control));
}

DeviceControlRequest::~DeviceControlRequest()
{
    spec.clear();
}

bool DeviceControlRequest::deserialize(const XMLElement *xmlReq, Request& req)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (!xmlCmdType)
    {
        return false;
    }

    const XMLElement *xmlSN = xmlReq->FirstChildElement("SN");
    if (!xmlSN)
    {
        return false;
    }

    const XMLElement *xmlDeviceID = xmlReq->FirstChildElement("DeviceID");
    if (!xmlDeviceID)
    {
        return false;
    }

    req.CmdType = xmlCmdType->GetText();
    req.SN = xmlSN->GetText();
    req.DeviceID = xmlDeviceID->GetText();

    return true;
}

bool DeviceControlRequest::match(const std::string& CmdType)
{
    return CmdType == "DeviceControl";
}

bool DeviceControlRequest::dispatch(const XMLElement *xmlReq)
{
    for (auto& i : spec)
    {
        if (i->match(xmlReq))
        {
            return i->handle(xmlReq);
        }
    }

    return false;
}

PTZCmdControl::PTZCmdControl(MANSCDPAgent *agent, Control *control)
    : CmdTypeSpecRequest(agent, control)
{}

PTZCmdControl::~PTZCmdControl()
{}

bool PTZCmdControl::deserialize(const XMLElement *xmlReq, Request& req)
{
    if (!DeviceControlRequest::deserialize(xmlReq, req))
    {
        return false;
    }

    const XMLElement *xmlPTZCmd = xmlReq->FirstChildElement("PTZCmd");
    if (xmlPTZCmd)
    {
        req.PTZCmd = xmlPTZCmd->GetText();
    }

    const XMLElement *xmlPTZCmdParams = xmlReq->FirstChildElement("PTZCmdParams");
    if (xmlPTZCmdParams)
    {
        const XMLElement *xmlPresetName = xmlPTZCmdParams->FirstChildElement("PresetName");
        const XMLElement *xmlCruiseTrackName = xmlPTZCmdParams->FirstChildElement("CruiseTrackName");

        if (xmlPresetName)
        {
            req.PTZCmdParams.PresetName = xmlPresetName->GetText();
        }

        if (xmlCruiseTrackName)
        {
            req.PTZCmdParams.CruiseTrackName = xmlCruiseTrackName->GetText();
        }
    }

    return true;
}

bool PTZCmdControl::match(const XMLElement *xmlReq)
{
    return xmlReq->FirstChildElement("PTZCmd") != nullptr;
}

bool PTZCmdControl::handle(const XMLElement *xmlReq)
{
    Request req;
    if (!deserialize(xmlReq, req))
    {
        return false;
    }

    DeviceControlResponse::Response res;
    if (m_control->process(req, res))
    {
        XMLDocument doc;
        DeviceControlResponse::serialize(res, &doc);
        return m_agent->sendResponse(doc);
    }

    return false;
}

DeviceConfigRequest::DeviceConfigRequest(MANSCDPAgent *agent, Control *control)
{}

DeviceConfigRequest::~DeviceConfigRequest()
{
    spec.clear();
}

bool DeviceConfigRequest::match(const std::string& CmdType)
{
    return CmdType == "DeviceConfig";
}

bool DeviceConfigRequest::dispatch(const XMLElement *xmlReq)
{
    return false;
}