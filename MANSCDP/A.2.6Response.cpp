#include "A.2.6Response.h"

bool CmdTypeResponse::serialize(const Response& res, XMLDocument *xmldocRes)
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

bool DeviceControlResponse::serialize(const Response& res, XMLDocument *xmldocRes)
{
    if (!CmdTypeResponse::serialize(res, xmldocRes))
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

bool CatalogQueryResponse::serialize(const Response& res, XMLDocument *xmldocRes)
{
    if (!CmdTypeResponse::serialize(res, xmldocRes))
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

    if (res.DeviceList.item.size() > 0)
    {
        for (auto item : res.DeviceList.item)
        {
            XMLElement *xmlItem = xmldocRes->NewElement("Item");
            xmlDeviceList->InsertEndChild(xmlItem);
            item.toXml(xmlItem);
        }
    }

    return true;
}

bool DeviceInfoQueryResponse::serialize(const Response& res, XMLDocument *xmldocRes)
{
    if (!CmdTypeResponse::serialize(res, xmldocRes))
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