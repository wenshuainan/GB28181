#include "A.2.6Response.h"
#include "Agent/MANSCDPAgent.h"

bool CmdTypeResponse::encode(XMLDocument *xmldocRes)
{
    XMLDeclaration *dec = xmldocRes->NewDeclaration("xml version=\"1.0\"");
    if (dec != nullptr)
    {
        xmldocRes->InsertFirstChild(dec);
    }

    XMLElement *rootElement = xmldocRes->NewElement("Response");
    if (rootElement != nullptr)
    {
        return xmldocRes->InsertEndChild(rootElement) != nullptr;
    }
    else
    {
        printf("xml new <Response> element failed\n");
        return false;
    }
}

bool CmdTypeResponse::match(const XMLElement *cmd)
{
    const char *name = cmd->Name();
    return name && strcmp(name, "Response") == 0;
}

DeviceControlResponse::DeviceControlResponse(MANSCDPAgent *agent, const DeviceControlRequest::Request& req)
    : MessageResultHandler(agent)
{
    printf("++++++ DeviceControlResponse\n");
    CmdType = req.CmdType;
    SN = req.SN;
    DeviceID = req.DeviceID;
    Result = resultType::ERROR;
}

DeviceControlResponse::~DeviceControlResponse()
{
    printf("------ DeviceControlResponse\n");
}

