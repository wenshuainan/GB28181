#include "A.2.3Control.h"
#include "Agent/MANSCDPAgent.h"
#include "A.2.6Response.h"
#include "A.3FrontDeviceControl.h"
#include "MANSCDPDevice.h"

ControlReuest::ControlReuest(MANSCDPAgent *agent)
{
    printf("++++++ ControlReuest\n");
    spec.push_back(std::move(std::unique_ptr<DeviceControlRequest>(new DeviceControlRequest(agent))));
    spec.push_back(std::move(std::unique_ptr<DeviceConfigRequest>(new DeviceConfigRequest(agent))));
}

ControlReuest::~ControlReuest()
{
    printf("----- ControlReuest\n");
}

bool ControlReuest::match(const std::string& ReqType)
{
    return ReqType == "Control";
}

bool ControlReuest::dispatch(const XMLElement *xmlReq)
{
    printf("MANSCDP <Control> request\n");
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (!xmlCmdType)
    {
        printf("CmdType not found\n");
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

    printf("Not supported CmdType %s\n", xmlCmdType->GetText());
    return false;
}

DeviceControlRequest::DeviceControlRequest(MANSCDPAgent *agent)
{
    printf("++++++ DeviceControlRequest\n");
    spec.push_back(std::move(std::unique_ptr<PTZCmdControl>(new PTZCmdControl(agent))));
    spec.push_back(std::move(std::unique_ptr<TeleBootControl>(new TeleBootControl(agent))));
    spec.push_back(std::move(std::unique_ptr<RecordControl>(new RecordControl(agent))));
    spec.push_back(std::move(std::unique_ptr<GuardControl>(new GuardControl(agent))));
    spec.push_back(std::move(std::unique_ptr<AlarmControl>(new AlarmControl(agent))));
}

DeviceControlRequest::~DeviceControlRequest()
{
    printf("----- DeviceControlRequest\n");
}

bool DeviceControlRequest::parse(const XMLElement *xmlReq, Request& req)
{
    const XMLElement *xmlCmdType = xmlReq->FirstChildElement("CmdType");
    if (!xmlCmdType)
    {
        printf("CmdType not found\n");
        return false;
    }

    const XMLElement *xmlSN = xmlReq->FirstChildElement("SN");
    if (!xmlSN)
    {
        printf("SN not found\n");
        return false;
    }

    const XMLElement *xmlDeviceID = xmlReq->FirstChildElement("DeviceID");
    if (!xmlDeviceID)
    {
        printf("DeviceID not found\n");
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
    printf("MANSCDP <DeviceControl> Request\n");
    for (auto& i : spec)
    {
        if (i->match(xmlReq))
        {
            return i->handle(xmlReq);
        }
    }

    printf("Not handled DeviceControl Command\n");
    return false;
}

PTZCmdControl::PTZCmdControl(MANSCDPAgent *agent)
    : CmdTypeSpecRequest(agent)
{
    printf("++++++ PTZCmdControl\n");
}

PTZCmdControl::~PTZCmdControl()
{
    printf("----- PTZCmdControl\n");
}

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
        if (!req.PTZCmd.isValid())
        {
            printf("PTZCmd not valid\n");
            return false;
        }
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
    printf("MANSCDP <PTZCmd> request\n");
    Request req;
    if (!parse(xmlReq, req))
    {
        return false;
    }

    MANSCDPDevice *dev = m_agent->getDevice(req.DeviceID.getStr());
    if (!dev)
    {
        return false;
    }

    std::unique_ptr<CommandFormat> cmd = CommandFormat::create(req.PTZCmd.getCmd());
    if (cmd)
    {
        if (cmd->parse())
        {
            return dev->controlPTZ(dynamic_cast<PTZCommand*>(cmd.get()));
        }
    }
    return false;
}

TeleBootControl::TeleBootControl(MANSCDPAgent *agent)
    : CmdTypeSpecRequest(agent)
{
    printf("++++++ TeleBootControl\n");
}

TeleBootControl::~TeleBootControl()
{
    printf("----- TeleBootControl\n");
}

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
        if (req.TeleBoot.getStr() != "Boot")
        {
            printf("TeleBoot not valid\n");
            return false;
        }
    }

    return true;
}

bool TeleBootControl::match(const XMLElement *xmlReq)
{
    return xmlReq->FirstChildElement("TeleBoot") != nullptr;
}

bool TeleBootControl::handle(const XMLElement *xmlReq)
{
    printf("MANSCDP <TeleBoot> request\n");
    Request req;
    if (!parse(xmlReq, req))
    {
        return false;
    }

    MANSCDPDevice *dev = m_agent->getDevice(req.DeviceID.getStr());
    if (dev)
    {
        return dev->reboot();
    }
    return false;
}

RecordControl::RecordControl(MANSCDPAgent *agent)
    : CmdTypeSpecRequest(agent)
{
    printf("++++++ RecordControl\n");
}

