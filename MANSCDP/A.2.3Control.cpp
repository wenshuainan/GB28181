#include "A.2.3Control.h"
#include "MANSCDPAgent.h"

ReuestControl::ReuestControl(MANSCDPAgent *agent) : RequestCmdType(agent)
{
    element.push_back(new RequestDeviceControl(agent));
    element.push_back(new RequestDeviceConfig(agent));
}

ReuestControl::~ReuestControl()
{
    for (auto& i : element)
    {
        delete i;
    }
}

bool ReuestControl::match(const std::string& name)
{
    return name == "Control";
}

bool ReuestControl::dispatch(XMLElement *xml)
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

RequestDeviceControl::RequestDeviceControl(MANSCDPAgent *agent) : RequestControlCmdType(agent)
{
    element.push_back(new RequestPTZCmd(agent));
}

RequestDeviceControl::~RequestDeviceControl()
{
    for (auto& i : element)
    {
        delete i;
    }
}

bool RequestDeviceControl::deserialize(const XMLElement *xml, Request& request)
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

bool RequestDeviceControl::match(const std::string& name)
{
    return name == "DeviceControl";
}

bool RequestDeviceControl::dispatch(XMLElement *xml)
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

RequestPTZCmd::RequestPTZCmd(MANSCDPAgent *agent) : RequestSpecCmdTypes(agent)
{}

RequestPTZCmd::~RequestPTZCmd()
{}

bool RequestPTZCmd::deserialize(const XMLElement *xml, Request& request)
{
    if (!RequestDeviceControl::deserialize(xml, request))
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

bool RequestPTZCmd::match(XMLElement *xml)
{
    return xml->FirstChildElement("PTZCmd") != nullptr;
}

bool RequestPTZCmd::dispatch(XMLElement *xml)
{
    Request req;

    if (!deserialize(xml, req))
    {
        return false;
    }

    return m_agent->agentControl(req);
}

RequestDeviceConfig::RequestDeviceConfig(MANSCDPAgent *agent) : RequestControlCmdType(agent)
{}

RequestDeviceConfig::~RequestDeviceConfig()
{}

bool RequestDeviceConfig::match(const std::string& name)
{
    return name == "DeviceConfig";
}

bool RequestDeviceConfig::dispatch(XMLElement *xml)
{
    return false;
}