#include "A.2.6Response.h"

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
        return false;
    }
}

DeviceControlResponse::DeviceControlResponse(const DeviceControlRequest::Request& req)
{
    CmdType = req.CmdType;
    SN = req.SN;
    DeviceID = req.DeviceID;
    Result = resultType::ERROR;
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
    xmlSN->SetText(SN.getValue());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
    xmlDeviceID->SetText(DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlResult = xmldocRes->NewElement("Result");
    xmlResult->SetText(Result.getStr().c_str());
    rootElement->InsertEndChild(xmlResult);

    return true;
}

CatalogQueryResponse::CatalogQueryResponse(const CatalogQuery::Request& req)
{
    CmdType = req.CmdType;
    SN = req.SN;
    DeviceID = req.DeviceID;
    SumNum = 0;
    DeviceList.Num = 0;
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
    xmlSN->SetText(SN.getValue());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
    xmlDeviceID->SetText(DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlSumNum = xmldocRes->NewElement("SumNum");
    xmlSumNum->SetText(SumNum.getValue());
    rootElement->InsertEndChild(xmlSumNum);

    XMLElement *xmlDeviceList = xmldocRes->NewElement("DeviceList");
    rootElement->InsertEndChild(xmlDeviceList);
    xmlDeviceList->SetAttribute("Num", DeviceList.Num.getValue());

    if (DeviceList.Item.size() > 0)
    {
        for (auto item : DeviceList.Item)
        {
            XMLElement *xmlItem = xmldocRes->NewElement("Item");
            xmlDeviceList->InsertEndChild(xmlItem);
            item.encode(xmlItem);
        }
    }

    return true;
}

DeviceInfoQueryResponse::DeviceInfoQueryResponse(const DeviceInfoQuery::Request& req)
{
    CmdType = req.CmdType;
    SN = req.SN;
    DeviceID = req.DeviceID;
    Result = resultType::ERROR;
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
    xmlSN->SetText(SN.getValue());
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
        xmlChannel->SetText(Channel.getValue());
        rootElement->InsertEndChild(xmlChannel);
    }

    return true;
}

RecordInfoQueryResponse::RecordInfoQueryResponse(const RecordInfoQuery::Request& req)
{
    CmdType = req.CmdType;
    SN = req.SN;
    DeviceID = req.DeviceID;
    SumNum = 0;
    RecordList.Num = 0;
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
    xmlSN->SetText(SN.getValue());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
    xmlDeviceID->SetText(DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlName = xmldocRes->NewElement("Name");
    xmlName->SetText(Name.getStr().c_str());
    rootElement->InsertEndChild(xmlName);

    XMLElement *xmlSumNum = xmldocRes->NewElement("SumNum");
    xmlSumNum->SetText(SumNum.getValue());
    rootElement->InsertEndChild(xmlSumNum);

    XMLElement *xmlRecordList = xmldocRes->NewElement("RecordList");
    rootElement->InsertEndChild(xmlRecordList);
    xmlRecordList->SetAttribute("Num", RecordList.Num.getValue());

    if (RecordList.Item.size() > 0)
    {
        for (auto item : RecordList.Item)
        {
            XMLElement *xmlItem = xmldocRes->NewElement("Item");
            xmlRecordList->InsertEndChild(xmlItem);
            item.encode(xmlItem);
        }
    }

    return true;
}