bool DeviceControlResponse::encode(XMLDocument *xmldocRes)
{
    if (!CmdTypeResponse::encode(xmldocRes))
    {
        return false;
    }

    XMLElement *rootElement = xmldocRes->LastChildElement();
    if (rootElement == nullptr)
    {
        return false;
    }

    XMLElement *xmlCmdType = xmldocRes->NewElement("CmdType");
    xmlCmdType->SetText("DeviceControl");
    rootElement->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = xmldocRes->NewElement("SN");
    xmlSN->SetText(SN.getInt());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
    xmlDeviceID->SetText(DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlResult = xmldocRes->NewElement("Result");
    xmlResult->SetText(Result.getStr().c_str());
    rootElement->InsertEndChild(xmlResult);

    return true;
}

bool DeviceControlResponse::response(std::shared_ptr<MessageResultHandler> handler)
{
    XMLDocument res;
    if (!encode(&res))
    {
        return false;
    }
    return m_agent->sendCmd(res, handler);
}

bool DeviceControlResponse::match(const XMLElement *cmd)
{
    if (!CmdTypeResponse::match(cmd))
    {
        return false;
    }
    
    const XMLElement *xmlCmdType = cmd->FirstChildElement("CmdType");
    const XMLElement *xmlSN = cmd->FirstChildElement("SN");
    return xmlCmdType && this->CmdType == xmlCmdType->GetText()
        && xmlSN && this->SN == xmlSN->IntText();
}

bool DeviceControlResponse::handle(int32_t code)
{
    return true;
}

CatalogQueryResponse::CatalogQueryResponse(MANSCDPAgent *agent, const CatalogQuery::Request& req)
    : MessageResultHandler(agent)
{
    printf("++++++ CatalogQueryResponse\n");
    CmdType = req.CmdType;
    SN = req.SN;
    DeviceID = req.DeviceID;
    SumNum = 0;
    DeviceList.Num = 0;
}

CatalogQueryResponse::~CatalogQueryResponse()
{
    printf("------ CatalogQueryResponse\n");
}

bool CatalogQueryResponse::encode(XMLDocument *xmldocRes)
{
    if (!CmdTypeResponse::encode(xmldocRes))
    {
        return false;
    }

    XMLElement *rootElement = xmldocRes->LastChildElement();
    if (rootElement == nullptr)
    {
        return false;
    }

    XMLElement *xmlCmdType = xmldocRes->NewElement("CmdType");
    xmlCmdType->SetText("Catalog");
    rootElement->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = xmldocRes->NewElement("SN");
    xmlSN->SetText(SN.getInt());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
    xmlDeviceID->SetText(DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlSumNum = xmldocRes->NewElement("SumNum");
    xmlSumNum->SetText(SumNum.getInt());
    rootElement->InsertEndChild(xmlSumNum);

    if (SumNum > 0)
    {
        XMLElement *xmlDeviceList = xmldocRes->NewElement("DeviceList");
        rootElement->InsertEndChild(xmlDeviceList);
        xmlDeviceList->SetAttribute("Num", DeviceList.Num.getInt());

        for (auto item : DeviceList.Item)
        {
            XMLElement *xmlItem = xmldocRes->NewElement("Item");
            xmlDeviceList->InsertEndChild(xmlItem);
            item.encode(xmlItem);
        }
    }

    return true;
}

bool CatalogQueryResponse::response(std::shared_ptr<MessageResultHandler> handler)
{
    XMLDocument res;
    if (!encode(&res))
    {
        return false;
    }
    return m_agent->sendCmd(res, handler);
}

bool CatalogQueryResponse::match(const XMLElement *cmd)
{
    if (!CmdTypeResponse::match(cmd))
    {
        return false;
    }
    
    const XMLElement *xmlCmdType = cmd->FirstChildElement("CmdType");
    const XMLElement *xmlSN = cmd->FirstChildElement("SN");
    return xmlCmdType && this->CmdType == xmlCmdType->GetText()
        && xmlSN && this->SN == xmlSN->IntText();
}

bool CatalogQueryResponse::handle(int32_t code)
{
    printf("CatalogQueryResponse::handle code=%d\n", code);
    if (code == 200)
    {
        if (m_next)
        {
            return m_next->response(m_next);
        }
    }
    return true;
}

DeviceInfoQueryResponse::DeviceInfoQueryResponse(MANSCDPAgent *agent, const DeviceInfoQuery::Request& req)
    : MessageResultHandler(agent)
{
    printf("++++++ DeviceInfoQueryResponse\n");
    CmdType = req.CmdType;
    SN = req.SN;
    DeviceID = req.DeviceID;
    Result = resultType::ERROR;
}

DeviceInfoQueryResponse::~DeviceInfoQueryResponse()
{
    printf("------ DeviceInfoQueryResponse\n");
}

bool DeviceInfoQueryResponse::encode(XMLDocument *xmldocRes)
{
    if (!CmdTypeResponse::encode(xmldocRes))
    {
        return false;
    }

    XMLElement *rootElement = xmldocRes->LastChildElement();
    if (rootElement == nullptr)
    {
        return false;
    }

    XMLElement *xmlCmdType = xmldocRes->NewElement("CmdType");
    xmlCmdType->SetText("DeviceInfo");
    rootElement->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = xmldocRes->NewElement("SN");
    xmlSN->SetText(SN.getInt());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
    xmlDeviceID->SetText(DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    if (DeviceName.isValid())
    {
        XMLElement *xmlDeviceName = xmldocRes->NewElement("DeviceName");
        xmlDeviceName->SetText(DeviceName.getStr().c_str());
        rootElement->InsertEndChild(xmlDeviceName);
    }

    XMLElement *xmlResult = xmldocRes->NewElement("Result");
    xmlResult->SetText(Result.getStr().c_str());
    rootElement->InsertEndChild(xmlResult);

    if (Manufacturer.isValid())
    {
        XMLElement *xmlManufacturer = xmldocRes->NewElement("Manufacturer");
        xmlManufacturer->SetText(Manufacturer.getStr().c_str());
        rootElement->InsertEndChild(xmlManufacturer);
    }

    if (Model.isValid())
    {
        XMLElement *xmlModel = xmldocRes->NewElement("Model");
        xmlModel->SetText(Model.getStr().c_str());
        rootElement->InsertEndChild(xmlModel);
    }

    if (Firmware.isValid())
    {
        XMLElement *xmlFirmware = xmldocRes->NewElement("Firmware");
        xmlFirmware->SetText(Firmware.getStr().c_str());
        rootElement->InsertEndChild(xmlFirmware);
    }

    if (Channel.isValid())
    {
        XMLElement *xmlChannel = xmldocRes->NewElement("Channel");
        xmlChannel->SetText(Channel.getInt());
        rootElement->InsertEndChild(xmlChannel);
    }

    return true;
}

bool DeviceInfoQueryResponse::response(std::shared_ptr<MessageResultHandler> handler)
{
    XMLDocument res;
    if (!encode(&res))
    {
        return false;
    }
    return m_agent->sendCmd(res, handler);
}

bool DeviceInfoQueryResponse::match(const XMLElement *cmd)
{
    if (!CmdTypeResponse::match(cmd))
    {
        return false;
    }
    
    const XMLElement *xmlCmdType = cmd->FirstChildElement("CmdType");
    const XMLElement *xmlSN = cmd->FirstChildElement("SN");
    return xmlCmdType && this->CmdType == xmlCmdType->GetText()
        && xmlSN && this->SN == xmlSN->IntText();
}

bool DeviceInfoQueryResponse::handle(int32_t code)
{
    return true;
}

DeviceStatusQueryResponse::DeviceStatusQueryResponse(MANSCDPAgent *agent, const DeviceStatusQuery::Request& req)
    : MessageResultHandler(agent)
{
    printf("++++++ DeviceStatusQueryResponse\n");
    CmdType = req.CmdType;
    SN = req.SN;
    DeviceID = req.DeviceID;
    AlarmStatus.Num = 0;
}

DeviceStatusQueryResponse::~DeviceStatusQueryResponse()
{
    printf("------ DeviceStatusQueryResponse\n");
}

bool DeviceStatusQueryResponse::encode(XMLDocument *xmldocRes)
{
    if (!CmdTypeResponse::encode(xmldocRes))
    {
        return false;
    }
    
    XMLElement *rootElement = xmldocRes->LastChildElement();
    if (rootElement == nullptr)
    {
        return false;
    }
    
    XMLElement *xmlCmdType = xmldocRes->NewElement("CmdType");
    xmlCmdType->SetText("DeviceStatus");
    rootElement->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = xmldocRes->NewElement("SN");
    xmlSN->SetText(SN.getInt());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
    xmlDeviceID->SetText(DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlResult = xmldocRes->NewElement("Result");
    xmlResult->SetText(Result.getStr().c_str());
    rootElement->InsertEndChild(xmlResult);

    XMLElement *xmlOnline = xmldocRes->NewElement("Online");
    switch (Online)
    {
    case ONLINE:
        xmlOnline->SetText("ONLINE");
        break;
    case OFFLINE:
        xmlOnline->SetText("OFFLINE");
        break;
    
    default:
        xmlOnline->SetText("OFFLINE");
        break;
    }
    rootElement->InsertEndChild(xmlOnline);

    XMLElement *xmlStatus = xmldocRes->NewElement("Status");
    xmlStatus->SetText(Status.getStr().c_str());
    rootElement->InsertEndChild(xmlStatus);

    if (Reason.isValid())
    {
        XMLElement *xmlReason = xmldocRes->NewElement("Reason");
        xmlReason->SetText(Reason.getStr().c_str());
        rootElement->InsertEndChild(xmlReason);
    }

    if (Encode.isValid())
    {
        XMLElement *xmlEncode = xmldocRes->NewElement("Encode");
        xmlEncode->SetText(Encode.getStr().c_str());
        rootElement->InsertEndChild(xmlEncode);
    }

    if (Record.isValid())
    {
        XMLElement *xmlRecord = xmldocRes->NewElement("Record");
        xmlRecord->SetText(Record.getStr().c_str());
        rootElement->InsertEndChild(xmlRecord);
    }

    if (DeviceTime.isValid())
    {
        XMLElement *xmlDeviceTime = xmldocRes->NewElement("DeviceTime");
        xmlDeviceTime->SetText(DeviceTime.getStr().c_str());
        rootElement->InsertEndChild(xmlDeviceTime);
    }

    if (AlarmStatus.Num > 0)
    {
        XMLElement *xmlAlarmStatus = xmldocRes->NewElement("AlarmStatus");
        xmlAlarmStatus->SetAttribute("Num", AlarmStatus.Num.getInt());
        rootElement->InsertEndChild(xmlAlarmStatus);

        for (auto& i : AlarmStatus.Item)
        {
            XMLElement *xmlItem = xmldocRes->NewElement("Item");
            xmlAlarmStatus->InsertEndChild(xmlItem);
            XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
            xmlDeviceID->SetText(i.DeviceID.getStr().c_str());
            xmlItem->InsertEndChild(xmlDeviceID);
            XMLElement *xmlDutyStatus = xmldocRes->NewElement("DutyStatus");
            switch (i.DutyStatus)
            {
            case _AlarmStatus::_Item::ONDUTY:
                xmlDutyStatus->SetText("ONDUTY");
                break;
            case _AlarmStatus::_Item::OFFDUTY:
                xmlDutyStatus->SetText("OFFDUTY");
                break;
            case _AlarmStatus::_Item::ALARM:
                xmlDutyStatus->SetText("ALARM");
                break;
            
            default:
                xmlDutyStatus->SetText("OFFDUTY");
                break;
            }
            xmlItem->InsertEndChild(xmlDutyStatus);
        }
    }
    
    return true;
}

bool DeviceStatusQueryResponse::response(std::shared_ptr<MessageResultHandler> handler)
{
    XMLDocument res;
    if (!encode(&res))
    {
        return false;
    }
    return m_agent->sendCmd(res, handler);
}

bool DeviceStatusQueryResponse::match(const XMLElement *cmd)
{
    if (!CmdTypeResponse::match(cmd))
    {
        return false;
    }
    
    const XMLElement *xmlCmdType = cmd->FirstChildElement("CmdType");
    const XMLElement *xmlSN = cmd->FirstChildElement("SN");
    return xmlCmdType && this->CmdType == xmlCmdType->GetText()
        && xmlSN && this->SN == xmlSN->IntText();
}

bool DeviceStatusQueryResponse::handle(int32_t code)
{
    return true;
}

RecordInfoQueryResponse::RecordInfoQueryResponse(MANSCDPAgent *agent, const RecordInfoQuery::Request& req)
    : MessageResultHandler(agent)
{
    printf("++++++ RecordInfoQueryResponse\n");
    CmdType = req.CmdType;
    SN = req.SN;
    DeviceID = req.DeviceID;
    SumNum = 0;
    RecordList.Num = 0;
}

RecordInfoQueryResponse::~RecordInfoQueryResponse()
{
    printf("------ RecordInfoQueryResponse\n");
}

bool RecordInfoQueryResponse::encode(XMLDocument *xmldocRes)
{
    if (!CmdTypeResponse::encode(xmldocRes))
    {
        return false;
    }

    XMLElement *rootElement = xmldocRes->LastChildElement();
    if (rootElement == nullptr)
    {
        return false;
    }

    XMLElement *xmlCmdType = xmldocRes->NewElement("CmdType");
    xmlCmdType->SetText("RecordInfo");
    rootElement->InsertEndChild(xmlCmdType);

    XMLElement *xmlSN = xmldocRes->NewElement("SN");
    xmlSN->SetText(SN.getInt());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
    xmlDeviceID->SetText(DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlName = xmldocRes->NewElement("Name");
    xmlName->SetText(Name.getStr().c_str());
    rootElement->InsertEndChild(xmlName);

    XMLElement *xmlSumNum = xmldocRes->NewElement("SumNum");
    xmlSumNum->SetText(SumNum.getInt());
    rootElement->InsertEndChild(xmlSumNum);

    if (SumNum > 0)
    {
        XMLElement *xmlRecordList = xmldocRes->NewElement("RecordList");
        rootElement->InsertEndChild(xmlRecordList);
        xmlRecordList->SetAttribute("Num", RecordList.Num.getInt());

        for (auto item : RecordList.Item)
        {
            XMLElement *xmlItem = xmldocRes->NewElement("Item");
            xmlRecordList->InsertEndChild(xmlItem);
            item.encode(xmlItem);
        }
    }

    return true;
}

bool RecordInfoQueryResponse::response(std::shared_ptr<MessageResultHandler> handler)
{
    XMLDocument res;
    if (!encode(&res))
    {
        return false;
    }
    return m_agent->sendCmd(res, handler);
}

bool RecordInfoQueryResponse::match(const XMLElement *cmd)
{
    if (!CmdTypeResponse::match(cmd))
    {
        return false;
    }
    
    const XMLElement *xmlCmdType = cmd->FirstChildElement("CmdType");
    const XMLElement *xmlSN = cmd->FirstChildElement("SN");
    return xmlCmdType && this->CmdType == xmlCmdType->GetText()
        && xmlSN && this->SN == xmlSN->IntText();
}

bool RecordInfoQueryResponse::handle(int32_t code)
{
    printf("RecordInfoQueryResponse::handle code=%d\n", code);
    if (code == 200)
    {
        if (m_next)
        {
            return m_next->response(m_next);
        }
    }
    return true;
}