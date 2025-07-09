#include "A.2.6Response.h"

bool DeviceControlResponse::serialize(const Response& res, XMLDocument *xmldocRes)
{
    XMLNode *rootElement = xmldocRes->FirstChild();
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
    XMLNode *rootElement = xmldocRes->FirstChild();
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
}

bool DeviceInfoQueryResponse::serialize(const Response& res, XMLDocument *xmldocRes)
{}