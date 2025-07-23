#include "A.2.3Control.h"
#include "Agent/MANSCDPAgent.h"
#include "A.2.6Response.h"

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

bool DeviceControlRequest::parse(const XMLElement *xmlReq, Request& req)
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

bool PTZCmdControl::parse(const XMLElement *xmlReq, Request& req)
{
    if (!DeviceControlRequest::parse(xmlReq, req))
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
    if (!parse(xmlReq, req))
    {
        return false;
    }

    int32_t ch = m_agent->getChNum(req.DeviceID.getStr());
    if (ch < 0)
    {
        return false;
    }

    return m_control->handle(ch, req);
}

TeleBootControl::TeleBootControl(MANSCDPAgent *agent, Control *control)
    : CmdTypeSpecRequest(agent, control)
{}

TeleBootControl::~TeleBootControl()
{}

bool TeleBootControl::parse(const XMLElement *xmlReq, Request& req)
{
    if (!DeviceControlRequest::parse(xmlReq, req))
    {
        return false;
    }

    const XMLElement *xmlTeleBoot = xmlReq->FirstChildElement("TeleBoot");
    if (xmlTeleBoot)
    {
        req.TeleBoot = xmlTeleBoot->GetText();
    }

    return true;
}

bool TeleBootControl::match(const XMLElement *xmlReq)
{
    return xmlReq->FirstChildElement("TeleBoot") != nullptr;
}

bool TeleBootControl::handle(const XMLElement *xmlReq)
{
    Request req;
    if (!parse(xmlReq, req))
    {
        return false;
    }

    int32_t ch = m_agent->getChNum(req.DeviceID.getStr());
    if (ch < 0)
    {
        return false;
    }

    return m_control->handle(ch, req);
}

RecordControl::RecordControl(MANSCDPAgent *agent, Control *control)
    : CmdTypeSpecRequest(agent, control)
{}

RecordControl::~RecordControl()
{}

bool RecordControl::parse(const XMLElement *xmlReq, Request& req)
{
    if (!DeviceControlRequest::parse(xmlReq, req))
    {
        return false;
    }

    const XMLElement *xmlRecordCmd = xmlReq->FirstChildElement("RecordCmd");
    if (xmlRecordCmd)
    {
        req.RecordCmd = xmlRecordCmd->GetText();
    }

    const XMLElement *xmlStreamNumber = xmlReq->FirstChildElement("StreamNumber");
    if (xmlStreamNumber)
    {
        req.StreamNumber = xmlStreamNumber->GetText();
    }

    return true;
}

bool RecordControl::match(const XMLElement *xmlReq)
{
    return xmlReq->FirstChildElement("RecordCmd") != nullptr;
}

bool RecordControl::handle(const XMLElement *xmlReq)
{
    Request req;
    if (!parse(xmlReq, req))
    {
        return false;
    }

    int32_t ch = m_agent->getChNum(req.DeviceID.getStr());
    if (ch < 0)
    {
        return false;
    }

    DeviceControlResponse res(req);
    res.Result = m_control->handle(ch, req);
    
    XMLDocument xmldocRes;
    if (res.encode(&xmldocRes))
    {
        return m_agent->sendResponseCmd(xmldocRes);
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
    (void) xmlReq;
    return false;
}