#include "A.2.3Control.h"

ControlReuest::ControlReuest()
{
    element.push_back(new DeviceControlRequest());
    element.push_back(new DeviceConfigRequest());
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

bool ControlReuest::process(XMLElement& ele)
{
    for (auto& i : element)
    {
        if (i->match(ele.FirstChildElement("CmdType")->GetText()))
        {
            return i->process(ele);
        }
    }

    return false;
}

DeviceControlRequest::DeviceControlRequest()
{
    element.push_back(new PTZCmdRequest());
}

DeviceControlRequest::~DeviceControlRequest()
{
    for (auto& i : element)
    {
        delete i;
    }
}

bool DeviceControlRequest::deserialize(const XMLElement& xml, Request& request)
{
    return false;
}

bool DeviceControlRequest::match(const std::string& name)
{
    return name == "DeviceControl";
}

bool DeviceControlRequest::process(XMLElement& ele)
{
    for (auto& i : element)
    {
        if (i->match(ele))
        {
            return i->process(ele);
        }
    }

    return false;
}

PTZCmdRequest::PTZCmdRequest()
{}

PTZCmdRequest::~PTZCmdRequest()
{}

bool PTZCmdRequest::deserialize(const XMLElement& xml, Request& request)
{
    return false;
}

bool PTZCmdRequest::match(XMLElement& ele)
{
    return ele.FirstChildElement("PTZCmd") != nullptr;
}

bool PTZCmdRequest::process(XMLElement& ele)
{}

DeviceConfigRequest::DeviceConfigRequest()
{}

DeviceConfigRequest::~DeviceConfigRequest()
{}

bool DeviceConfigRequest::match(const std::string& name)
{
    return name == "DeviceConfig";
}

bool DeviceConfigRequest::process(XMLElement& ele)
{}