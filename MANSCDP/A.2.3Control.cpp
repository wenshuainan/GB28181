#include "A.2.3Control.h"
#include "MANSCDPAgent.h"

ControlReuest::ControlReuest(MANSCDPAgent *agent) : CmdTypeRequest(agent)
{
    element.push_back(new DeviceControlRequest(agent));
    element.push_back(new DeviceConfigRequest(agent));
}

ControlReuest::~ControlReuest()
{
    for (auto& i : element)
    {
        delete i;
    }
}

bool ControlReuest::match(const std::string& name)
{
    return name == "Control";
}

bool ControlReuest::dispatch(XMLElement *xml)
{
    XMLElement *xmlCmdType = xml->FirstChildElement("CmdType");
    if (!xmlCmdType)
    {
        return false;
    }

    std::string CmdType = xmlCmdType->GetText();
    for (auto& i : element)
    {
        if (i->match(CmdType))
        {
            return i->dispatch(xml);
        }
    }

    return false;
}

DeviceControlRequest::DeviceControlRequest(MANSCDPAgent *agent) : CmdTypeControl(agent)
{
    element.push_back(new PTZCmdRequest(agent));
}

DeviceControlRequest::~DeviceControlRequest()
{
    for (auto& i : element)
    {
        delete i;
    }
}

bool DeviceControlRequest::deserialize(const XMLElement *xml, Request& request)
{
    const XMLElement *xmlCmdType = xml->FirstChildElement("CmdType");
    if (!xmlCmdType)
    {
        return false;
    }

    const XMLElement *xmlSN = xml->FirstChildElement("SN");
    if (!xmlSN)
    {
        return false;
    }

    const XMLElement *xmlDeviceID = xml->FirstChildElement("DeviceID");
    if (!xmlDeviceID)
    {
        return false;
    }

    request.CmdType = xmlCmdType->GetText();
    request.SN = xmlSN->GetText();
    request.DeviceID = xmlDeviceID->GetText();

    return true;
}

bool DeviceControlRequest::match(const std::string& name)
{
    return name == "DeviceControl";
}

bool DeviceControlRequest::dispatch(XMLElement *xml)
{
    for (auto& i : element)
    {
        if (i->match(xml))
        {
            return i->dispatch(xml);
        }
    }

    return false;
}

PTZCmdRequest::PTZCmdRequest(MANSCDPAgent *agent) : CmdTypesSpecRequest(agent)
{}

PTZCmdRequest::~PTZCmdRequest()
{}

bool PTZCmdRequest::deserialize(const XMLElement *xml, Request& request)
{
    if (!DeviceControlRequest::deserialize(xml, request))
    {
        return false;
    }

    const XMLElement *xmlPTZCmd = xml->FirstChildElement("PTZCmd");
    if (xmlPTZCmd)
    {
        request.PTZCmd = xmlPTZCmd->GetText();
    }

    const XMLElement *xmlPTZCmdParams = xml->FirstChildElement("PTZCmdParams");
    if (xmlPTZCmdParams)
    {
        const XMLElement *xmlPresetName = xmlPTZCmdParams->FirstChildElement("PresetName");
        const XMLElement *xmlCruiseTrackName = xmlPTZCmdParams->FirstChildElement("CruiseTrackName");

        if (xmlPresetName)
        {
            request.PTZCmdParams.PresetName = xmlPresetName->GetText();
        }

        if (xmlCruiseTrackName)
        {
            request.PTZCmdParams.CruiseTrackName = xmlCruiseTrackName->GetText();
        }
    }

    return true;
}

bool PTZCmdRequest::match(XMLElement *xml)
{
    return xml->FirstChildElement("PTZCmd") != nullptr;
}

bool PTZCmdRequest::dispatch(XMLElement *xml)
{
    Request req;

    if (!deserialize(xml, req))
    {
        return false;
    }

    return m_agent->agentControl(req);
}

DeviceConfigRequest::DeviceConfigRequest(MANSCDPAgent *agent) : CmdTypeControl(agent)
{}

DeviceConfigRequest::~DeviceConfigRequest()
{}

bool DeviceConfigRequest::match(const std::string& name)
{
    return name == "DeviceConfig";
}

bool DeviceConfigRequest::dispatch(XMLElement *xml)
{
    return false;
}