RecordControl::~RecordControl()
{
    printf("----- RecordControl\n");
}

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
        if (!req.RecordCmd.isValid())
        {
            printf("RecordCmd not valid\n");
            return false;
        }
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
    printf("MANSCDP <RecordCmd> request\n");
    Request req;
    if (!parse(xmlReq, req))
    {
        return false;
    }

    MANSCDPDevice *dev = m_agent->getDevice(req.DeviceID.getStr());
    if (!dev)
    {
        return false;
    }

    std::shared_ptr<DeviceControlResponse> res = m_agent->createCmdMessage<DeviceControlResponse>(m_agent, req);
    if (!res)
    {
        return false;
    }
    int32_t recCmd = req.RecordCmd.getInt();
    if (recCmd == recordType::Record)
    {
        res->Result = dev->startRecord();
    }
    else if (recCmd == recordType::StopRecord)
    {
        res->Result = dev->stopRecord();
    }
    else
    {
        res->Result = resultType::ERROR;
    }
    return res->response();
}

GuardControl::GuardControl(MANSCDPAgent *agent)
    : CmdTypeSpecRequest(agent)
{
    printf("++++++ GuardControl\n");
}

GuardControl::~GuardControl()
{
    printf("----- GuardControl\n");
}

bool GuardControl::parse(const XMLElement *xmlReq, Request& req)
{
    if (!DeviceControlRequest::parse(xmlReq, req))
    {
        return false;
    }

    const XMLElement *xmlGuardCmd = xmlReq->FirstChildElement("GuardCmd");
    if (xmlGuardCmd)
    {
        req.GuardCmd = xmlGuardCmd->GetText();
        if (!req.GuardCmd.isValid())
        {
            printf("GuardCmd not valid\n");
            return false;
        }
    }

    return true;
}

bool GuardControl::match(const XMLElement *xmlReq)
{
    return xmlReq->FirstChildElement("GuardCmd") != nullptr;
}

bool GuardControl::handle(const XMLElement *xmlReq)
{
    printf("MANSCDP <GuardCmd> request\n");
    Request req;
    if (!parse(xmlReq, req))
    {
        return false;
    }

    MANSCDPDevice *dev = m_agent->getDevice(req.DeviceID.getStr());
    if (!dev)
    {
        return false;
    }

    std::shared_ptr<DeviceControlResponse> res = m_agent->createCmdMessage<DeviceControlResponse>(m_agent, req);
    if (!res)
    {
        return false;
    }
    int32_t guardCmd = req.GuardCmd.getInt();
    if (guardCmd == guardType::SetGuard)
    {
        res->Result = dev->setGuard();
    }
    else if (guardCmd == guardType::ResetGuard)
    {
        res->Result = dev->resetGuard();
    }
    else
    {
        res->Result = resultType::ERROR;
    }
    return res->response();
}

AlarmControl::AlarmControl(MANSCDPAgent *agent)
    : CmdTypeSpecRequest(agent)
{
    printf("++++++ AlarmControl\n");
}

AlarmControl::~AlarmControl()
{
    printf("------ AlarmControl\n");
}

bool AlarmControl::parse(const XMLElement *xmlReq, Request& req)
{
    if (!DeviceControlRequest::parse(xmlReq, req))
    {
        return false;
    }

    const XMLElement *xmlAlarmCmd = xmlReq->FirstChildElement("AlarmCmd");
    if (xmlAlarmCmd)
    {
        req.AlarmCmd = xmlAlarmCmd->GetText();
        if (req.AlarmCmd.getStr() != "ResetAlarm")
        {
            printf("AlarmCmd not valid\n");
            return false;
        }
    }

    const XMLElement *xmlInfo = xmlReq->FirstChildElement("Info");
    if (xmlInfo)
    {
        const XMLElement *xmlAlarmMethod = xmlInfo->FirstChildElement("AlarmMethod");
        if (xmlAlarmMethod)
        {
            req.Info.AlarmMethod = xmlAlarmMethod->GetText();
        }

        const XMLElement *xmlAlarmType = xmlInfo->FirstChildElement("AlarmType");
        if (xmlAlarmType)
        {
            req.Info.AlarmType = xmlAlarmType->GetText();
        }
    }

    return true;
}

bool AlarmControl::match(const XMLElement *xmlReq)
{
    return xmlReq->FirstChildElement("AlarmCmd") != nullptr;
}

bool AlarmControl::handle(const XMLElement *xmlReq)
{
    printf("MANSCDP <AlarmCmd> request\n");
    Request req;
    if (!parse(xmlReq, req))
    {
        return false;
    }

    MANSCDPDevice *dev = m_agent->getDevice(req.DeviceID.getStr());
    if (!dev)
    {
        return false;
    }

    std::shared_ptr<DeviceControlResponse> res = m_agent->createCmdMessage<DeviceControlResponse>(m_agent, req);
    if (!res)
    {
        return false;
    }
    if (req.Info.AlarmType.isValid())
    {
        res->Result = dev->resetAlarm(0, req.Info.AlarmType.getInt());
    }
    else
    {
        res->Result = resultType::ERROR;
    }
    return res->response();
}

DeviceConfigRequest::DeviceConfigRequest(MANSCDPAgent *agent)
{
    printf("++++++ DeviceConfigRequest\n");
}

DeviceConfigRequest::~DeviceConfigRequest()
{
    printf("------ DeviceConfigRequest\n");
}

bool DeviceConfigRequest::match(const std::string& CmdType)
{
    return CmdType == "DeviceConfig";
}

bool DeviceConfigRequest::dispatch(const XMLElement *xmlReq)
{
    printf("MANSCDP <DeviceConfig> Request\n");
    return false;
}