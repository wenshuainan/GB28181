#include "A.2.6Response.h"

bool CmdTypeResponse::encode(const Response& res, XMLDocument *xmldocRes)
{
    (void) res;
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

bool DeviceControlResponse::encode(const Response& res, XMLDocument *xmldocRes)
{
    if (!CmdTypeResponse::encode(res, xmldocRes))
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
    xmlSN->SetText(res.SN.getValue());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
    xmlDeviceID->SetText(res.DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlResult = xmldocRes->NewElement("Result");
    xmlResult->SetText(res.Result.getStr().c_str());
    rootElement->InsertEndChild(xmlResult);

    return true;
}

bool CatalogQueryResponse::encode(const Response& res, XMLDocument *xmldocRes)
{
    if (!CmdTypeResponse::encode(res, xmldocRes))
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
    xmlSN->SetText(res.SN.getValue());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
    xmlDeviceID->SetText(res.DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlSumNum = xmldocRes->NewElement("SumNum");
    xmlSumNum->SetText(res.SumNum.getValue());
    rootElement->InsertEndChild(xmlSumNum);

    XMLElement *xmlDeviceList = xmldocRes->NewElement("DeviceList");
    rootElement->InsertEndChild(xmlDeviceList);
    xmlDeviceList->SetAttribute("Num", res.DeviceList.Num.getValue());

    if (res.DeviceList.Item.size() > 0)
    {
        for (auto item : res.DeviceList.Item)
        {
            XMLElement *xmlItem = xmldocRes->NewElement("Item");
            xmlDeviceList->InsertEndChild(xmlItem);
            item.encode(xmlItem);
        }
    }

    return true;
}

bool DeviceInfoQueryResponse::encode(const Response& res, XMLDocument *xmldocRes)
{
    if (!CmdTypeResponse::encode(res, xmldocRes))
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
    xmlSN->SetText(res.SN.getValue());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
    xmlDeviceID->SetText(res.DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    if (res.DeviceName.isValid())
    {
        XMLElement *xmlDeviceName = xmldocRes->NewElement("DeviceName");
        xmlDeviceName->SetText(res.DeviceName.getStr().c_str());
        rootElement->InsertEndChild(xmlDeviceName);
    }

    XMLElement *xmlResult = xmldocRes->NewElement("Result");
    xmlResult->SetText(res.Result.getStr().c_str());
    rootElement->InsertEndChild(xmlResult);

    if (res.Manufacturer.isValid())
    {
        XMLElement *xmlManufacturer = xmldocRes->NewElement("Manufacturer");
        xmlManufacturer->SetText(res.Manufacturer.getStr().c_str());
        rootElement->InsertEndChild(xmlManufacturer);
    }

    if (res.Model.isValid())
    {
        XMLElement *xmlModel = xmldocRes->NewElement("Model");
        xmlModel->SetText(res.Model.getStr().c_str());
        rootElement->InsertEndChild(xmlModel);
    }

    if (res.Firmware.isValid())
    {
        XMLElement *xmlFirmware = xmldocRes->NewElement("Firmware");
        xmlFirmware->SetText(res.Firmware.getStr().c_str());
        rootElement->InsertEndChild(xmlFirmware);
    }

    if (res.Channel.isValid())
    {
        XMLElement *xmlChannel = xmldocRes->NewElement("Channel");
        xmlChannel->SetText(res.Channel.getValue());
        rootElement->InsertEndChild(xmlChannel);
    }

    return true;
}

bool RecordInfoQueryResponse::encode(const Response& res, XMLDocument *xmldocRes)
{
    if (!CmdTypeResponse::encode(res, xmldocRes))
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
    xmlSN->SetText(res.SN.getValue());
    rootElement->InsertEndChild(xmlSN);

    XMLElement *xmlDeviceID = xmldocRes->NewElement("DeviceID");
    xmlDeviceID->SetText(res.DeviceID.getStr().c_str());
    rootElement->InsertEndChild(xmlDeviceID);

    XMLElement *xmlName = xmldocRes->NewElement("Name");
    xmlName->SetText(res.Name.getStr().c_str());
    rootElement->InsertEndChild(xmlName);

    XMLElement *xmlSumNum = xmldocRes->NewElement("SumNum");
    xmlSumNum->SetText(res.SumNum.getValue());
    rootElement->InsertEndChild(xmlSumNum);

    XMLElement *xmlRecordList = xmldocRes->NewElement("RecordList");
    rootElement->InsertEndChild(xmlRecordList);
    xmlRecordList->SetAttribute("Num", res.RecordList.Num.getValue());

    if (res.RecordList.Item.size() > 0)
    {
        for (auto item : res.RecordList.Item)
        {
            XMLElement *xmlItem = xmldocRes->NewElement("Item");
            xmlRecordList->InsertEndChild(xmlItem);
            item.encode(xmlItem);
        }
    }

    return true;
}